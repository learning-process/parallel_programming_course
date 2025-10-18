import json
import os
import tempfile
from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path
from unittest import TestCase


REPO_ROOT = Path.cwd()
VALIDATOR_PATH = REPO_ROOT / ".github/scripts/validate_pr.py"


def load_validator():
    spec = spec_from_file_location("validate_pr", str(VALIDATOR_PATH))
    assert spec and spec.loader
    mod = module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[attr-defined]
    return mod


class TestMainIntegration(TestCase):
    def setUp(self) -> None:
        self.validator = load_validator()
        self._old_event_path = os.environ.get("GITHUB_EVENT_PATH")

    def tearDown(self) -> None:
        if self._old_event_path is None:
            os.environ.pop("GITHUB_EVENT_PATH", None)
        else:
            os.environ["GITHUB_EVENT_PATH"] = self._old_event_path

    def _with_event(self, title: str) -> str:
        payload = {"pull_request": {"title": title}}
        fd, path = tempfile.mkstemp(prefix="gh-event-", suffix=".json")
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            json.dump(payload, f)
        os.environ["GITHUB_EVENT_PATH"] = path
        return path

    def test_main_ok(self) -> None:
        self._with_event(
            "[TASK] 2-12. Иванов Иван Иванович. 2341-а234. OMP. Вычисление суммы элементов вектора."
        )
        rc = self.validator.main()
        self.assertEqual(rc, 0)

    def test_main_fail(self) -> None:
        self._with_event("Bad title format")
        rc = self.validator.main()
        self.assertEqual(rc, 1)

