#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import unittest


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SCRIPT_PATH = os.path.join(ROOT, ".github", "scripts", "validate_pr.py")


def _import_validator():
    import importlib.util

    spec = importlib.util.spec_from_file_location("validate_pr", SCRIPT_PATH)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(mod)
    return mod


class TestPRTitle(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.v = _import_validator()

    def test_title_valid_ru_and_en(self):
        ok_ru = (
            "2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора."
        )
        ok_task_tag_ru = "[TASK] " + ok_ru
        ok_en = "3-7. Smith John Edward. 1234-a1. Fast matrix multiplication."
        for t in (ok_ru, ok_task_tag_ru, ok_en):
            with self.subTest(t=t):
                self.assertEqual(self.v.validate_title(t), [])

    def test_title_invalid_examples(self):
        bad = [
            "Иванов Иван Иванович — задача",  # no numbers/format
            "2-12 Иванов Иван Иванович. 2341-а234. …",  # missing dot after 2-12
            "2-12. Иванов Иван. 2341-а234. …",  # no patronymic
        ]
        for t in bad:
            with self.subTest(t=t):
                self.assertNotEqual(self.v.validate_title(t), [])

    def test_title_empty_and_task_prefix_partial(self):
        # Empty title path
        errs = self.v.validate_title("")
        self.assertNotEqual(errs, [])
        # Strip [TASK] and fail early on task/variant block to cover branch
        errs = self.v.validate_title("[TASK] Иванов Иван Иванович — задача")
        self.assertTrue(any("Invalid task/variant block" in e for e in errs))


class TestPRBody(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.v = _import_validator()

    def test_body_valid(self):
        body = (
            "## 1. Full name and group\n"
            "Name and group: Ivanov Ivan, 2341-a234\n\n"
            "## 2. Assignment / Topic / Task\n"
            "Assignment: Implement vector sum\n\n"
            "## 3. Technology / Platform used\n"
            "Technology: omp\n\n"
            "## 4. Goals of the work\n"
            "Goals: Learn parallel reduction\n\n"
            "## 5. Solution description and structure\n"
            "Description: Parallelize with OpenMP reduction\n\n"
            "## 6. System requirements and build instructions\n"
            "Build & Run: cmake ..; make; ./app\n\n"
            "## 7. Testing and verification\n"
            "Testing: Unit and perf tests\n\n"
            "## 8. Results\n"
            "Results: Speedup 3x\n\n"
            "## 9. Performance analysis\n"
            "Analysis: Scaling on 8 threads\n\n"
            "## 10. Conclusions and possible improvements\n"
            "Conclusions: Improve memory layout\n\n"
            "## 11. Limitations / known issues\n"
            "Limitations: Small N overhead\n\n"
            "## 12. CI / static analysis / code style\n"
            "CI & Style: Lint, formatting, CI passing\n"
        )
        self.assertEqual(self.v.validate_body(body), [])

    def test_body_missing_header_and_empty_label(self):
        body = (
            "## 1. Full name and group\n"
            "Name and group: \n\n"  # empty label
            # Missing header 2 entirely
            "## 3. Technology / Platform used\n"
            "Technology: omp\n\n"
            "## 4. Goals of the work\n"
            "Goals: x\n\n"
            "## 5. Solution description and structure\n"
            "Description: x\n\n"
            "## 6. System requirements and build instructions\n"
            "Build & Run: x\n\n"
            "## 7. Testing and verification\n"
            "Testing: x\n\n"
            "## 8. Results\n"
            "Results: x\n\n"
            "## 9. Performance analysis\n"
            "Analysis: x\n\n"
            "## 10. Conclusions and possible improvements\n"
            "Conclusions: x\n\n"
            "## 11. Limitations / known issues\n"
            "Limitations: x\n\n"
            "## 12. CI / static analysis / code style\n"
            "CI & Style: x\n"
        )
        errs = self.v.validate_body(body)
        self.assertTrue(any("Missing required sections:" in e for e in errs))
        self.assertTrue(any("Empty required fields" in e for e in errs))

    def test_body_with_html_comments(self):
        body = (
            "## 1. Full name and group\n"
            "Name and group: x\n\n"
            "<!-- remove me -->\n"
            "## 2. Assignment / Topic / Task\n"
            "Assignment: x\n\n"
            "## 3. Technology / Platform used\n"
            "Technology: x\n\n"
            "## 4. Goals of the work\n"
            "Goals: x\n\n"
            "## 5. Solution description and structure\n"
            "Description: x\n\n"
            "## 6. System requirements and build instructions\n"
            "Build & Run: x\n\n"
            "## 7. Testing and verification\n"
            "Testing: x\n\n"
            "## 8. Results\n"
            "Results: x\n\n"
            "## 9. Performance analysis\n"
            "Analysis: x\n\n"
            "## 10. Conclusions and possible improvements\n"
            "Conclusions: x\n\n"
            "## 11. Limitations / known issues\n"
            "Limitations: x\n\n"
            "## 12. CI / static analysis / code style\n"
            "CI & Style: x\n"
        )
        errs = self.v.validate_body(body)
        self.assertTrue(any("Found HTML comments" in e for e in errs))

    def test_body_empty(self):
        errs = self.v.validate_body("")
        self.assertTrue(any("PR body is empty" in e for e in errs))

    def test_body_label_without_any_value(self):
        body = (
            "## 1. Full name and group\n"
            "Name and group:\n\n"  # no spaces after colon
            "## 2. Assignment / Topic / Task\n"
            "Assignment: A\n\n"
            "## 3. Technology / Platform used\n"
            "Technology: A\n\n"
            "## 4. Goals of the work\n"
            "Goals: A\n\n"
            "## 5. Solution description and structure\n"
            "Description: A\n\n"
            "## 6. System requirements and build instructions\n"
            "Build & Run: A\n\n"
            "## 7. Testing and verification\n"
            "Testing: A\n\n"
            "## 8. Results\n"
            "Results: A\n\n"
            "## 9. Performance analysis\n"
            "Analysis: A\n\n"
            "## 10. Conclusions and possible improvements\n"
            "Conclusions: A\n\n"
            "## 11. Limitations / known issues\n"
            "Limitations: A\n\n"
            "## 12. CI / static analysis / code style\n"
            "CI & Style: A\n"
        )
        errs = self.v.validate_body(body)
        self.assertTrue(any("Empty required fields" in e for e in errs))


class TestCommitMessages(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.v = _import_validator()

    def test_commit_valid(self):
        msg = (
            "feat(omp|nesterov_a_vector_sum): implement parallel vector sum\n\n"
            "[What]\nAdd OMP reduction for vector sum.\n\n"
            "[Why]\nImprove performance.\n\n"
            "[How]\nUse #pragma omp parallel for reduction(+:sum).\n\n"
            "Scope:\n- Task: 2\n- Variant: 12\n- Technology: omp\n- Folder: nesterov_a_vector_sum\n\n"
            "Tests:\nAdded unit and perf tests.\n\n"
            "Local runs:\nmake test\n"
        )
        self.assertEqual(self.v.validate_commit_message(msg), [])

    def test_commit_invalid_cases(self):
        # wrong type
        msg1 = (
            "feature(omp|nesterov_a_vector_sum): summary\n\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Variant: 1\n- Technology: omp\n- Folder: f\n"
            "Tests:\nLocal runs:\n"
        )
        # disallowed technology
        msg2 = (
            "feat(cuda|nesterov_a_vector_sum): add cuda impl\n\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Variant: 1\n- Technology: cuda\n- Folder: f\n"
            "Tests:\nLocal runs:\n"
        )
        # subject too long
        long_summary = "x" * 73
        msg3 = (
            f"feat(omp|nesterov_a_vector_sum): {long_summary}\n\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Variant: 1\n- Technology: omp\n- Folder: f\n"
            "Tests:\nLocal runs:\n"
        )
        # no blank line
        msg4 = (
            "feat(omp|nesterov_a_vector_sum): ok\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Variant: 1\n- Technology: omp\n- Folder: f\n"
            "Tests:\nLocal runs:\n"
        )
        # missing tokens
        msg5 = "feat(omp|nesterov_a_vector_sum): ok\n\nNo sections here\n"
        # missing fields in scope
        msg6 = (
            "feat(omp|nesterov_a_vector_sum): ok\n\n[What]\n[Why]\n[How]\n"
            "Scope:\n- Task: 1\n- Technology: omp\n- Folder: f\n\n"
            "Tests:\nLocal runs:\n"
        )

        for i, m in enumerate([msg1, msg2, msg3, msg4, msg5, msg6], start=1):
            with self.subTest(i=i):
                errs = self.v.validate_commit_message(m)
                self.assertNotEqual(errs, [])

    def test_title_partial_group_and_taskname_errors_and_fallback(self):
        # Arbitrary group should be accepted
        t1 = "2-12. Иванов Иван Иванович. ХХХ. Что-то"
        self.assertEqual(self.v.validate_title(t1), [])
        # Trailing newline after group triggers fallback (missing taskname)
        t2 = "2-12. Иванов Иван Иванович. 2341-а234. \n"
        errs = self.v.validate_title(t2)
        self.assertNotEqual(errs, [])
        # Fallback case: embed newline so full regex fails while partial checks pass
        t3 = "2-12. Иванов Иван Иванович. 2341-а234. Задача\nещё"
        errs = self.v.validate_title(t3)
        self.assertTrue(any("does not match the required pattern" in e for e in errs))


if __name__ == "__main__":
    unittest.main(verbosity=2)
