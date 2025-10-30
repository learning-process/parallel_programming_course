Report
======

Overview and placement
----------------------
- The report is a Markdown file placed inside your task directory:
  ``tasks/<last>_<initial>_<short>/report.md``.
- Presence of ``report.md`` is required to receive report (R) points for that task (see ``Points``).
- Write the report concisely with enough detail to reproduce the results (environment, commands, data generation). Use English or Russian consistently.

Report points
-------------

  +--------------+--------------+--------------------+-------+
  | Completeness | Text Quality | Formatting Quality | Total |
  +==============+==============+====================+=======+
  | 5            | 2.5          | 2.5                | 10    |
  +--------------+--------------+--------------------+-------+

Recommended structure
---------------------
- Introduction (short context and goal)
- Problem Statement (task definition and constraints)
- Algorithm Description (baseline/sequential)
- Parallelization Scheme (MPI topology/data flow or threads scheduling)
- Experimental Results
  - Environment (CPU, OS, compiler, build type; thread/process counts)
  - Execution time tables/plots; performance metrics (speedup/efficiency)
  - Correctness checks description
- Conclusions (what worked, what didn’t)
- References (articles, docs; provide links)
- Appendix (optional code snippets)

Evaluation criteria
-------------------
- Completeness
  - All items from the recommended structure are present and meaningful
  - Results include both correctness and performance evidence
- Text Quality
  - Clarity, coherence, correct terminology; acceptable language quality
- Formatting Quality
  - Proper Markdown headings, lists, tables, fenced code blocks
  - Figures/plots with captions and relative paths; readable formatting

Notes
-----
- Reports are reviewed online in the Pull Request. The final grade is based on the submitted version that passes the review.
- If the task is disabled by the end of semester (e.g., fails in master; see scoreboard “blue zone”), report points for that task are nullified.
- Plagiarism is prohibited; identical or near‑duplicate reports will be rejected.
Markdown template (report.md)
-----------------------------

Use the following skeleton as a starting point for your ``report.md``. Keep file paths relative to the task directory (for images/data).

.. code-block:: markdown

   # <Task Full Name>
   
   - Student: <Last First Middle>, group <Group>
   - Technology: <SEQ | MPI | OMP | TBB | STL | ALL>
   - Variant: <N>
   
   ## 1. Introduction
   Brief motivation, problem context, expected outcome.
   
   ## 2. Problem Statement
   Formal task definition, input/output format, constraints.
   
   ## 3. Baseline Algorithm (Sequential)
   Describe the base algorithm with enough detail to reproduce.
   
   ## 4. Parallelization Scheme
   - For MPI: data distribution, communication pattern/topology, rank roles.
   - For threads: decomposition, scheduling, synchronization.
   Diagrams or short pseudocode are welcome.
   
   ## 5. Implementation Details
   - Code structure (files, key classes/functions)
   - Important assumptions and corner cases
   - Memory usage considerations
   
   ## 6. Experimental Setup
   - Hardware/OS: CPU model, cores/threads, RAM, OS version
   - Toolchain: compiler, version, build type (Release/RelWithDebInfo)
   - Environment: PPC_NUM_THREADS / PPC_NUM_PROC, other relevant vars
   - Data: how test data is generated or sourced (relative paths)
   
   ## 7. Results and Discussion
   
   ### 7.1 Correctness
   Briefly explain how correctness was verified (reference results, invariants, unit tests).
   
   ### 7.2 Performance
   Present time, speedup and efficiency. Example table:
   
   | Mode        | Count | Time, s | Speedup | Efficiency |
   |-------------|-------|---------|---------|------------|
   | seq         | 1     | 1.234   | 1.00    | N/A        |
   | omp         | 2     | 0.700   | 1.76    | 88.0%      |
   | omp         | 4     | 0.390   | 3.16    | 79.0%      |
   
   Optionally add plots (use relative paths), and discuss bottlenecks and scalability limits.
   
   ## 8. Conclusions
   Summarize findings and limitations.
   
   ## 9. References
   1. <Article/Book/Doc URL>
   2. <Another source>
   
   ## Appendix (Optional)
   ```cpp
   // Short, readable code excerpts if needed
   ```
