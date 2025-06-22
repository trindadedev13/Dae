#!/usr/bin/bash

set -e

INSTALL_P=false
RUN_P=false

TERMUX_P=false
ASAN_P=false
CLEAN_P=true

YELLOW="\e[33m"
LGREEN="\e[92m"
BLUE="\e[34m"
LMAGENTA="\e[95m"
RESET="\e[0m"

print_help_and_close() {
  echo -e "$BLUE Script to compile & install dae"
  echo -e "$RESET"
  echo "Usage Make.sh <option>"
  echo "Options:"
  echo -e "$LGREEN-i  or --install | $LMAGENTA Compile and install."
  echo -e "$LGREEN-r  or --run     | $LMAGENTA Compile and run."
  echo -e "$LGREEN-t  or --termux  | $LMAGENTA [USE WITH -r] Compile and run it fixing termux restrictions."
  echo -e "$LGREEN-h  or --help    | $LMAGENTA Prints help."
  echo -e "$LGREEN-as or --asan    | $LMAGENTA Enables Address Sanitizer."
  echo -e "$LGREEN-c  or --clean   | $LMAGENTA Cleanup build before build again."
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
      INSTALL_P=true
      ;;
    --install)
      INSTALL_P=true
      ;;
    -r)
      RUN_P=true
      ;;
    --run)
      RUN_P=true
      ;;
    -t)
      TERMUX_P=true
      ;;
    --termux)
      TERMUX_P=true
      ;;
    -as)
      ASAN_P=true
      ;;
    --asan)
      ASAN_P=true
      ;;
    -c)
      CLEAN_P=true
      ;;
    --clean)
      CLEAN_P=true
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

if [ "$ASAN_P" = true ]; then
  export ASAN=true
fi

if [ "$CLEAN_P" = true ]; then
  rm -rf build
fi

# Creates build dir (if needed) and enter in it
mkdir -p build
cd build

# Create CMake Build files based on INSTALL option
if [ "$INSTALL_P" = true ]; then
  export INSTALL=$INSTALL_P
  cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/../usr
else
  cmake ..
fi

# Finally build
make

# Install program if install is true
if [ "$INSTALL_P" = true ]; then
  cmake --install .
fi

# Runs program if install option is false
if [ "$INSTALL_P" = false ]; then
  if [ "$RUN_P" = true ]; then
    if [ "$TERMUX_P" = true ]; then
      cp dae $HOME
      cd $HOME
      chmod +x dae
    else
      chmod +x dae
    fi
    ./dae run main.dae
  fi
fi