#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path


MPI_PACKAGES_LINUX = [
    "mpich",
    "libmpich-dev",
    "openmpi-bin",
    "openmpi-common",
    "libopenmpi-dev",
    "mpi-default-bin",
    "mpi-default-dev",
]
MPI_PACKAGES_MACOS = ["mpich", "open-mpi"]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Set up mpi-extensions Open MPI for GitHub Actions.")
    parser.add_argument("--repo-root", required=True, type=Path)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--platform", required=True)
    parser.add_argument("--purge-system-mpi", choices=["true", "false"], required=True)
    parser.add_argument("--github-env", required=True, type=Path)
    parser.add_argument("--github-path", required=True, type=Path)
    parser.add_argument("--runner-temp", required=True, type=Path)
    return parser.parse_args()


def run(command: list[str], *, env: dict[str, str] | None = None, check: bool = True) -> subprocess.CompletedProcess[str]:
    print("+ " + " ".join(command), flush=True)
    return subprocess.run(command, check=check, env=env)


def sudo_prefix() -> list[str] | None:
    if os.geteuid() == 0:
        return []
    if shutil.which("sudo"):
        return ["sudo"]
    return None


def purge_linux_mpi() -> None:
    if not shutil.which("apt-get"):
        return
    sudo = sudo_prefix()
    if sudo is None:
        print("::warning::sudo is unavailable; skipping Linux MPI package removal")
        return

    apt_get = sudo + ["apt-get"]
    run(apt_get + ["update"])
    run(apt_get + ["purge", "-y"] + MPI_PACKAGES_LINUX, check=False)
    run(apt_get + ["autoremove", "-y"], check=False)
    if os.geteuid() == 0:
        shutil.rmtree("/var/lib/apt/lists", ignore_errors=True)


def purge_macos_mpi() -> None:
    if not shutil.which("brew"):
        return
    for package in MPI_PACKAGES_MACOS:
        if subprocess.run(["brew", "list", "--versions", package], check=False).returncode == 0:
            run(["brew", "uninstall", "--ignore-dependencies", "--force", package])


def purge_system_mpi(enabled: bool) -> None:
    if not enabled:
        return
    system = platform.system()
    if system == "Linux":
        purge_linux_mpi()
    elif system == "Darwin":
        purge_macos_mpi()


def prepare_python(runner_temp: Path) -> Path:
    venv_dir = runner_temp / "mpi-extensions-python"
    python_bin = venv_dir / "bin" / "python"
    try:
        run([sys.executable, "-m", "venv", str(venv_dir)])
        run([str(python_bin), "-m", "pip", "install", "--upgrade", "pip"])
        run([str(python_bin), "-m", "pip", "install", "requests>=2.31,<3"])
        return python_bin
    except subprocess.CalledProcessError:
        print("::warning::Python venv setup failed; trying the current interpreter with pip")
        if subprocess.run([sys.executable, "-m", "pip", "install", "--user", "requests>=2.31,<3"], check=False).returncode != 0:
            run([sys.executable, "-m", "pip", "install", "--break-system-packages", "requests>=2.31,<3"])
        return Path(sys.executable)


def install_openmpi(args: argparse.Namespace, python_bin: Path) -> None:
    installer = args.repo_root / "scripts" / "install_mpi_extensions.py"
    if not installer.exists():
        raise SystemExit(f"mpi-extensions installer not found: {installer}")
    run(
        [
            str(python_bin),
            str(installer),
            "--platform",
            args.platform,
            "--prefix",
            str(args.prefix),
            "--force",
        ]
    )


def append_github_environment(prefix: Path, github_env: Path, github_path: Path) -> None:
    lib_path = prefix / "lib"
    env_lines = [
        f"MPI_EXTENSIONS_HOME={prefix}",
        f"MPI_HOME={prefix}",
        f"OPAL_PREFIX={prefix}",
        "OMPI_MCA_shmem=mmap",
        f"LD_LIBRARY_PATH={lib_path}:{os.environ.get('LD_LIBRARY_PATH', '')}",
        f"DYLD_LIBRARY_PATH={lib_path}:{os.environ.get('DYLD_LIBRARY_PATH', '')}",
    ]
    with github_env.open("a", encoding="utf-8") as output:
        for line in env_lines:
            output.write(line + "\n")
    with github_path.open("a", encoding="utf-8") as output:
        output.write(str(prefix / "bin") + "\n")


def validate_openmpi(prefix: Path) -> None:
    expected_bin = prefix / "bin"
    env = os.environ.copy()
    env["MPI_EXTENSIONS_HOME"] = str(prefix)
    env["MPI_HOME"] = str(prefix)
    env["OPAL_PREFIX"] = str(prefix)
    env["OMPI_MCA_shmem"] = "mmap"
    env["PATH"] = f"{expected_bin}:{env.get('PATH', '')}"
    env["LD_LIBRARY_PATH"] = f"{prefix / 'lib'}:{env.get('LD_LIBRARY_PATH', '')}"
    env["DYLD_LIBRARY_PATH"] = f"{prefix / 'lib'}:{env.get('DYLD_LIBRARY_PATH', '')}"

    for tool in ("mpicc", "mpicxx", "mpirun", "mpiexec"):
        actual = shutil.which(tool, path=env["PATH"])
        expected = expected_bin / tool
        if actual != str(expected):
            raise SystemExit(f"{tool} resolves to {actual} instead of {expected}")

    run([str(expected_bin / "mpirun"), "--version"], env=env)
    run([str(expected_bin / "mpicc"), "--showme:version"], env=env)


def main() -> int:
    args = parse_args()
    prefix = args.prefix.resolve()
    purge_system_mpi(args.purge_system_mpi == "true")
    python_bin = prepare_python(args.runner_temp)
    install_openmpi(args, python_bin)
    append_github_environment(prefix, args.github_env, args.github_path)
    validate_openmpi(prefix)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
