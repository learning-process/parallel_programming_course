# Scoreboard

HTML scoreboard generator for parallel programming tasks.

## Usage

```bash
# Install dependencies
pip install -r requirements.txt

# Generate scoreboard
python main.py -o output_directory
```

Generates `output_directory/index.html` with the scoreboard.

To generate it through CMake without C++ project dependencies:

```bash
cmake -S . -B build -DUSE_SCOREBOARD=ON -DPPC_BUILD_COMPONENTS=OFF
cmake --build build -t generate_scoreboard
```

Note: `-DPPC_BUILD_COMPONENTS=OFF` could be used to configure documentation without C++ project dependencies

## Configuration

- `data/points-info.yml` - Task points, deadlines, penalties
- `data/plagiarism.yml` - Flagged submissions
- `data/deadlines.yml` - Optional deadline display labels

## Testing

```bash
# Install test dependencies
pip install -r tests/requirements.txt

# Run tests
python -m pytest tests/ -v
```

## Output

HTML table with columns: S (solution), A (acceleration), E (efficiency), D (deadline), C (copying), Total.

### Deadlines display

- Deadline labels are shown only when explicitly configured in `data/deadlines.yml`.
- Missing entries are hidden.
- Due time is 23:59 MSK on the shown date.
- Example:

```yaml
threads:
  seq: "22 Feb"
  omp: "14 Mar"
  tbb: "4 Apr"
  stl: "24 Apr"
  all: "15 May"
processes:
  task_1: "4 Nov"
  task_2: "24 Nov"
  task_3: "14 Dec"
```
