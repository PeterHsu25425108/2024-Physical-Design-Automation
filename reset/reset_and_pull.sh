#!/bin/bash

# Ensure the script is run from a Git repository
echo "Checking if this is a Git repository..."
if [ ! -d .git ]; then
  echo "Error: This directory is not a Git repository."
  exit 1
fi

# Discard all local changes
echo "Resetting local changes to last committed state..."
git reset --hard

# Remove untracked files and directories
echo "Cleaning up untracked files and directories..."
git clean -fd

# Fetch the latest changes from the remote repository
echo "Fetching latest changes from remote..."
git fetch origin

# Reset the local branch to match the remote branch
BRANCH=$(git rev-parse --abbrev-ref HEAD)
echo "Resetting local branch '$BRANCH' to match 'origin/$BRANCH'..."
git reset --hard origin/$BRANCH

# Confirm success
echo "Repository successfully reset and updated from remote."
