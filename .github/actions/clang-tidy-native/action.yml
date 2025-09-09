name: 'Native Clang-Tidy Analysis'
description: 'Run clang-tidy analysis without Docker'
inputs:
  build_dir:
    description: 'Build directory for CMake'
    required: false
    default: 'build'
  exclude:
    description: 'Directories to exclude from analysis (space-separated)'
    required: false
    default: '3rdparty'
  clang_tidy_version:
    description: 'Clang-tidy version to use'
    required: false
    default: '21'
outputs:
  total_comments:
    description: 'Total number of clang-tidy issues found'
    value: ${{ steps.analyze.outputs.total_comments }}
runs:
  using: 'composite'
  steps:
    - name: Verify clang-tidy installation
      shell: bash
      run: |
        clang-tidy-${{ inputs.clang_tidy_version }} --version

    - name: Get changed files
      id: changed-files
      shell: bash
      run: |
        git config --global --add safe.directory $GITHUB_WORKSPACE
        git fetch origin ${{ github.event.pull_request.base.ref }}

        # Check if .clang-tidy files were changed
        CLANG_TIDY_CHANGED=$(git diff --name-only \
          origin/${{ github.event.pull_request.base.ref }}...HEAD \
          -- '**/.clang-tidy' || true)

        if [ -n "$CLANG_TIDY_CHANGED" ]; then
          echo "::notice::.clang-tidy configuration changed, analyzing all source files"
          # Find all source files in the repository (excluding specified directories)
          CHANGED_FILES=$(find . -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h")
          # Filter out excluded directories
          for exclude_dir in ${{ inputs.exclude }}; do
            CHANGED_FILES=$(echo "$CHANGED_FILES" | grep -v "^./${exclude_dir}/" || true)
          done
        else
          # Only analyze changed source files
          CHANGED_FILES=$(git diff --name-only \
            origin/${{ github.event.pull_request.base.ref }}...HEAD \
            -- '*.cpp' '*.hpp' '*.c' '*.h')
          # Filter out excluded directories
          for exclude_dir in ${{ inputs.exclude }}; do
            CHANGED_FILES=$(echo "$CHANGED_FILES" | grep -v "^${exclude_dir}/" || true)
          done
        fi

        echo "changed_files<<EOF" >> $GITHUB_OUTPUT
        echo "$CHANGED_FILES" >> $GITHUB_OUTPUT
        echo "EOF" >> $GITHUB_OUTPUT

        if [ -z "$CHANGED_FILES" ]; then
          echo "has_changes=false" >> $GITHUB_OUTPUT
        else
          echo "has_changes=true" >> $GITHUB_OUTPUT
        fi

    - name: Run clang-tidy analysis
      id: analyze
      shell: bash
      if: steps.changed-files.outputs.has_changes == 'true'
      run: |
        COMMENTS_FILE=$(mktemp)
        TOTAL_ISSUES=0

        while IFS= read -r file; do
          if [ -n "$file" ] && [ -f "$file" ]; then
            echo "Analyzing $file..."
            FILE_OUTPUT=$(mktemp)
            if clang-tidy-${{ inputs.clang_tidy_version }} "$file" \
               -p ${{ inputs.build_dir }} --format-style=file 2>&1 | \
               tee "$FILE_OUTPUT"; then
              ISSUES=$(grep -c "warning:\|error:" "$FILE_OUTPUT" 2>/dev/null || echo "0")
              ISSUES=$(echo "$ISSUES" | tr -d '[:space:]')
              TOTAL_ISSUES=$((TOTAL_ISSUES + ${ISSUES:-0}))
              cat "$FILE_OUTPUT" >> "$COMMENTS_FILE"
            else
              echo "::error::Failed to analyze $file"
              TOTAL_ISSUES=$((TOTAL_ISSUES + 1))
            fi
            rm -f "$FILE_OUTPUT"
          fi
        done <<< "${{ steps.changed-files.outputs.changed_files }}"

        echo "total_comments=$TOTAL_ISSUES" >> $GITHUB_OUTPUT

        if [ -f "$COMMENTS_FILE" ] && [ -s "$COMMENTS_FILE" ]; then
          echo "::group::Clang-tidy Analysis Results"
          cat "$COMMENTS_FILE"
          echo "::endgroup::"
        fi

        if [ "$TOTAL_ISSUES" -gt 0 ]; then
          echo "::error::Found $TOTAL_ISSUES clang-tidy issues"
        else
          echo "No clang-tidy issues found"
        fi

    - name: Set output for no changes
      shell: bash
      if: steps.changed-files.outputs.has_changes == 'false'
      run: |
        echo "total_comments=0" >> $GITHUB_OUTPUT
