#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Write the PPC MPI runtime environment JSON file."
    )
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--mpi-extensions-home", required=True)
    parser.add_argument("--mpi-exec", required=True)
    parser.add_argument("--path-prepend", required=True)
    parser.add_argument("--library-path-prepend", required=True)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    runtime_config = {
        "mpi_extensions_home": args.mpi_extensions_home,
        "mpi_exec": args.mpi_exec,
        "path_prepend": args.path_prepend,
        "library_path_prepend": args.library_path_prepend,
        "env": {
            "MPI_EXTENSIONS_HOME": args.mpi_extensions_home,
            "MPI_HOME": args.mpi_extensions_home,
            "OPAL_PREFIX": args.mpi_extensions_home,
            "OMPI_MCA_shmem": "mmap",
        },
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(
        json.dumps(runtime_config, indent=2) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
