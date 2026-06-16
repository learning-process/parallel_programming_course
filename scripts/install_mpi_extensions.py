#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
from pathlib import Path, PurePosixPath
from typing import Any

try:
    import requests
except ImportError as exc:
    raise SystemExit(
        "Missing Python dependency 'requests'. Install it with: "
        "python3 -m pip install 'requests>=2.31,<3'"
    ) from exc


DEFAULT_REPO = "learning-process/mpi-extensions"
RELEASE_TAG = "main"
SUPPORTED_PLATFORMS = {
    "linux-x86_64",
    "linux-arm64",
    "macos-arm64",
    "macos-x86_64",
}


def normalize_machine(machine: str) -> str:
    normalized = machine.lower()
    if normalized in {"x86_64", "amd64"}:
        return "x86_64"
    if normalized in {"arm64", "aarch64"}:
        return "arm64"
    return normalized


def detect_platform() -> str:
    system = platform.system()
    machine = normalize_machine(platform.machine())
    if system == "Linux":
        return f"linux-{machine}"
    if system == "Darwin":
        return f"macos-{machine}"
    raise SystemExit(f"Unsupported operating system for mpi-extensions: {system}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Install Open MPI from the mpi-extensions main prerelease."
    )
    parser.add_argument("--repo", default=DEFAULT_REPO, help=f"default: {DEFAULT_REPO}")
    parser.add_argument(
        "--platform",
        default="auto",
        help="auto, linux-x86_64, linux-arm64, macos-arm64, or macos-x86_64",
    )
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument(
        "--force",
        action="store_true",
        help="remove an existing installation prefix before extracting",
    )
    return parser.parse_args()


def github_headers() -> dict[str, str]:
    headers = {
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
        "User-Agent": "parallel-programming-course-mpi-extensions-installer",
    }
    token = os.environ.get("GITHUB_TOKEN")
    if token:
        headers["Authorization"] = f"Bearer {token}"
    return headers


def get_json(session: requests.Session, url: str) -> dict[str, Any]:
    response = session.get(url, timeout=60)
    if response.status_code == 404:
        raise SystemExit(f"GitHub resource not found: {url}")
    response.raise_for_status()
    data = response.json()
    if not isinstance(data, dict):
        raise SystemExit(f"Unexpected GitHub API response from {url}")
    return data


def select_asset(release: dict[str, Any], asset_name: str) -> dict[str, Any]:
    for asset in release.get("assets", []):
        if asset.get("name") == asset_name:
            return asset
    available = ", ".join(
        asset.get("name", "<unnamed>") for asset in release.get("assets", [])
    )
    raise SystemExit(
        f"Release asset not found: {asset_name}. Available assets: {available}"
    )


def select_archive_asset(
    release: dict[str, Any], target_platform: str
) -> dict[str, Any]:
    pattern = re.compile(
        rf"^mpi-extensions-openmpi-.+-{re.escape(target_platform)}\.tar\.gz$"
    )
    matches = [
        asset
        for asset in release.get("assets", [])
        if pattern.match(str(asset.get("name", "")))
    ]
    if len(matches) != 1:
        available = ", ".join(
            asset.get("name", "<unnamed>") for asset in release.get("assets", [])
        )
        raise SystemExit(
            f"Expected exactly one main release archive for {target_platform}, "
            f"found {len(matches)}. Available assets: {available}"
        )
    return matches[0]


def download(session: requests.Session, url: str, destination: Path) -> None:
    with session.get(url, stream=True, timeout=300) as response:
        response.raise_for_status()
        with destination.open("wb") as output:
            for chunk in response.iter_content(chunk_size=1024 * 1024):
                if chunk:
                    output.write(chunk)


def sha256sum(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as input_file:
        for chunk in iter(lambda: input_file.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def path_is_inside(path: Path, directory: Path) -> bool:
    return path == directory or directory in path.parents


def archive_member_path(member: tarfile.TarInfo) -> PurePosixPath:
    member_path = PurePosixPath(member.name)
    if member_path.is_absolute() or not member_path.parts:
        raise SystemExit(f"Archive contains unsafe path: {member.name}")
    if any(part in {"", ".", ".."} for part in member_path.parts):
        raise SystemExit(f"Archive contains unsafe path: {member.name}")
    return member_path


def validate_archive_member_type(member: tarfile.TarInfo) -> None:
    if member.isdir() or member.isfile() or member.issym() or member.islnk():
        return
    raise SystemExit(f"Archive contains unsupported member type: {member.name}")


def validate_archive_link(
    member: tarfile.TarInfo,
    member_path: PurePosixPath,
    destination: Path,
    resolved_destination: Path,
) -> None:
    if not (member.issym() or member.islnk()):
        return
    if not member.linkname:
        raise SystemExit(f"Archive contains empty link target: {member.name}")

    link_path = PurePosixPath(member.linkname)
    if link_path.is_absolute():
        raise SystemExit(
            f"Archive link target is absolute: {member.name} -> {member.linkname}"
        )

    member_target = (destination / Path(*member_path.parts)).resolve(strict=False)
    if member.issym():
        link_target = (member_target.parent / member.linkname).resolve(strict=False)
    else:
        link_target = (destination / member.linkname).resolve(strict=False)

    if not path_is_inside(link_target, resolved_destination):
        raise SystemExit(
            f"Archive link target escapes destination: "
            f"{member.name} -> {member.linkname}"
        )


def safe_extract(archive: Path, destination: Path) -> Path:
    destination.mkdir(parents=True, exist_ok=True)
    resolved_destination = destination.resolve()
    with tarfile.open(archive, "r:gz") as tar:
        members = tar.getmembers()
        roots = set()
        member_paths = []
        for member in members:
            member_path = archive_member_path(member)
            validate_archive_member_type(member)
            roots.add(member_path.parts[0])
            member_paths.append((member, member_path))
            target = (destination / Path(*member_path.parts)).resolve(strict=False)
            if not path_is_inside(target, resolved_destination):
                raise SystemExit(f"Archive contains unsafe path: {member.name}")
            validate_archive_link(
                member, member_path, destination, resolved_destination
            )

        symlink_paths = {
            member_path for member, member_path in member_paths if member.issym()
        }
        for member, member_path in member_paths:
            for symlink_path in symlink_paths:
                if symlink_path in member_path.parents:
                    raise SystemExit(
                        f"Archive member is nested under a symlink: {member.name}"
                    )

        if len(roots) != 1:
            raise SystemExit(
                f"Expected archive to contain one top-level directory, found: {roots}"
            )
        tar.extractall(destination, members=members)
    return destination / roots.pop()


def install_tree(source_root: Path, prefix: Path, force: bool) -> None:
    required_tools = ["mpicc", "mpicxx", "mpirun", "mpiexec"]
    missing_tools = [
        tool for tool in required_tools if not (source_root / "bin" / tool).exists()
    ]
    if missing_tools:
        raise SystemExit(
            f"Archive is missing required MPI tools: {', '.join(missing_tools)}"
        )

    if prefix.exists():
        if not force and any(prefix.iterdir()):
            raise SystemExit(
                f"Installation prefix is not empty: {prefix}. Use --force to replace it."
            )
        shutil.rmtree(prefix)
    prefix.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source_root, prefix, symlinks=True)


def run_tool(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )


def otool_lines(path: Path) -> list[str]:
    result = run_tool(["otool", "-L", str(path)])
    if result.returncode != 0:
        return []
    return result.stdout.splitlines()[1:]


def patch_macos_install_names(prefix: Path) -> None:
    if platform.system() != "Darwin":
        return
    if (
        not shutil.which("install_name_tool")
        or not shutil.which("otool")
        or not shutil.which("codesign")
    ):
        raise SystemExit(
            "macOS install_name_tool, otool, and codesign are required to install mpi-extensions"
        )

    lib_dir = prefix / "lib"
    if not lib_dir.exists():
        return

    local_libs = {
        path.name: path.resolve()
        for path in lib_dir.iterdir()
        if path.is_file() and not path.is_symlink() and ".dylib" in path.name
    }
    mach_o_candidates = [
        path
        for base_dir in (prefix / "bin", lib_dir)
        if base_dir.exists()
        for path in base_dir.iterdir()
        if path.is_file() and not path.is_symlink()
    ]
    patched_paths: set[Path] = set()

    for path in local_libs.values():
        result = run_tool(["otool", "-D", str(path)])
        if result.returncode != 0:
            continue
        install_names = [
            line.strip() for line in result.stdout.splitlines()[1:] if line.strip()
        ]
        if install_names:
            local_name = local_libs.get(Path(install_names[0]).name)
            if local_name:
                replacement = f"@rpath/{local_name.name}"
                if install_names[0] != replacement:
                    subprocess.run(
                        [
                            "install_name_tool",
                            "-id",
                            replacement,
                            str(path),
                        ],
                        check=True,
                    )
                    patched_paths.add(path)

    for path in mach_o_candidates:
        for line in otool_lines(path):
            dependency = line.strip().split(" (", 1)[0]
            if not dependency.startswith("/"):
                continue
            local_dependency = local_libs.get(Path(dependency).name)
            if local_dependency and dependency != str(local_dependency):
                if path.parent.resolve() == lib_dir.resolve():
                    replacement = f"@loader_path/{local_dependency.name}"
                elif path.parent.resolve() == (prefix / "bin").resolve():
                    replacement = f"@loader_path/../lib/{local_dependency.name}"
                else:
                    replacement = f"@rpath/{local_dependency.name}"
                subprocess.run(
                    [
                        "install_name_tool",
                        "-change",
                        dependency,
                        replacement,
                        str(path),
                    ],
                    check=True,
                )
                patched_paths.add(path)

    for path in patched_paths:
        subprocess.run(
            ["codesign", "--force", "--sign", "-", "--timestamp=none", str(path)],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )


def print_environment(prefix: Path) -> None:
    prefix_text = str(prefix)
    print(f"PPC_MPI_EXTENSIONS_HOME={prefix_text}")
    print(f"CMake option: -DPPC_MPI_EXTENSIONS_HOME={prefix_text}")


def main() -> int:
    args = parse_args()
    target_platform = detect_platform() if args.platform == "auto" else args.platform
    if target_platform not in SUPPORTED_PLATFORMS:
        raise SystemExit(
            f"Unsupported platform '{target_platform}'. Supported values: "
            f"{', '.join(sorted(SUPPORTED_PLATFORMS))}"
        )

    session = requests.Session()
    session.headers.update(github_headers())
    release_url = (
        f"https://api.github.com/repos/{args.repo}/releases/tags/{RELEASE_TAG}"
    )
    release = get_json(session, release_url)
    if release.get("tag_name") != RELEASE_TAG or not release.get("prerelease"):
        raise SystemExit(
            f"Release {args.repo}@{RELEASE_TAG} is not the main prerelease"
        )

    archive_asset = select_archive_asset(release, target_platform)
    archive_name = archive_asset["name"]
    checksum_asset = select_asset(release, f"{archive_name}.sha256")

    with tempfile.TemporaryDirectory(prefix="mpi-extensions-") as temp_dir_text:
        temp_dir = Path(temp_dir_text)
        archive_path = temp_dir / archive_name
        checksum_path = temp_dir / f"{archive_name}.sha256"
        print(
            f"Downloading {archive_name} from {args.repo}@{RELEASE_TAG}",
            file=sys.stderr,
        )
        download(session, archive_asset["browser_download_url"], archive_path)
        download(session, checksum_asset["browser_download_url"], checksum_path)

        expected_sha = checksum_path.read_text(encoding="utf-8").split()[0]
        actual_sha = sha256sum(archive_path)
        if actual_sha != expected_sha:
            raise SystemExit(
                f"SHA256 mismatch for {archive_name}: expected {expected_sha}, got {actual_sha}"
            )

        source_root = safe_extract(archive_path, temp_dir / "extract")
        prefix = args.prefix.resolve()
        install_tree(source_root, prefix, args.force)
        patch_macos_install_names(prefix)

    print_environment(args.prefix.resolve())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
