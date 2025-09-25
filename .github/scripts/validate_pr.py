#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
PR/commit compliance validator for parallel_programming_course.

Runs locally and in GitHub Actions. Uses only stdlib.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sys
from typing import List, Dict, Tuple, Optional
from urllib.request import Request, urlopen
from urllib.parse import quote


# --- Title validation regex (strict) ---
TITLE_REGEX = r"""
^(?:\[TASK\]\s*)?
(?P<task>\d+)-(?P<variant>\d+)\.\s+
(?P<lastname>[А-ЯA-ZЁ][а-яa-zё]+)\s+
(?P<firstname>[А-ЯA-ZЁ][а-яa-zё]+)\s+
(?P<middlename>[А-ЯA-ZЁ][а-яa-zё]+)\.\s+
(?P<group>.+?)\.\s+
(?P<taskname>\S.*)
$
"""


SUBJECT_REGEX = r"^(feat|fix|perf|test|refactor|docs|build|chore)\(([a-z]+)\|([a-z0-9_]+)\): [A-Za-z0-9].*$"
ALLOWED_TECH = {"seq", "omp", "mpi", "stl", "tbb", "all"}


def print_section(title: str) -> None:
    line = "=" * 8
    print(f"\n{line} {title} {line}")


def _trim(s: Optional[str]) -> str:
    return (s or "").strip()


def validate_title(title: str) -> List[str]:
    """Validate PR title against strict course rules.

    Returns a list of error messages (empty if valid).
    """
    errors: List[str] = []
    title = title.strip()

    example_ru = (
        "2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора."
    )
    example_en = (
        "2-12. Ivanov Ivan Ivanovich. 2341-a234. Vector elements sum calculation."
    )

    if not title:
        return [
            "PR title is empty. Expected format is strictly defined.\n"
            f"Example (RU): {example_ru}\n"
            f"Example (EN): {example_en}"
        ]

    # Full strict match (record but do not early-return to allow granular checks)
    full_match = re.match(TITLE_REGEX, title, flags=re.UNICODE | re.VERBOSE) is not None

    # If not matched – try to pinpoint the failing segment.
    # 1) Optional prefix is allowed; strip it for partial checks
    work = title
    if work.startswith("[TASK]"):
        task_prefix_len = len("[TASK]")
        work = work[task_prefix_len:].lstrip()

    # 2) Task/variant with dot
    m = re.match(r"^(\d+)-(\d+)\.\s+", work)
    if not m:
        errors.append(
            "Invalid task/variant block — expected '<Task>-<Variant>. ' (a dot and a space after the digits).\n"
            f"Example (RU): {example_ru}\n"
            f"Example (EN): {example_en}"
        )
        return errors

    pos = m.end()
    rest = work[pos:]

    # 3) Full name with dot after patronymic
    m = re.match(
        r"^([А-ЯA-ZЁ][а-яa-zё]+)\s+([А-ЯA-ZЁ][а-яa-zё]+)\s+([А-ЯA-ZЁ][а-яa-zё]+)\.\s+",
        rest,
        flags=re.UNICODE,
    )
    if not m:
        errors.append(
            "Invalid full name block — expected 'LastName FirstName MiddleName.' (a dot after patronymic).\n"
            f"Example (RU): {example_ru}\n"
            f"Example (EN): {example_en}"
        )
        return errors

    pos = m.end()
    rest = rest[pos:]

    # 4) Group with dot
    m = re.match(r"^(.+?)\.\s+", rest, flags=re.UNICODE)
    if not m:
        errors.append(
            "Invalid group block — expected '<Group>. ' (a dot and a space after group).\n"
            f"Example (RU): {example_ru}\n"
            f"Example (EN): {example_en}"
        )
        return errors

    pos = m.end()
    rest = rest[pos:]

    # 5) Task name validity is enforced by the full regex (non-whitespace start)

    # If no specific segment errors and full regex matched, accept
    if full_match:
        return errors
    # Otherwise, fallback message
    errors.append(
        "PR title does not match the required pattern.\n"
        f"Current title: '{title}'\n"
        f"Example (RU): {example_ru}\n"
        f"Example (EN): {example_en}"
    )
    return errors


def _split_sections_by_headers(body: str) -> Dict[str, Tuple[int, int]]:
    """Return mapping from exact header to (start_index, end_index) slice in body.

    We look for exact level-2 headers beginning with '## ' and use their textual content
    to delimit sections.
    """
    headers_regex = re.compile(r"^##\s+\d+\..*$", flags=re.MULTILINE)
    matches = list(headers_regex.finditer(body))
    sections: Dict[str, Tuple[int, int]] = {}
    for i, m in enumerate(matches):
        start = m.start()
        end = matches[i + 1].start() if i + 1 < len(matches) else len(body)
        next_newline = body.find("\n", m.start())
        if next_newline == -1:
            next_newline = end
        header_line = body[m.start() : next_newline]
        sections[header_line.strip()] = (start, end)
    return sections


BodyReq = Tuple[str, str]  # (header, label)


REQUIRED_BODY: List[BodyReq] = [
    ("## 1. Full name and group", "Name and group:"),
    ("## 2. Assignment / Topic / Task", "Assignment:"),
    ("## 3. Technology / Platform used", "Technology:"),
    ("## 4. Goals of the work", "Goals:"),
    ("## 5. Solution description and structure", "Description:"),
    ("## 6. System requirements and build instructions", "Build & Run:"),
    ("## 7. Testing and verification", "Testing:"),
    ("## 8. Results", "Results:"),
    ("## 9. Performance analysis", "Analysis:"),
    ("## 10. Conclusions and possible improvements", "Conclusions:"),
    ("## 11. Limitations / known issues", "Limitations:"),
    ("## 12. CI / static analysis / code style", "CI & Style:"),
]


def _label_value_in_section(section_text: str, label: str) -> Optional[str]:
    # Look for a line that starts with the label and has any non-whitespace after colon
    pattern = re.compile(rf"^{re.escape(label)}\s*(.+)$", flags=re.MULTILINE)
    m = pattern.search(section_text)
    if not m:
        return None
    value = m.group(1).strip()
    return value if value else None


def validate_body(body: str) -> List[str]:
    errors: List[str] = []
    if not body or not body.strip():
        errors.append("PR body is empty. Fill all 12 required sections.")
        return errors

    if "<!--" in body:
        errors.append(
            "Found HTML comments '<!-- ... -->'. Remove all guidance comments."
        )

    sections_map = _split_sections_by_headers(body)

    missing_headers: List[str] = []
    empty_labels: List[str] = []

    for header, label in REQUIRED_BODY:
        if header not in sections_map:
            missing_headers.append(header)
            continue
        start, end = sections_map[header]
        section_text = body[start:end]
        value = _label_value_in_section(section_text, label)
        if not value:
            empty_labels.append(f"{header} → '{label}'")

    if missing_headers:
        errors.append("Missing required sections:")
        for h in missing_headers:
            errors.append(f"✗ {h}")

    if empty_labels:
        errors.append("Empty required fields (add text after the colon):")
        for label_entry in empty_labels:
            errors.append(f"✗ {label_entry}")

    return errors


def fetch_pr_commits(owner: str, repo: str, pr_number: int, token: str) -> List[Dict]:
    url = f"https://api.github.com/repos/{quote(owner)}/{quote(repo)}/pulls/{pr_number}/commits"
    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {token}",
        "X-GitHub-Api-Version": "2022-11-28",
        "User-Agent": "parallel-programming-course-pr-validator",
    }
    req = Request(url, headers=headers, method="GET")
    with urlopen(req) as resp:
        data = json.loads(resp.read().decode("utf-8"))
    return data  # list of commits


def _extract_scope_block(body: str) -> Optional[str]:
    # Find 'Scope:' line and return until next blank line or end
    m = re.search(r"^Scope:\s*$", body, flags=re.MULTILINE)
    if not m:
        return None
    start = m.end()
    # From next line onward
    rest = body[start:]
    # Stop at first double newline
    split = re.split(r"\n\s*\n", rest, maxsplit=1)
    return split[0]


def validate_commit_message(message: str) -> List[str]:
    errors: List[str] = []
    lines = message.splitlines()
    subject = lines[0] if lines else ""

    # Subject line length
    if len(subject) > 72:
        errors.append("Subject exceeds 72 characters.")

    # Subject regex
    m = re.match(SUBJECT_REGEX, subject)
    if not m:
        errors.append(
            "Invalid subject. Expected '<type>(<technology>|<task_folder>): <summary>'.\n"
            "Example (EN): feat(omp|nesterov_a_vector_sum): implement parallel vector sum\n"
            "Example (RU): feat(omp|nesterov_a_vector_sum): implement параллельную сумму вектора"
        )
    else:
        tech = m.group(2)
        if tech not in ALLOWED_TECH:
            errors.append(
                f"Disallowed technology '{tech}'. Allowed: {', '.join(sorted(ALLOWED_TECH))}."
            )

    # Require blank line after subject and non-empty body
    if len(lines) < 2 or lines[1].strip() != "":
        errors.append("A blank line must follow the subject.")

    body = "\n".join(lines[2:]) if len(lines) >= 2 else ""

    # Body tokens at start of line
    required_tokens = [
        r"^\[What\]",
        r"^\[Why\]",
        r"^\[How\]",
        r"^Scope:",
        r"^Tests:",
        r"^Local runs:",
    ]
    for tok in required_tokens:
        if not re.search(tok, body, flags=re.MULTILINE):
            errors.append(f"Missing required body section: '{tok.strip('^')}'.")

    # Scope block must include Task, Variant, Technology, Folder
    scope_block = _extract_scope_block(body)
    if scope_block is None:
        # already flagged by required token; keep specific scope details only if present
        pass
    else:
        required_scope = ["Task", "Variant", "Technology", "Folder"]
        for key in required_scope:
            if not re.search(
                rf"^\s*[-*]?\s*{key}\s*:\s*.+$", scope_block, flags=re.MULTILINE
            ):
                errors.append(f"In 'Scope:' section missing or empty field '{key}:'.")

    return errors


def _load_event_payload(path: Optional[str]) -> Optional[dict]:
    if not path or not os.path.exists(path):
        return None
    with open(path, "r", encoding="utf-8") as f:
        try:
            return json.load(f)
        except Exception:
            return None


def main() -> int:
    parser = argparse.ArgumentParser(description="PR/commit compliance validator")
    parser.add_argument(
        "--repo",
        type=str,
        default=os.environ.get("GITHUB_REPOSITORY"),
        help="owner/repo",
    )
    parser.add_argument("--pr", type=int, default=None, help="PR number")
    parser.add_argument(
        "--checks", type=str, choices=["title", "body", "commits", "all"], default="all"
    )
    parser.add_argument("--fail-on-warn", action="store_true")
    parser.add_argument("--verbose", action="store_true")

    args = parser.parse_args()

    payload = _load_event_payload(os.environ.get("GITHUB_EVENT_PATH"))

    owner: Optional[str] = None
    repo: Optional[str] = None
    pr_number: Optional[int] = args.pr

    if args.repo and "/" in args.repo:
        owner, repo = args.repo.split("/", 1)

    if payload and not pr_number:
        pr_number = payload.get("number") or (
            payload.get("pull_request", {}).get("number")
            if payload.get("pull_request")
            else None
        )

    # Collect title/body from payload when available
    pr_title = None
    pr_body = None
    if payload and payload.get("pull_request"):
        pr = payload["pull_request"]
        pr_title = pr.get("title")
        pr_body = pr.get("body") or ""

    if args.verbose:
        print_section("CONFIG")
        print(f"repo: {args.repo}")
        print(f"owner: {owner}, repo: {repo}, pr: {pr_number}")
        print(f"event payload: {'loaded' if payload else 'none'}")

    total_errors: List[str] = []

    try:
        if args.checks in ("title", "all"):
            print_section("PR TITLE")
            if pr_title is None:
                print(
                    "Could not get PR title from event payload. Ensure a pull_request context or supply it manually."
                )
                total_errors.append("No title data")
            else:
                errs = validate_title(pr_title)
                if errs:
                    for e in errs:
                        if not e.startswith("✗"):
                            print(f"✗ {e}")
                        else:
                            print(e)
                        total_errors.append(e)
                else:
                    print("OK: PR title is valid.")

        if args.checks in ("body", "all"):
            print_section("PR BODY")
            if pr_body is None:
                print(
                    "Could not get PR body from event payload. Ensure a pull_request context or supply it manually."
                )
                total_errors.append("No body data")
            else:
                errs = validate_body(pr_body)
                if errs:
                    for e in errs:
                        if not e.startswith("✗"):
                            print(f"✗ {e}")
                        else:
                            print(e)
                        total_errors.append(e)
                else:
                    print("OK: PR body is valid.")

        if args.checks in ("commits", "all"):
            print_section("COMMITS")
            if not (owner and repo and pr_number):
                print(
                    "Commit validation requires --repo owner/repo and --pr <number> or a GitHub event payload."
                )
                total_errors.append("Insufficient params for commits fetch")
            else:
                token = os.environ.get("GITHUB_TOKEN")
                if not token:
                    print("GITHUB_TOKEN not found in environment for API access.")
                    total_errors.append("No GITHUB_TOKEN")
                else:
                    commits = fetch_pr_commits(owner, repo, int(pr_number), token)
                    commit_errors = 0
                    for c in commits:
                        sha = c.get("sha", "")[:7]
                        message = c.get("commit", {}).get("message", "")
                        first = message.splitlines()[0] if message else ""
                        errs = validate_commit_message(message)
                        header = f"{sha} — {first}"
                        if errs:
                            commit_errors += 1
                            print(f"Commit {header}")
                            for e in errs:
                                print(f"✗ {e}")
                            total_errors.extend(errs)
                        else:
                            print(f"OK: {header}")

        print_section("SUMMARY")
        if total_errors:
            print(f"Found {len(total_errors)} error(s)")
            return 1
        else:
            print("All checks passed")
            return 0

    except SystemExit:
        raise
    except Exception as e:
        print_section("INTERNAL ERROR")
        print(f"Internal error occurred: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    # Lightweight self-checks printed only with --verbose when run without payload
    exit_code = main()
    # Do simple doctest-like probes when verbose flag present (cannot easily detect here),
    # so rely on environment toggle to keep runtime minimal in CI.
    if "--verbose" in sys.argv:
        print_section("SELF-TESTS")
        # Title positives
        good_titles = [
            "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора.",
            "2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора.",
        ]
        bad_titles = [
            "Иванов Иван Иванович — задача",
            "2-12 Иванов Иван Иванович. 2341-а234. …",
            "2-12. Иванов Иван. 2341-а234. …",
        ]
        for t in good_titles:
            assert not validate_title(t), f"Expected valid title: {t}"
        for t in bad_titles:
            assert validate_title(t), f"Expected invalid title: {t}"

        # Commit subjects
        assert re.match(
            SUBJECT_REGEX,
            "feat(omp|nesterov_a_vector_sum): implement parallel vector sum",
        )
        assert not re.match(SUBJECT_REGEX, "feature(omp|x): bad type")
        # Technology validation is performed outside the regex
        errs = validate_commit_message(
            (
                "feat(cuda|nesterov_a_vector_sum): add cuda impl"
                "\n\n[What]\n[Why]\n[How]\nScope:\n"
                "- Task: 1\n- Variant: 2\n- Technology: cuda\n- Folder: nesterov_a_vector_sum\n"
                "Tests:\nLocal runs:\n"
            )
        )
        assert any("Disallowed technology" in e for e in errs)
        too_long = "feat(omp|nesterov_a_vector_sum): " + "x" * 73
        errs = validate_commit_message(
            (
                too_long + "\n\n[What]\n[Why]\n[How]\nScope:\n"
                "- Task: 1\n- Variant: 2\n- Technology: omp\n- Folder: nesterov_a_vector_sum\n"
                "Tests:\nLocal runs:\n"
            )
        )
        assert any("exceeds 72" in e for e in errs)
        print("Self-tests passed")

    sys.exit(exit_code)
