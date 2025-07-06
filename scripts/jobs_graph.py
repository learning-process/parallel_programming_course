#!/usr/bin/env python3
"""Script to generate job dependency graphs from GitHub Actions workflow files."""

import os
import sys

try:
    import yaml
except ImportError:
    print("Please install pyyaml: pip install pyyaml")
    sys.exit(1)

try:
    import graphviz
except ImportError:
    print("Please install graphviz: pip install graphviz")
    sys.exit(1)


def parse_gha_yml(file_path):
    """Parse GitHub Actions YAML workflow file."""
    with open(file_path, "r", encoding='utf-8') as file:
        data = yaml.safe_load(file)
    return data


def build_jobs_graph(workflow_data):
    """Build a dependency graph from workflow jobs data."""
    jobs = workflow_data.get("jobs", {})
    dot = graphviz.Digraph()

    for job_name, job_data in jobs.items():
        dot.node(job_name)
        needs = job_data.get("needs", [])
        if isinstance(needs, str):
            needs = [needs]
        for dependency in needs:
            dot.edge(dependency, job_name)

    return dot


def save_graph(dot, filename, file_format):
    """Save the graph in the specified format."""
    dot.render(filename, format=file_format, cleanup=True)


if __name__ == "__main__":
    gha_file_path = os.path.join(".github", "workflows", "main.yml")
    svg_path = os.path.join("docs", "_static", "ci_graph")
    gha_data = parse_gha_yml(gha_file_path)
    jobs_graph = build_jobs_graph(gha_data)
    save_graph(jobs_graph, svg_path, "svg")