# Parallel Programming Course Documentation

### How to make and test your changes

1. Navigate to the documentation directory:
```bash
cd parallel_programming_course/docs
```

2. (Optional) Create a virtual environment:
```bash
python3 -m venv venv
```

3. Install the required dependencies:
```bash
pip install -r requirements.txt
```

4. Build the documentation:
```bash
make html
```

5. Update the locale files:
```bash
make update
```

6. Re-build the documentation:
```bash
make html
```

7. Make local deployment of the changes:
```bash
cd _build/html
python3 -m http.server 8080
```

8. Open the documentation in your browser:
```bash
open http://localhost:8080/en
open http://localhost:8080/ru
```
