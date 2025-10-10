# file: assign_variant.py
"""
Deterministic variant assignment from Full Name + Group
with the repository name as the ONLY salt.

Algorithm:
  1) Normalize strings (NFKC, trim, lowercase, map 'ё'->'е', collapse spaces).
  2) Build a key: "surname|name|patronymic|group|repo".
  3) SHA-256(key) -> big integer -> modulo `num_variants`.

Properties:
- Stable: same inputs → same output.
- Uniform: modulo of a cryptographic hash distributes evenly.
- Note: Without the full group roster, zero collisions cannot be *guaranteed*
  (birthday paradox). This is intended for “approximately unique” per-group use.

Usage:
    from assign_variant import assign_variant
    v = assign_variant(
        surname="Petrov",
        name="Pyotr",
        patronymic="Petrovich",
        group="MEN-201",
        repo="learning-process/parallel_programming_course",
        num_variants=31,  # produces values in 0..30
    )
    print(v)
"""

from __future__ import annotations

import hashlib
import re
import unicodedata
from typing import Optional

__all__ = ["assign_variant", "normalize"]


def normalize(s: Optional[str]) -> str:
    """
    Normalize a string:
      - Unicode NFKC,
      - trim,
      - lowercase,
      - map Cyrillic 'ё' -> 'е' (common normalization in Russian names),
      - collapse multiple spaces to a single space.

    None -> '' (empty string).
    """
    if not s:
        return ""
    s = unicodedata.normalize("NFKC", s).strip().lower()
    s = s.replace("ё", "е")
    s = re.sub(r"\s+", " ", s)
    return s


def _hash_int(key: str) -> int:
    """Return SHA-256(key) as a big integer."""
    return int.from_bytes(hashlib.sha256(key.encode("utf-8")).digest(), "big")


def assign_variant(
    surname: str,
    name: str,
    group: str,
    repo: str,
    patronymic: Optional[str] = "",
    num_variants: int = 31,
) -> int:
    """
    Deterministically returns a variant index in [0 .. num_variants-1]
    based on (surname, name, patronymic, group) and the repository name (repo)
    as the sole salt.

    :param surname: Last name
    :param name: First name
    :param group: Group identifier (e.g., "MEN-201")
    :param repo: Repository name used as salt (e.g., "org/repo" or just "repo")
    :param patronymic: Middle name / patronymic (optional)
    :param num_variants: Total number of variants (> 0). Output range: 0..num_variants-1
    :return: int — the variant index
    """
    if not isinstance(num_variants, int) or num_variants < 1:
        raise ValueError("num_variants must be a positive integer (> 0)")
    if not repo or not isinstance(repo, str):
        raise ValueError("repo must be a non-empty string")

    key = "|".join(
        (
            normalize(surname),
            normalize(name),
            normalize(patronymic),
            normalize(group),
            normalize(repo),
        )
    )
    h = _hash_int(key)
    return h % num_variants


# Minimal self-check when executed directly (no CLI arguments).
if __name__ == "__main__":

    def demo():
        print("Demo: deterministic assignment\n")

        v1 = assign_variant(
            surname="Петров",
            name="Пётр",
            patronymic="Петрович",
            group="МЕН-201",
            repo="learning-process/parallel_programming_course",
            num_variants=31,
        )
        # Different casing/spacing/ё→е should not change the result:
        v2 = assign_variant(
            surname="ПЕТРОВ",
            name="петр ",
            patronymic="пЕТРОВИЧ",
            group="  мен-201 ",
            repo="learning-process/parallel_programming_course",
            num_variants=31,
        )
        assert v1 == v2, "Normalization should make results identical"

        v_other_repo = assign_variant(
            surname="Petrov",
            name="Pyotr",
            patronymic="Petrovich",
            group="MEN-201",
            repo="learning-process/ppc_2025_fall",  # different salt → likely different value
            num_variants=31,
        )

        print(f"Variant (repo=A): {v1}")
        print(f"Variant (same inputs, normalized): {v2}")
        print(f"Variant (repo=B): {v_other_repo}")
        print("\nOK: deterministic & normalized.")

    demo()
