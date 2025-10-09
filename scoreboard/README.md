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

## Testing

```bash
# Install test dependencies
pip install -r tests/requirements.txt

# Run tests
python -m pytest tests/ -v
```

## Output

HTML table with columns: S (solution), A (acceleration), E (efficiency), D (deadline), P (plagiarism), Total.
