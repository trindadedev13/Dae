# Kilate
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

## How to install Kilate-Interpreter

### 🐧 Linux

> Requirements: ruby, cmake, make, C++ compiler (g++ or clang)

### 1. Clone the repository

```bash
git clone https://github.com/kilate-lang/kilate
cd kilate
```

### 3. Install

```
ruby build.rb --install
```

## How to use Kilate-Interpreter
Below we have the basic interpreter commands.

## Run
```bash
kilate run <files>
```

## Help
```bash
kilate help
```