#!/bin/bash

# Set the source folder (change this if needed)
SOURCE_FOLDER="./"

# Set the build directory
BUILD_DIR="../build"

# Set the output zip file name
ZIP_FILE="$BUILD_DIR/arranger.zip"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Find all matching files and store them in a variable
FILES_TO_ZIP=$(find "$SOURCE_FOLDER" -type f \( -iname "*.txt" -o -iname "*.wav" -o -iname "*.pd" -o -iname "*.mmp" \) ! -iname "LICENSE")

# Print the files being compressed
echo "Files to be compressed:"
echo "$FILES_TO_ZIP"
echo "-----------------------------------"

# Zip the files
echo "$FILES_TO_ZIP" | tr '\n' '\0' | xargs -0 zip -j -r "$ZIP_FILE" -0

echo "✅ Documents compressed to $ZIP_FILE"
