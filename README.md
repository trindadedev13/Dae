# Dae
- An easy interpreted programming language.

## Syntax
```
work main(): bool {
  // We have 3 ways to declare variables
  // they are all the same, the only difference is the syntax.
  let helloMessage = "Hello, world!"
  var helloMessage = "Hello, world!"
  string helloMessage = "Hello, world!"

  print -> helloMessage
  return -> false
}
```

## How to install Dae-Interpreter

### 🐧 Linux (ou Windows com WSL)

> Requirements: cmake, make, C++ compiler (g++ or clang)

### 1. Clone the repository

```bash
git clone https://github.com/trindadedev13/dae.git
cd dae-c
```

### 2. Allow make.sh to run

```bash
chmod +x Make.sh
```

### 3. Install

```
./Make.sh -i
```

## How to use Dae-Interpreter
Below we have the basic interpreter commands.

## Run
```bash
dae run <files>
```

## Help
```bash
dae help
```