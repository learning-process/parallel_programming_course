import argparse
import fnmatch
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

SOURCE_EXTENSIONS = {
    ".c",
    ".cpp",
    ".h",
    ".hpp",
}


@dataclass(frozen=True)
class ApiPattern:
    category: str
    regex: re.Pattern
    strip_literals: bool


@dataclass(frozen=True)
class Violation:
    path: Path
    line_number: int
    category: str
    matched_api: str
    backend: str


def join_regex_patterns(patterns: tuple[str, ...]) -> str:
    return "|".join(patterns)


def compile_include_regex(headers: tuple[str, ...]) -> re.Pattern:
    escaped_headers = tuple(re.escape(header) for header in headers)
    return re.compile(
        r"^\s*#\s*include\s*[<\"](?:" + join_regex_patterns(escaped_headers) + r")[>\"]"
    )


CPP_THREAD_HEADERS = (
    "thread",
    "future",
    "mutex",
    "shared_mutex",
    "condition_variable",
    "execution",
    "barrier",
    "latch",
    "semaphore",
    "stop_token",
)

CPP_THREAD_STD_SYMBOL_PATTERNS = (
    r"j?thread",
    "this_thread",
    "async",
    "future",
    "shared_future",
    "promise",
    "packaged_task",
    "mutex",
    "recursive_mutex",
    "timed_mutex",
    "recursive_timed_mutex",
    "shared_mutex",
    "shared_timed_mutex",
    "unique_lock",
    "shared_lock",
    "lock_guard",
    "scoped_lock",
    r"condition_variable(?:_any)?",
    r"memory_order(?:_[a-z_]+)?",
    "barrier",
    "latch",
    "counting_semaphore",
    "binary_semaphore",
    "stop_source",
    "stop_token",
    "stop_callback",
    "execution",
)


API_PATTERNS = {
    "openmp": [
        ApiPattern("OpenMP", re.compile(r"^\s*#\s*include\s*[<\"]omp\.h[>\"]"), False),
        ApiPattern("OpenMP", re.compile(r"^\s*#\s*pragma\s+omp\b"), False),
        ApiPattern("OpenMP", re.compile(r"\bomp_[A-Za-z0-9_]*\b"), True),
        ApiPattern("OpenMP", re.compile(r"\b_OPENMP\b"), True),
    ],
    "tbb": [
        ApiPattern(
            "TBB",
            re.compile(r"^\s*#\s*include\s*[<\"](?:oneapi/)?tbb/[^>\"]+[>\"]"),
            False,
        ),
        ApiPattern("TBB", re.compile(r"\boneapi::tbb::"), True),
        ApiPattern("TBB", re.compile(r"\btbb::"), True),
        ApiPattern("TBB", re.compile(r"\bTBB_[A-Za-z0-9_]*\b"), True),
    ],
    "mpi": [
        ApiPattern("MPI", re.compile(r"^\s*#\s*include\s*[<\"]mpi\.h[>\"]"), False),
        ApiPattern("MPI", re.compile(r"\bMPI_[A-Za-z0-9_]*\b"), True),
        ApiPattern("MPI", re.compile(r"\bMPI::"), True),
    ],
    "osh": [
        ApiPattern(
            "OSH",
            re.compile(r"^\s*#\s*include\s*[<\"]shmem\.h[>\"]"),
            False,
        ),
        ApiPattern("OSH", re.compile(r"\bshmem_[A-Za-z0-9_]*\b"), True),
        ApiPattern("OSH", re.compile(r"\bSHMEM_[A-Za-z0-9_]*\b"), True),
    ],
    "cpp_thread": [
        ApiPattern(
            "C++ thread API",
            compile_include_regex(CPP_THREAD_HEADERS),
            False,
        ),
        ApiPattern(
            "C++ thread API",
            re.compile(
                r"\bstd::(?:"
                + join_regex_patterns(CPP_THREAD_STD_SYMBOL_PATTERNS)
                + r")\b"
            ),
            True,
        ),
        ApiPattern(
            "POSIX thread API",
            re.compile(r"^\s*#\s*include\s*[<\"]pthread\.h[>\"]"),
            False,
        ),
        ApiPattern("POSIX thread API", re.compile(r"\bpthread_[A-Za-z0-9_]*\b"), True),
    ],
}


API_ORDER = ("openmp", "tbb", "mpi", "osh", "cpp_thread")


BACKEND_RULES = {
    "seq": {"openmp", "tbb", "mpi", "osh", "cpp_thread"},
    "omp": {"tbb", "mpi", "osh", "cpp_thread"},
    "tbb": {"openmp", "mpi", "osh", "cpp_thread"},
    "mpi": {"openmp", "tbb", "osh", "cpp_thread"},
    "osh": {"openmp", "tbb", "mpi", "cpp_thread"},
    "stl": {"openmp", "tbb", "mpi", "osh"},
    "common": set(API_ORDER),
}


def strip_comments(text: str) -> str:
    result = []
    index = 0
    state = "normal"

    while index < len(text):
        current = text[index]
        next_char = text[index + 1] if index + 1 < len(text) else ""

        if state == "normal":
            if current == "R" and next_char == '"':
                raw_end = consume_raw_string(text, index)
                if raw_end is not None:
                    result.extend(text[index:raw_end])
                    index = raw_end
                    continue
            if current in {"'", '"'}:
                quote = current
                result.append(current)
                index += 1
                escaped = False
                while index < len(text):
                    char = text[index]
                    result.append(char)
                    index += 1
                    if char == "\n":
                        break
                    if escaped:
                        escaped = False
                        continue
                    if char == "\\":
                        escaped = True
                        continue
                    if char == quote:
                        break
                continue
            if current == "/" and next_char == "/":
                result.extend("  ")
                index += 2
                state = "line_comment"
                continue
            if current == "/" and next_char == "*":
                result.extend("  ")
                index += 2
                state = "block_comment"
                continue
            result.append(current)
            index += 1
            continue

        if state == "line_comment":
            if current == "\n":
                result.append(current)
                state = "normal"
            else:
                result.append(" ")
            index += 1
            continue

        if current == "\n":
            result.append(current)
            index += 1
            continue
        if current == "*" and next_char == "/":
            result.extend("  ")
            index += 2
            state = "normal"
            continue
        result.append(" ")
        index += 1

    return "".join(result)


def consume_raw_string(text: str, start: int) -> int | None:
    delimiter_start = start + 2
    delimiter_end = delimiter_start

    while delimiter_end < len(text) and text[delimiter_end] != "(":
        if text[delimiter_end] in "\\ \t\r\n":
            return None
        delimiter_end += 1

    if delimiter_end >= len(text):
        return None

    delimiter = text[delimiter_start:delimiter_end]
    closing = f'){delimiter}"'
    closing_start = text.find(closing, delimiter_end + 1)
    if closing_start == -1:
        return None
    return closing_start + len(closing)


def strip_literals(text: str) -> str:
    result = []
    index = 0

    while index < len(text):
        current = text[index]
        next_char = text[index + 1] if index + 1 < len(text) else ""

        if current == "R" and next_char == '"':
            raw_end = consume_raw_string(text, index)
            if raw_end is not None:
                result.extend(
                    "\n" if char == "\n" else " " for char in text[index:raw_end]
                )
                index = raw_end
                continue

        if current not in {"'", '"'}:
            result.append(current)
            index += 1
            continue

        quote = current
        result.append(" ")
        index += 1
        escaped = False
        while index < len(text):
            char = text[index]
            if char == "\n":
                result.append(char)
                index += 1
                break
            result.append(" ")
            index += 1
            if escaped:
                escaped = False
                continue
            if char == "\\":
                escaped = True
                continue
            if char == quote:
                break

    return "".join(result)


def get_backend(path: Path) -> str | None:
    parts = path.parts
    try:
        tasks_index = parts.index("tasks") + 1
    except ValueError:
        return None

    if tasks_index >= len(parts) or parts[tasks_index] == "common":
        return None

    for backend_index, backend in enumerate(parts):
        if backend_index <= tasks_index:
            continue
        if backend in BACKEND_RULES:
            return backend
    return None


def is_source_file(path: Path) -> bool:
    return path.suffix in SOURCE_EXTENSIONS


def read_changed_files(base: str, head: str) -> list[Path]:
    command = [
        "git",
        "diff",
        "--name-only",
        "--diff-filter=ACMRT",
        f"{base}...{head}",
        "--",
        "tasks/",
    ]
    result = subprocess.run(command, check=True, stdout=subprocess.PIPE, text=True)
    return [Path(line) for line in result.stdout.splitlines() if line]


def read_all_task_files() -> list[Path]:
    return [path for path in Path("tasks").rglob("*") if path.is_file()]


def is_mpi_openmp_allowed(path: Path, allow_mpi_openmp: list[str]) -> bool:
    path_name = path.as_posix()
    return any(fnmatch.fnmatch(path_name, pattern) for pattern in allow_mpi_openmp)


def find_violations(path: Path, allow_mpi_openmp: list[str]) -> list[Violation]:
    backend = get_backend(path)
    if backend is None or not is_source_file(path) or not path.exists():
        return []

    forbidden_apis = set(BACKEND_RULES[backend])
    if backend == "mpi" and is_mpi_openmp_allowed(path, allow_mpi_openmp):
        forbidden_apis.discard("openmp")

    if not forbidden_apis:
        return []

    text = path.read_text(encoding="utf-8")
    without_comments = strip_comments(text)
    without_literals = strip_literals(without_comments)
    lines = {
        False: without_comments.splitlines(),
        True: without_literals.splitlines(),
    }

    violations = []
    seen = set()
    for api in API_ORDER:
        if api not in forbidden_apis:
            continue
        for pattern in API_PATTERNS[api]:
            for line_number, line in enumerate(lines[pattern.strip_literals], start=1):
                match = pattern.regex.search(line)
                if match is None:
                    continue
                key = (path, line_number, pattern.category)
                if key in seen:
                    continue
                seen.add(key)
                violations.append(
                    Violation(
                        path,
                        line_number,
                        pattern.category,
                        match.group(0).strip(),
                        backend,
                    )
                )

    return violations


def github_escape(value: str) -> str:
    return value.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")


def print_violation(violation: Violation) -> None:
    scope = (
        "tasks/<task>/common shared files"
        if violation.backend == "common"
        else f"tasks/<task>/{violation.backend} implementation files"
    )
    message = (
        f"{violation.category} usage ({violation.matched_api}) is forbidden in {scope}."
    )
    print(
        f"::error file={github_escape(str(violation.path))},line={violation.line_number},"
        f"title=Forbidden backend API::{github_escape(message)}"
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Check task backend source files for forbidden parallel API usage."
    )
    source_group = parser.add_mutually_exclusive_group(required=True)
    source_group.add_argument(
        "--all", action="store_true", help="check all task source files"
    )
    source_group.add_argument("--base", help="base commit for changed-file mode")
    parser.add_argument("--head", help="head commit for changed-file mode")
    parser.add_argument(
        "--allow-mpi-openmp",
        action="append",
        default=[],
        metavar="GLOB",
        help="allow OpenMP in matching tasks/*/mpi source files",
    )
    args = parser.parse_args()

    if args.base and not args.head:
        parser.error("--head is required when --base is used")
    if args.head and not args.base:
        parser.error("--base is required when --head is used")

    paths = (
        read_all_task_files() if args.all else read_changed_files(args.base, args.head)
    )
    checked_paths = [
        path
        for path in paths
        if get_backend(path) is not None and is_source_file(path) and path.exists()
    ]

    if not checked_paths:
        print("No changed backend source files to check.")
        return 0

    violations = []
    for path in checked_paths:
        violations.extend(find_violations(path, args.allow_mpi_openmp))

    for violation in violations:
        print_violation(violation)

    if violations:
        print(
            f"Found {len(violations)} forbidden backend API usage issue(s).",
            file=sys.stderr,
        )
        return 1

    print(f"Task backend API check passed for {len(checked_paths)} file(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
