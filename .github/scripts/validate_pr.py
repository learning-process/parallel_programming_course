#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Minimal PR title validator for CI gate.

Rules:
- Accept either a strict task title with required prefix '[TASK]'
  Pattern: [TASK] <Task>-<Variant>. <Last> <First> <Middle>. <Group>. <Task name>
- Or a development title with prefix '[DEV]' followed by any non-empty text
  Pattern: [DEV] <any text>
"""

from __future__ import annotations

import json
import os
import re
import sys
from typing import List, Optional


TITLE_TASK_REGEX = r"""
^\[TASK\]\s+
(?P<task>\d+)-(?P<variant>\d+)\.\s+
(?P<lastname>[А-ЯA-ZЁ][а-яa-zё]+)\s+
(?P<firstname>[А-ЯA-ZЁ][а-яa-zё]+)\s+
(?P<middlename>[А-ЯA-ZЁ][а-яa-zё]+)\.\s+
(?P<group>.+?)\.\s+
(?P<taskname>\S.*)
$
"""

TITLE_DEV_REGEX = r"^\[DEV\]\s+\S.*$"


def _trim(s: Optional[str]) -> str:
    return (s or "").strip()


def validate_title(title: str) -> List[str]:
    """Validate PR title. Returns a list of error messages (empty if valid)."""
    title = (title or "").strip()
    if not title:
        return [
            "Empty PR title. Use '[TASK] …' for tasks or '[DEV] …' for development.",
        ]

    # Accept development titles with a simple rule
    if re.match(TITLE_DEV_REGEX, title, flags=re.UNICODE):
        return []

    # Accept strict course task titles
    if re.match(TITLE_TASK_REGEX, title, flags=re.UNICODE | re.VERBOSE):
        return []

    example_task_ru = "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора."
    example_task_en = "[TASK] 2-12. Ivanov Ivan Ivanovich. 2341-a234. Vector elements sum calculation."
    example_dev = "[DEV] Update docs for lab 2"
    return [
        "Invalid PR title.",
        "Allowed formats:",
        f"- Task: {example_task_ru}",
        f"- Task: {example_task_en}",
        f"- Dev:  {example_dev}",
    ]


def _load_event_payload(path: Optional[str]) -> Optional[dict]:
    if not path or not os.path.exists(path):
        return None
    with open(path, "r", encoding="utf-8") as f:
        try:
            return json.load(f)
        except Exception:
            return None


def main() -> int:
    try:
        payload = _load_event_payload(os.environ.get("GITHUB_EVENT_PATH"))

        pr_title = None
        if payload and payload.get("pull_request"):
            pr = payload["pull_request"]
            pr_title = pr.get("title")

        if pr_title is None:
            # Not a PR context – do not fail the gate
            print("No PR title in event payload; skipping title check (non-PR event).")
            return 0

        errs = validate_title(pr_title)
        if errs:
            for e in errs:
                print(f"✗ {e}")
            return 1

        print("OK: PR title is valid.")
        return 0

    except SystemExit:
        raise
    except Exception as e:
        print(f"Internal error occurred: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
