#!/bin/sh

# Setup script name
name=$(basename $0)

# Determine the OS
os=$(uname)

# Get the core.autocrlf configuration for the current repository
autocrlf=$(git config --local core.autocrlf)

# Check the setting based on the OS
if [[ "$os" == "Windows_NT" ]] || [[ "$os" == MINGW64_NT-* ]] || [[ "$os" == MSYS_NT-* ]]; then
  if [ "$autocrlf" != "true" ]; then
    echo "$name: core.autocrlf should be set to 'true' on Windows."
    echo "$name: Please run 'git config --local core.autocrlf true'."
    exit 1
  fi
elif [ "$os" = "Linux" ] || [ "$os" = "Darwin" ]; then
  if [ "$autocrlf" != "input" ]; then
    echo "$name: core.autocrlf should be set to 'input' on Unix/Linux/Mac."
    echo "$name: Please run 'git config --local core.autocrlf input'."
    exit 1
  fi
else
  echo "$name: Unknown OS <$os>. Please manually check your core.autocrlf setting."
  exit 1
fi

exit 0
