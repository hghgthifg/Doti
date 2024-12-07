#!/bin/bash

# Check if a directory is provided
if [ -z "$1" ]; then
  echo "Usage: $0 target_directory"
  exit 1
fi

TARGET_DIR="$1"

# Verify the directory exists
if [ ! -d "$TARGET_DIR" ]; then
  echo "Directory '$TARGET_DIR' does not exist."
  exit 1
fi

# Convert all .png files to .jxl
shopt -s nullglob
for f in "$TARGET_DIR"/*.png; do
  outfile="${f%.png}.jxl"
  echo "Converting $f to $outfile"
  cjxl "$f" "$outfile" --distance 0
done

# Backup and replace .png with .jxl in all .mtl files
for mtl in "$TARGET_DIR"/*.mtl; do
  if [ -f "$mtl" ]; then
    backup_file="${mtl}.bak"
    echo "Creating backup of $mtl as $backup_file"
    cp "$mtl" "$backup_file"

    echo "Updating references in $mtl from .png to .jxl"
    sed -i 's/\.png/\.jxl/g' "$mtl"
  fi
done

echo "All operations completed."