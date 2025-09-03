#!/bin/bash
set -e
if [ -z "$1" ]; then
  echo "Usage: $0 <PORT>"
  exit 1
fi
idf.py build
idf.py -p "$1" flash
