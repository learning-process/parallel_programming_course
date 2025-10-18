import json
from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path
from typing import Tuple
from unittest import TestCase, mock


REPO_ROOT = Path.cwd()
VALIDATOR_PATH = REPO_ROOT / ".github/scripts/validate_pr.py"
POLICY_PATH = REPO_ROOT / ".github/policy/pr_title.json"


def load_validator():
    spec = spec_from_file_location("validate_pr", str(VALIDATOR_PATH))
    assert spec and spec.loader
    mod = module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[attr-defined]
    return mod


class TestValidateTitle(TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        assert VALIDATOR_PATH.exists(), "Validator script not found"
        assert POLICY_PATH.exists(), "Policy file not found"
        cls.validator = load_validator()
        with open(POLICY_PATH, "r", encoding="utf-8") as f:
            cls.policy = json.load(f)

    def test_valid_task_ru(self) -> None:
        title = (
            "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. OMP. Вычисление суммы элементов вектора."
        )
        errs = self.validator.validate_title(title)
        self.assertEqual(errs, [])

    def test_valid_task_en(self) -> None:
        title = (
            "[TASK] 3-4. Ivanov Ivan Ivanovich. 2341-a234. MPI. Vector elements sum calculation."
        )
        errs = self.validator.validate_title(title)
        self.assertEqual(errs, [])

    def test_invalid_task_number_out_of_range(self) -> None:
        title = (
            "[TASK] 6-1. Иванов Иван Иванович. 2341-а234. SEQ. Вычисление суммы элементов вектора."
        )
        errs = self.validator.validate_title(title)
        self.assertTrue(errs, "Expected errors for out-of-range task number")

    def test_valid_dev_when_allowed(self) -> None:
        title = "[DEV] Update docs for lab 2"
        errs = self.validator.validate_title(title)
        self.assertEqual(errs, [])

    def test_dev_disallowed_by_policy(self) -> None:
        cfg = dict(self.policy)
        cfg["allow_dev"] = False

        def fake_load_title_config() -> Tuple[dict, list]:
            return cfg, [str(POLICY_PATH)]

        with mock.patch.object(self.validator, "_load_title_config", fake_load_title_config):
            errs = self.validator.validate_title("[DEV] Working WIP")
            self.assertTrue(errs, "Expected errors when allow_dev is False")

    def test_missing_policy_file(self) -> None:
        def fake_load_title_config_missing():
            return None, [str(POLICY_PATH)]

        with mock.patch.object(self.validator, "_load_title_config", fake_load_title_config_missing):
            errs = self.validator.validate_title("[TASK] 2-1. X Y Z. G. OMP. Title.")
            self.assertTrue(errs, "Expected error for missing policy config")
            self.assertIn("policy config not found", " ".join(errs).lower())

    def test_missing_technology_block(self) -> None:
        title = (
            "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора."
        )
        errs = self.validator.validate_title(title)
        self.assertTrue(errs, "Expected error when technology block is missing")

    def test_invalid_technology_token(self) -> None:
        title = (
            "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. CUDA. Вычисление суммы элементов вектора."
        )
        errs = self.validator.validate_title(title)
        self.assertTrue(errs, "Expected error for unsupported technology token")

