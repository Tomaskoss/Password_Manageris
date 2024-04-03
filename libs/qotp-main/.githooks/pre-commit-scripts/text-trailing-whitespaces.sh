#!/bin/bash

# Setup script name
name=$(basename $0)

# Find all text files (as per .gitattributes)
TEXT_FILES=$(git diff --cached --name-only --diff-filter=ACM)

# Flag to indicate whether the commit should be aborted
abort_commit=false

# Loop over text files
for FILE in $TEXT_FILES; do
  # Skip files in ThirdParty directory
  if echo "$FILE" | grep -q "^ThirdParty/"; then
      continue
  fi

  if file "$FILE" | grep -q text; then
    # If the file has a line ending with a whitespace, output a warning
    if grep -q '[[:blank:]]$' "$FILE"; then
      echo "$FILE:"
      echo ""
      abort_commit=true

      # Output the faulting lines.
      grep -n '[[:blank:]]$' "$FILE";
      echo ""
    fi
  fi
done

# Abort the commit if necessary
if $abort_commit; then
  echo "$name: The lines above, prefixed with their line numbers, have trailing whitespaces."
  echo "$name: Please remove them and try again."
  echo "$name: Instructions for automatically enabling the removal of trailing white spaces are available in the README.md file for a variety of popular Integrated Development Environments (IDEs)."
  echo "$name: Please refer to these guidelines to appropriately configure your chosen IDE."
  exit 1
fi
