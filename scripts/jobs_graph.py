import argparse
import os
import subprocess

try:
    import yaml
except ImportError:
    print("Please install pyyaml: pip install pyyaml")
    exit(1)

EXCLUDED_JOBS = {"ci-scope"}


def parse_gha_yml(file_path):
    with open(file_path, "r") as file:
        gha_data = yaml.safe_load(file)
    return gha_data


def build_jobs_graph(gha_data):
    jobs = filter_jobs(gha_data.get("jobs", {}))
    dot = [
        "digraph CI_Overview {",
        '  graph [rankdir="LR", ranksep="1.0", nodesep="0.5", splines="ortho"];',
        '  node [shape="box", style="rounded", fontname="Helvetica"];',
        '  edge [color="#333333"];',
    ]

    for job_name, job_data in jobs.items():
        dot.append(f"  {quote_dot_id(job_name)};")
        needs = job_data.get("needs", [])
        if isinstance(needs, str):
            needs = [needs]
        for dependency in needs:
            if dependency not in jobs:
                continue
            dot.append(f"  {quote_dot_id(dependency)} -> {quote_dot_id(job_name)};")

    for rank_jobs in get_ranked_jobs(jobs).values():
        same_rank_jobs = " ".join(
            f"{quote_dot_id(job_name)};" for job_name in rank_jobs
        )
        dot.append(f"  {{ rank=same; {same_rank_jobs} }}")

    dot.append("}")
    return "\n".join(dot)


def filter_jobs(jobs):
    return {
        job_name: job_data
        for job_name, job_data in jobs.items()
        if job_name not in EXCLUDED_JOBS
    }


def quote_dot_id(value):
    return f'"{value.replace("\\", "\\\\").replace('"', '\\"')}"'


def get_ranked_jobs(jobs):
    ranks = {}

    def get_rank(job_name):
        if job_name in ranks:
            return ranks[job_name]

        needs = jobs[job_name].get("needs", [])
        if isinstance(needs, str):
            needs = [needs]
        needs = [dependency for dependency in needs if dependency in jobs]

        if not needs:
            ranks[job_name] = 0
        else:
            ranks[job_name] = max(get_rank(dependency) for dependency in needs) + 1
        return ranks[job_name]

    for job_name in jobs:
        get_rank(job_name)

    ranked_jobs = {}
    for job_name, rank in ranks.items():
        ranked_jobs.setdefault(rank, []).append(job_name)
    return ranked_jobs


def save_graph(dot, filename, file_format):
    subprocess.run(
        ["dot", f"-T{file_format}", "-o", f"{filename}.{file_format}"],
        input=dot,
        text=True,
        check=True,
    )


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate a Graphviz CI jobs graph from a GitHub Actions workflow."
    )
    parser.add_argument(
        "--workflow", default=os.path.join(".github", "workflows", "main.yml")
    )
    parser.add_argument("--out", default=os.path.join("docs", "_static", "ci_graph"))
    parser.add_argument("--format", default="svg")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    gha_data = parse_gha_yml(args.workflow)
    jobs_graph = build_jobs_graph(gha_data)
    save_graph(jobs_graph, args.out, args.format)
