#!/usr/bin/env python3

"""Normalize Sphinx gettext .po files after sphinx-intl updates."""

import argparse
import subprocess
from pathlib import Path

POT_CREATION_DATE_PREFIX = '"POT-Creation-Date: '


def get_committed_pot_creation_date(repo_root: Path, po_file: Path) -> str | None:
    relative_path = po_file.relative_to(repo_root).as_posix()
    result = subprocess.run(
        ["git", "show", f"HEAD:{relative_path}"],
        cwd=repo_root,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
    )
    if result.returncode != 0:
        return None

    for line in result.stdout.splitlines():
        if line.startswith(POT_CREATION_DATE_PREFIX):
            return line
    return None


def normalize_po_content(content: str, committed_pot_creation_date: str | None) -> str:
    lines = []
    previous_blank = False
    skip_broken_pot_date_continuation = False
    for line in content.splitlines():
        if skip_broken_pot_date_continuation and line == '"':
            skip_broken_pot_date_continuation = False
            continue

        if line.startswith("#~"):
            continue

        is_pot_creation_date = line.startswith(POT_CREATION_DATE_PREFIX)
        if is_pot_creation_date and committed_pot_creation_date is not None:
            lines.append(committed_pot_creation_date)
            previous_blank = False
            skip_broken_pot_date_continuation = not line.endswith(r'\n"')
            continue

        is_blank = line == ""
        if is_blank and previous_blank:
            continue

        lines.append(line)
        previous_blank = is_blank

    return "\n".join(lines).rstrip() + "\n"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", type=Path, required=True)
    parser.add_argument("--locale-dir", type=Path, required=True)
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()
    locale_dir = args.locale_dir.resolve()

    for po_file in sorted(locale_dir.rglob("*.po")):
        committed_pot_creation_date = get_committed_pot_creation_date(
            repo_root, po_file
        )
        content = po_file.read_text(encoding="utf-8")
        normalized = normalize_po_content(content, committed_pot_creation_date)
        if normalized != content:
            po_file.write_text(normalized, encoding="utf-8")


if __name__ == "__main__":
    main()
