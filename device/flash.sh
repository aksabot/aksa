#!/bin/sh
# Flash the on-device runtime — the board becomes its own Wi-Fi hotspot +
# editor + interpreter. Assembles device/sketch/ (interpreter core + HAL +
# baked locales) via build.sh, then compiles and uploads with arduino-cli.
# Retries the upload once on the ESP32-C6 native-USB re-enumeration hiccup
# ("exit status 2").
#   ./device/flash.sh [board] [port]
# Defaults: board=c6  port=first /dev/cu.usbmodem*
set -e
cd "$(dirname "$0")/.."

board=${1:-c6}
port=${2:-$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)}

case "$board" in
  c3) fqbn=esp32:esp32:esp32c3 ;;
  c6) fqbn=esp32:esp32:esp32c6 ;;
  *) echo "unknown board '$board' (use c3 or c6)"; exit 2 ;;
esac
[ -n "$port" ] || { echo "no board found — plug in the ESP32 (or pass the port as arg 2)"; exit 1; }

echo "assembling sketch ..."
device/build.sh
echo "compiling for $board ($fqbn) ..."
arduino-cli compile --fqbn "$fqbn" device/sketch
echo "uploading to $port ..."
if ! arduino-cli upload --fqbn "$fqbn" -p "$port" device/sketch; then
    echo "upload failed (C6 native-USB hiccup?) — retrying once ..."
    sleep 1
    port=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)  # may have re-enumerated
    arduino-cli upload --fqbn "$fqbn" -p "$port" device/sketch \
      || { echo "still failing — force bootloader: hold BOOT, tap RESET, release BOOT, then rerun"; exit 1; }
fi
echo "done — board flashed. Join Wi-Fi 'Aksa-XXXX' (password = same as the name), open http://192.168.4.1"
