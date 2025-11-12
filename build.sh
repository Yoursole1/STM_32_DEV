set -e
set -o pipefail

BUILD_DIR="./src/build/"
OPENOCD_CMD=(openocd -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg -c "program titan.elf verify reset exit")

# detecting OS
OS_TYPE=$(uname -s)
case "$OS_TYPE" in
  Darwin)
    USB_RESET_BIN="./stm_usb/stm_usb_mac"
    ;;
  Linux)
    USB_RESET_BIN="./stm_usb/stm_usb_linux"
    ;;
  MINGW*|MSYS*|CYGWIN*|Windows_NT)
    USB_RESET_BIN="./stm_usb/stm_usb_win.exe"
    ;;
  *)
    echo "Unsupported OS: $OS_TYPE"
    exit 1
    ;;
esac

# if binary file not found
if [ ! -f "$USB_RESET_BIN" ]; then
  echo "USB reset binary not found: $USB_RESET_BIN"
  exit 2
fi

echo "Running USB reset ($USB_RESET_BIN)..."
"$USB_RESET_BIN"
USB_RC=$?
if [ $USB_RC -ne 0 ]; then
  echo "USB reset failed (rc=$USB_RC). Aborting."
  exit 3
fi


cd "src"
mkdir "build/"
cd "build/"

echo "Running cmake .."
cmake .. || { echo "cmake failed"; popd >/dev/null; exit 20; }

echo "Running make"
make || { echo "make failed"; popd >/dev/null; exit 21; }


# openocd
echo "Flashing with OpenOCD..."
OPENOCD_OUTPUT_FILE="$(mktemp)"
("${OPENOCD_CMD[@]}" 2>&1 | tee "$OPENOCD_OUTPUT_FILE")
OPENOCD_RC=${PIPESTATUS[0]}

# Check for common errors
if grep -Eiq "failed|error|no device|no such file|target not halted|no connection|failed to connect|controller not found" "$OPENOCD_OUTPUT_FILE"; then
  echo "OpenOCD reported errors. Inspect output:"
  sed -n '1,200p' "$OPENOCD_OUTPUT_FILE"
  echo "If programming failed due to target state, try a manual reset or power cycle."
  rm -f "$OPENOCD_OUTPUT_FILE"
  exit 30
fi

# Success
rm -f "$OPENOCD_OUTPUT_FILE"
if [ $OPENOCD_RC -eq 0 ]; then
  echo "Flash successful."
  exit 0
else
  echo "OpenOCD exited with code $OPENOCD_RC"
  exit 31
fi