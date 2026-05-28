#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path, PurePosixPath

FULL_SUITE = "all"


def _changed_files(base_sha: str, head_sha: str) -> list[str]:
    result = subprocess.run(
        ["git", "diff", "--name-only", "--no-renames", f"{base_sha}...{head_sha}"],
        check=True,
        stdout=subprocess.PIPE,
        text=True,
    )
    return [line for line in result.stdout.splitlines() if line]


def _task_from_path(path: str, tasks_root: Path) -> str | None:
    parts = PurePosixPath(path).parts
    if len(parts) < 3 or parts[0] != "tasks" or parts[1] == "common":
        return None

    task_id = parts[1]
    if not (tasks_root / task_id).is_dir():
        return None

    return task_id


def detect_scope(changed_files: list[str], tasks_root: Path) -> tuple[str, bool]:
    task_ids = set()

    for changed_file in changed_files:
        task_id = _task_from_path(changed_file, tasks_root)
        if task_id is None:
            return FULL_SUITE, False
        task_ids.add(task_id)

    if not task_ids:
        return FULL_SUITE, False

    return ";".join(sorted(task_ids)), True


def _write_github_output(github_output: Path, scope: str, task_scoped: bool) -> None:
    with github_output.open("a", encoding="utf-8") as output:
        output.write(f"ppc_tasks={scope}\n")
        output.write(f"task_scoped={str(task_scoped).lower()}\n")


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Detect the PPC_TASKS value for CI from changed PR paths.",
    )
    parser.add_argument(
        "--base-sha", required=True, help="Base commit for the PR diff."
    )
    parser.add_argument(
        "--head-sha", required=True, help="Head commit for the PR diff."
    )
    parser.add_argument(
        "--tasks-root", default="tasks", type=Path, help="Path to the tasks directory."
    )
    parser.add_argument(
        "--github-output",
        type=Path,
        help="Optional GITHUB_OUTPUT file to append workflow outputs.",
    )
    return parser.parse_args()


def main() -> None:
    args = _parse_args()
    scope, task_scoped = detect_scope(
        _changed_files(args.base_sha, args.head_sha), args.tasks_root
    )

    print(f"PPC_TASKS={scope}")
    if args.github_output is not None:
        _write_github_output(args.github_output, scope, task_scoped)


if __name__ == "__main__":
    main()
