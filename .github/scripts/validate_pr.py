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
from typing import List, Optional, Tuple


DEFAULT_TITLE_TASK_REGEX = None  # No built-in defaults — must come from file
DEFAULT_TITLE_DEV_REGEX = None   # No built-in defaults — must come from file


def _trim(s: Optional[str]) -> str:
    return (s or "").strip()


def _load_title_config() -> Tuple[Optional[dict], List[str]]:
    policy_path = os.path.join(".github", "policy", "pr_title.json")
    if os.path.exists(policy_path):
        try:
            with open(policy_path, "r", encoding="utf-8") as f:
                return json.load(f), [policy_path]
        except Exception:
            # Invalid JSON — treat as error (no defaults)
            return None, [policy_path]
    return None, [policy_path]


def validate_title(title: str) -> List[str]:
    """Validate PR title. Returns a list of error messages (empty if valid)."""
    title = (title or "").strip()
    if not title:
        return [
            "Empty PR title. Use '[TASK] …' for tasks or '[DEV] …' for development.",
        ]

    # Load policy config (required)
    cfg, candidates = _load_title_config()
    if not cfg:
        return [
            "PR title policy config not found or invalid.",
            f"Expected one of: {', '.join(candidates)}",
        ]

    # Validate required keys (no built-in defaults)
    errors: List[str] = []
    task_regex = cfg.get("task_regex")
    dev_regex = cfg.get("dev_regex")
    allow_dev = cfg.get("allow_dev")
    examples = cfg.get("examples") if isinstance(cfg.get("examples"), dict) else {}

    if not isinstance(task_regex, str) or not task_regex.strip():
        errors.append("Missing or empty 'task_regex' in policy config.")
    if not isinstance(dev_regex, str) or not dev_regex.strip():
        errors.append("Missing or empty 'dev_regex' in policy config.")
    if not isinstance(allow_dev, bool):
        errors.append("Missing or non-boolean 'allow_dev' in policy config.")
    if errors:
        return errors

    # Accept development titles with a simple rule
    if allow_dev and re.match(dev_regex, title, flags=re.UNICODE | re.VERBOSE):
        return []

    # Accept strict course task titles
    if re.match(task_regex, title, flags=re.UNICODE | re.VERBOSE):
        return []

    example_task_ru = examples.get("task_ru")
    example_task_en = examples.get("task_en")
    example_dev = examples.get("dev")
    return [
        "Invalid PR title.",
        "Allowed formats (see policy config):",
        *( [f"- Task (RU): {example_task_ru}"] if example_task_ru else [] ),
        *( [f"- Task (EN): {example_task_en}"] if example_task_en else [] ),
        *( [f"- Dev: {example_dev}"] if example_dev else [] ),
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
