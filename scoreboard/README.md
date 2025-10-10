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

## Configuration

- `data/points-info.yml` - Task points, deadlines, penalties
- `data/plagiarism.yml` - Flagged submissions
- `data/deadlines.yml` - Optional display deadlines and day offsets

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

- Threads deadlines are auto-distributed across the Spring window: 1 Feb → 15 May.
- Processes deadlines are auto-distributed across the Autumn window: 15 Oct → 14 Dec.
- Due time is 23:59 MSK on the shown date.
- File `data/deadlines.yml` can shift dates per item by integer day offsets (default 0). Example:

```
threads:
  seq: 0     # no shift
  omp: -2    # 2 days earlier
  tbb: 3     # 3 days later
  stl: 0
  all: 0
processes:
  task_1: 0
  task_2: 5
  task_3: -1
```

- If you put a non-integer string instead of a number, it is used as-is as the label (e.g., `"10 Nov"`).
