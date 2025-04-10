# Developer documentation

This readme-file documents how to make changes to the M-Bus stack and is meant
only for developers of the stack. For documentation about how to use the stack,
see [the online manual](https://docs.rt-labs.com/m-bus/).


## How update the manual

Please see docs/README.rst.


## How to perform static analysis on the stack

1. Install the clang-tidy tool on Linux:

```bash
sudo apt install clang clang-format clang-tidy python3
```

2. Run the clang-tidy static analysis tool:

```bash
cmake -B build/analysis -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy -p build/analysis
```

There should be no warnings or errors.

