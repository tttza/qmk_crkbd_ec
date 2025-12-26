#!/usr/bin/env bash
set -euo pipefail

KB="sekigon/crkbd_ec"
KM="tttza"
OUT_DIR="${OUT_DIR:-artifacts}"
mkdir -p "$OUT_DIR"

# Variants to build: name|extra_env
variants=(
  "default|"
  "master_left|MASTER=left"
  "master_right|MASTER=right"
  "ee_hands|EE_HANDS=1"
  "right_master_usb_detect|MASTER=right SPLIT_USB_DETECT=yes EE_HANDS=1"
)

run_build() {
  local name="$1"
  local envs="$2"
  echo "==== Building ${name} ===="
  if [[ -z "$envs" ]]; then
    qmk compile -kb "$KB" -km "$KM"
  else
    qmk compile -kb "$KB" -km "$KM" -e "$envs"
  fi
  local base="sekigon_crkbd_ec_${KM}"
  if [[ -f "${base}.uf2" ]]; then
    cp "${base}.uf2" "$OUT_DIR/${base}_${name}.uf2"
  fi
  if [[ -f ".build/${base}.bin" ]]; then
    cp ".build/${base}.bin" "$OUT_DIR/${base}_${name}.bin"
  fi
  echo "==== Done ${name} ===="
}

for v in "${variants[@]}"; do
  name="${v%%|*}"
  envs="${v#*|}"
  run_build "$name" "$envs"
  echo
done

echo "All builds finished."
