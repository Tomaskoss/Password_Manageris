#!/bin/sh

# Setup script name
name=$(basename $0)

# Get a list of all changed text files
TEXT_FILES=$(git diff --cached --name-only --diff-filter=ACM)

# Check each file
for FILE in $TEXT_FILES; do
    # Skip files in ThirdParty directory
    if echo "$FILE" | grep -q "^ThirdParty/"; then
        continue
    fi

    # Check if the file is a text file
    if file "$FILE" | grep -q text; then
        # Use 'tail -c 1' to get the last character of the file
        LAST_CHAR=$(tail -c 1 "$FILE")

        # If the last character isn't a newline, print an error message and exit
        if [ "$LAST_CHAR" != "" ]; then
            echo "$name: File $FILE does not end with a newline."
            exit 1
        fi
    fi
done

# If we haven't exited by now, all files are fine
exit 0
