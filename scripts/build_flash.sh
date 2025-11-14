#!/bin/bash
set -e

command -v idf.py >/dev/null || { echo "idf.py introuvable"; exit 1; }

detect_current_target() {
  # Priorité: variable d'environnement (ESP-IDF >=6.1), idf.py get-target, puis sdkconfig
  if [ -n "$IDF_TARGET" ]; then
    echo "$IDF_TARGET"
    return 0
  fi

  if TARGET_LINE=$(idf.py get-target 2>/dev/null | tail -n1); then
    TARGET_LINE=${TARGET_LINE//$'\r'/}
    TARGET_CLEAN=${TARGET_LINE##*: }
    TARGET_CLEAN=${TARGET_CLEAN##* }
    TARGET_CLEAN=$(printf '%s' "$TARGET_CLEAN" | tr '[:upper:]' '[:lower:]')
    if [ -n "$TARGET_CLEAN" ]; then
      echo "$TARGET_CLEAN"
      return 0
    fi
  fi

  if TARGET_LINE=$(idf.py show-targets 2>/dev/null | awk -F': ' '/Current target/ {print $2; exit}'); then
    TARGET_LINE=${TARGET_LINE//$'\r'/}
    TARGET_LINE=$(printf '%s' "$TARGET_LINE" | tr '[:upper:]' '[:lower:]')
    if [ -n "$TARGET_LINE" ]; then
      echo "$TARGET_LINE"
      return 0
    fi
  fi

  if [ -f sdkconfig ]; then
    TARGET_VALUE=$(awk -F '"' '/^CONFIG_IDF_TARGET=/ { print $2; exit }' sdkconfig)
    if [ -n "$TARGET_VALUE" ]; then
      echo "$TARGET_VALUE"
      return 0
    fi
  fi

  return 1
}

CURRENT_TARGET=$(detect_current_target)
if [ -z "$CURRENT_TARGET" ]; then
  echo "Erreur: impossible de déterminer la cible ESP-IDF actuelle. Vérifiez votre installation ESP-IDF 6.1 (idf.py set-target esp32s3)."
  exit 1
fi

if [ "$CURRENT_TARGET" != "esp32s3" ]; then
  echo "Erreur: cible ESP-IDF détectée '$CURRENT_TARGET'. Configurez esp32s3 (idf.py set-target esp32s3)."
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
