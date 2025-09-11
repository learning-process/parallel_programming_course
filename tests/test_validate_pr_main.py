#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import os
import runpy
import sys
import tempfile
import unittest
from unittest import mock


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SCRIPT_PATH = os.path.join(ROOT, ".github", "scripts", "validate_pr.py")


def _import_validator():
    import importlib.util

    spec = importlib.util.spec_from_file_location("validate_pr", SCRIPT_PATH)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(mod)
    # Ensure it's importable by name for mocks if needed
    import sys as _sys

    _sys.modules.setdefault("validate_pr", mod)
    return mod


class TestHelpersAndPayload(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.v = _import_validator()

    def test_trim_helper(self):
        self.assertEqual(self.v._trim(None), "")
        self.assertEqual(self.v._trim("  a  "), "a")

    def test_load_event_payload(self):
        # Non-existing path
        self.assertIsNone(self.v._load_event_payload("/nonexistent/path.json"))
        # Invalid JSON
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            tf.write("not json")
            path_bad = tf.name
        try:
            self.assertIsNone(self.v._load_event_payload(path_bad))
        finally:
            os.unlink(path_bad)
        # Valid JSON
        data = {"pull_request": {"number": 1, "title": "x", "body": "y"}}
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            json.dump(data, tf)
            path_ok = tf.name
        try:
            loaded = self.v._load_event_payload(path_ok)
            self.assertIsInstance(loaded, dict)
            self.assertIn("pull_request", loaded)
        finally:
            os.unlink(path_ok)

    def test_split_headers_without_trailing_newline(self):
        # Cover branch where a header has no trailing newline at EOF
        body = "## 1. Full name and group"
        sections = self.v._split_sections_by_headers(body)
        self.assertIn("## 1. Full name and group", sections)


class DummyHTTPResponse:
    def __init__(self, payload: bytes):
        self._payload = payload

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc, tb):
        return False

    def read(self):
        return self._payload


class TestFetchAndMain(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.v = _import_validator()

    def test_fetch_pr_commits_mocked(self):
        commits = [
            {
                "sha": "1234567",
                "commit": {
                    "message": (
                        "feat(omp|t): m\n\n[What]\n[Why]\n[How]\nScope:\n"
                        "- Task: 1\n- Variant: 1\n- Technology: omp\n- Folder: t\n"
                        "Tests:\nLocal runs:\n"
                    )
                },
            }
        ]
        payload = json.dumps(commits).encode("utf-8")
        with mock.patch.object(
            self.v, "urlopen", return_value=DummyHTTPResponse(payload)
        ):
            res = self.v.fetch_pr_commits("o", "r", 1, "t")
            self.assertEqual(len(res), 1)

    def test_main_commits_insufficient_params(self):
        # No repo/pr and no payload
        with mock.patch.object(sys, "argv", ["prog", "--checks", "commits"]):
            # Ensure no event path
            os.environ.pop("GITHUB_EVENT_PATH", None)
            code = self.v.main()
            self.assertEqual(code, 1)

    def test_main_commits_missing_token(self):
        # Have repo/pr but no token
        with mock.patch.object(
            sys, "argv", ["prog", "--checks", "commits", "--repo", "o/r", "--pr", "1"]
        ):
            os.environ.pop("GITHUB_TOKEN", None)
            code = self.v.main()
            self.assertEqual(code, 1)

    def test_main_commits_ok(self):
        # Valid commit and token supplied
        ok_msg = (
            "feat(omp|nesterov_a_vector_sum): ok\n\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Variant: 1\n- Technology: omp\n- Folder: nesterov_a_vector_sum\n\n"
            "Tests:\nX\n\nLocal runs:\nX\n"
        )
        commits = [{"sha": "abcdef0", "commit": {"message": ok_msg}}]
        with mock.patch.object(
            sys, "argv", ["prog", "--checks", "commits", "--repo", "o/r", "--pr", "1"]
        ):
            os.environ["GITHUB_TOKEN"] = "token"
            with mock.patch.object(self.v, "fetch_pr_commits", return_value=commits):
                code = self.v.main()
                self.assertEqual(code, 0)

    def test_main_commits_error_and_internal(self):
        bad_msg = "feat(omp|task): bad no blank line\n[What]"
        commits = [{"sha": "abcdef0", "commit": {"message": bad_msg}}]
        with mock.patch.object(
            sys, "argv", ["prog", "--checks", "commits", "--repo", "o/r", "--pr", "1"]
        ):
            os.environ["GITHUB_TOKEN"] = "token"
            with mock.patch.object(self.v, "fetch_pr_commits", return_value=commits):
                code = self.v.main()
                self.assertEqual(code, 1)

    def test_main_title_and_body_missing_in_payload(self):
        # No payload at all
        with mock.patch.object(sys, "argv", ["prog", "--checks", "title", "--verbose"]):
            os.environ.pop("GITHUB_EVENT_PATH", None)
            code = self.v.main()
            self.assertEqual(code, 1)
        # Payload present but missing title/body triggers error printing branch
        data = {
            "pull_request": {
                "number": 5,
                "title": "Иванов Иван Иванович — задача",
                "body": "## 1. Full name and group\nName and group: \n",
            }
        }
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            json.dump(data, tf)
            path = tf.name
        try:
            with mock.patch.object(
                sys, "argv", ["prog", "--checks", "title", "--verbose"]
            ):
                os.environ["GITHUB_EVENT_PATH"] = path
                code = self.v.main()
                self.assertEqual(code, 1)
            with mock.patch.object(
                sys, "argv", ["prog", "--checks", "body", "--verbose"]
            ):
                os.environ["GITHUB_EVENT_PATH"] = path
                code = self.v.main()
                self.assertEqual(code, 1)
        finally:
            os.unlink(path)

        # Now raise an internal error
        with mock.patch.object(
            sys, "argv", ["prog", "--checks", "commits", "--repo", "o/r", "--pr", "1"]
        ):
            os.environ["GITHUB_TOKEN"] = "token"
            with mock.patch.object(
                self.v, "fetch_pr_commits", side_effect=RuntimeError("boom")
            ):
                code = self.v.main()
                self.assertEqual(code, 2)

    def test_main_body_none_and_ok(self):
        v = _import_validator()
        # Body None path (no payload)
        with mock.patch.object(sys, "argv", ["prog", "--checks", "body", "--verbose"]):
            os.environ.pop("GITHUB_EVENT_PATH", None)
            code = v.main()
            self.assertEqual(code, 1)
        # Body OK path
        body = (
            "## 1. Full name and group\nName and group: A\n\n"
            "## 2. Assignment / Topic / Task\nAssignment: A\n\n"
            "## 3. Technology / Platform used\nTechnology: omp\n\n"
            "## 4. Goals of the work\nGoals: A\n\n"
            "## 5. Solution description and structure\nDescription: A\n\n"
            "## 6. System requirements and build instructions\nBuild & Run: A\n\n"
            "## 7. Testing and verification\nTesting: A\n\n"
            "## 8. Results\nResults: A\n\n"
            "## 9. Performance analysis\nAnalysis: A\n\n"
            "## 10. Conclusions and possible improvements\nConclusions: A\n\n"
            "## 11. Limitations / known issues\nLimitations: A\n\n"
            "## 12. CI / static analysis / code style\nCI & Style: A\n"
        )
        data = {"pull_request": {"number": 7, "title": "x", "body": body}}
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            json.dump(data, tf)
            path = tf.name
        try:
            with mock.patch.object(
                sys, "argv", ["prog", "--checks", "body", "--verbose"]
            ):
                os.environ["GITHUB_EVENT_PATH"] = path
                code = v.main()
                self.assertEqual(code, 0)
        finally:
            os.unlink(path)

    def test_main_title_print_branch_else(self):
        v = _import_validator()
        data = {"pull_request": {"number": 8, "title": "bad", "body": ""}}
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            json.dump(data, tf)
            path = tf.name
        try:
            with mock.patch.object(sys, "argv", ["prog", "--checks", "title"]):
                # Force errs starting with '✗' to hit print(e)
                with mock.patch.object(
                    v, "validate_title", return_value=["✗ forced-error"]
                ):
                    os.environ["GITHUB_EVENT_PATH"] = path
                    code = v.main()
                    self.assertEqual(code, 1)
        finally:
            os.unlink(path)


class TestRunAsMain(unittest.TestCase):
    def test_run_module_as_main_with_verbose(self):
        # Execute the script as __main__ under coverage to hit self-tests block
        data = {
            "number": 999,
            "pull_request": {
                "number": 999,
                "title": "2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора.",
                "body": (
                    "## 1. Full name and group\nName and group: A\n\n"
                    "## 2. Assignment / Topic / Task\nAssignment: A\n\n"
                    "## 3. Technology / Platform used\nTechnology: omp\n\n"
                    "## 4. Goals of the work\nGoals: A\n\n"
                    "## 5. Solution description and structure\nDescription: A\n\n"
                    "## 6. System requirements and build instructions\nBuild & Run: A\n\n"
                    "## 7. Testing and verification\nTesting: A\n\n"
                    "## 8. Results\nResults: A\n\n"
                    "## 9. Performance analysis\nAnalysis: A\n\n"
                    "## 10. Conclusions and possible improvements\nConclusions: A\n\n"
                    "## 11. Limitations / known issues\nLimitations: A\n\n"
                    "## 12. CI / static analysis / code style\nCI & Style: A\n"
                ),
            },
        }
        with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
            json.dump(data, tf)
            path = tf.name
        try:
            os.environ["GITHUB_EVENT_PATH"] = path
            # Prepare argv for verbose title check to trigger self-tests
            argv = [SCRIPT_PATH, "--checks", "title", "--verbose"]
            with mock.patch.object(sys, "argv", argv):
                with self.assertRaises(SystemExit) as cm:
                    runpy.run_path(SCRIPT_PATH, run_name="__main__")
                self.assertEqual(cm.exception.code, 0)
        finally:
            os.unlink(path)

    def test_systemexit_branch_in_try(self):
        # Force validate_title to raise SystemExit inside try block to hit 'except SystemExit: raise'
        v = _import_validator()
        with mock.patch.object(sys, "argv", ["prog", "--checks", "title"]):
            with mock.patch.object(v, "validate_title", side_effect=SystemExit(5)):
                # Provide minimal payload with title
                data = {"pull_request": {"number": 1, "title": "bad", "body": ""}}
                with tempfile.NamedTemporaryFile("w+", delete=False) as tf:
                    json.dump(data, tf)
                    path = tf.name
                try:
                    os.environ["GITHUB_EVENT_PATH"] = path
                    with self.assertRaises(SystemExit) as cm:
                        v.main()
                    self.assertEqual(cm.exception.code, 5)
                finally:
                    os.unlink(path)


if __name__ == "__main__":
    unittest.main(verbosity=2)
