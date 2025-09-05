#!/bin/bash
set -e

command -v idf.py >/dev/null || { echo "idf.py introuvable"; exit 1; }

# Optionnel: vérifier que la cible esp32s3 est configurée
if ! idf.py --version 2>/dev/null | grep -q "IDF_TARGET: esp32s3"; then
  echo "Erreur: cible ESP-IDF esp32s3 non configurée."
  exit 1
fi

usage() {
  echo "Usage: $0 <PORT> [--baud <BAUD>] [--erase]"
}

if [ -z "$1" ]; then
  usage
  exit 1
fi

PORT="$1"
shift

BAUD=""
ERASE=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --baud)
      if [[ -n "$2" && "$2" != --* ]]; then
        BAUD="$2"
        shift 2
      else
        echo "Error: --baud requires an argument"
        usage
        exit 1
      fi
      ;;
    --erase)
      ERASE=1
      shift
      ;;
    *)
      echo "Unknown option: $1"
      usage
      exit 1
      ;;
  esac
done

if [ ! -f sdkconfig ] || ! grep -q '^CONFIG_SPIRAM=y' sdkconfig; then
  echo "Erreur: PSRAM non activée dans sdkconfig (CONFIG_SPIRAM)."
  exit 1
fi

idf.py build

FLASH_ARGS=(-p "$PORT")
if [ -n "$BAUD" ]; then
  FLASH_ARGS+=(-b "$BAUD")
fi

if [ "$ERASE" -eq 1 ]; then
  idf.py "${FLASH_ARGS[@]}" erase_flash
fi

idf.py "${FLASH_ARGS[@]}" flash
