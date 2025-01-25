import os

try:
    import yaml
except ImportError:
    print("Please install pyyaml: pip install pyyaml")
    exit(1)

try:
    import graphviz
except ImportError:
    print("Please install graphviz: pip install graphviz")
    exit(1)


def parse_gha_yml(file_path):
    with open(file_path, "r") as file:
        gha_data = yaml.safe_load(file)
    return gha_data


def build_jobs_graph(gha_data):
    jobs = gha_data.get("jobs", {})
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
    dot.render(filename, format=file_format, cleanup=True)


if __name__ == "__main__":
    gha_file_path = os.path.join(".github", "workflows", "main.yml")
    svg_path = os.path.join("docs", "_static", "ci_graph")
    gha_data = parse_gha_yml(gha_file_path)
    jobs_graph = build_jobs_graph(gha_data)
    save_graph(jobs_graph, svg_path, "svg")
