#!/usr/bin/bash

set -e

INSTALL_P=OFF
RUN_P=OFF

TERMUX_P=OFF
ASAN_P=OFF
CLEAN_P=OFF
LIB_SO_P=OFF

YELLOW="\e[33m"
LGREEN="\e[92m"
BLUE="\e[34m"
LMAGENTA="\e[95m"
RESET="\e[0m"

print_help_and_close() {
  echo -e "$BLUE Script to compile & install kilate"
  echo -e "$RESET"
  echo "Usage Make.sh <option>"
  echo "Options:"
  echo -e "$LGREEN-i    or  --install | $LMAGENTA Compile and install."
  echo -e "$LGREEN-r    or  --run     | $LMAGENTA Compile and run."
  echo -e "$LGREEN-t    or  --termux  | $LMAGENTA [USE WITH -r] Compile and run it fixing termux restrictions."
  echo -e "$LGREEN-h    or  --help    | $LMAGENTA Prints help."
  echo -e "$LGREEN-as   or  --asan    | $LMAGENTA Enables Address Sanitizer."
  echo -e "$LGREEN-c    or  --clean   | $LMAGENTA Cleanup build before build again."
  echo -e "$LGREEN-lso  or  --libso   | $LMAGENTA Cleanup build before build again."
  echo ""
  echo -e "$YELLOW"
  echo "WARNING"
  echo "Dont use install and run command together."
  echo -e "$RESET"
  exit 0
}

for arg in "$@"; do
  case "$arg" in
    -i)
      INSTALL_P=ON
      ;;
    --install)
      INSTALL_P=ON
      ;;
    -r)
      RUN_P=ON
      ;;
    --run)
      RUN_P=ON
      ;;
    -t)
      TERMUX_P=ON
      ;;
    --termux)
      TERMUX_P=ON
      ;;
    -as)
      ASAN_P=ON
      ;;
    --asan)
      ASAN_P=ON
      ;;
    -c)
      CLEAN_P=ON
      ;;
    --clean)
      CLEAN_P=ON
      ;;
    -lso)
      LIB_SO_P=ON
      ;;
    --libso)
      LIB_SO_P=ON
      ;;
    -h)
      print_help_and_close
      ;;
    --help)
      print_help_and_close
      ;;
    *)
      print_help_and_close
      ;;
  esac
done

if [ "$ASAN_P" = ON ]; then
  export ASAN=ON
fi

if [ "$CLEAN_P" = ON ]; then
  rm -rf build
fi

mkdir -p build

# Create CMake Build files based on INSTALL option
if [ "$LIB_SO_P" = ON ]; then
  ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")
  for ABI in "${ABIS[@]}"; do
    BUILD_DIR=build/$ABI

    cmake -B $BUILD_DIR \
      -DASAN=$ASAN_P \
      -DINSTALL=$INSTALL_P \
      -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=$ABI \
      -DANDROID_PLATFORM=android-26 \
      -DCMAKE_BUILD_TYPE=Release

    cmake --build $BUILD_DIR
  done
else
  cmake -B build \
    -DASAN=$ASAN_P \
    -DINSTALL=$INSTALL_P \
    -DCMAKE_INSTALL_PREFIX=$HOME/../usr
fi

# Finally build
cmake --build build

# Install program if install is ON
if [ "$INSTALL_P" = ON ]; then
  cmake --install build
fi

# Runs program if install option is OFF
if [ "$INSTALL_P" = OFF ]; then
  if [ "$RUN_P" = ON ]; then
    if [ "$TERMUX_P" = ON ]; then
      cp kilate $HOME
      cd $HOME
      chmod +x kilate
    else
      chmod +x kilate
    fi
    ./kilate run main.klt
  fi
fi