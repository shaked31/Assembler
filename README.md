# Assembler (MMN 14)

## Overview
This project is a two-pass assembler written in ANSI C for a simulated 32-bit machine architecture.
The assembler translates custom assembly language source code (`.as` files) into machine code, resolving macros, evaluating data directives, and linking symbolic memory addresses. 

## Authors
* **Shaked Pollak**
* **Daniela Aslan**

## Architecture & Pipeline
The assembler processes input files through a strict, modular pipeline:

1. **Pre-Assembler (`pre_assembler.c`):** 
   * Scans the source code for macro definitions (`mcro` and `mcroend`).
   * Saves macros to a dynamic linked list.
   * Expands macro calls in the source code and generates an intermediate `.am` file.
2. **First Pass (`first_pass.c`):**
   * Parses the `.am` file to build a Symbol Table (`symbol_table.c`).
   * Validates instruction syntax, register bounds, and data directive limits.
   * Calculates the memory requirements for the Instruction Counter (IC) and Data Counter (DC).
3. **Second Pass (`second_pass.c`):**
   * Encodes the R-Type, I-Type, and J-Type instructions into 32-bit machine words (`memory_image.h`).
   * Resolves direct and relative memory addresses for branch and jump instructions using the Symbol Table.
   * Tracks `.entry` and `.extern` symbol usages.
4. **File Generation (`file_generator.c`):**
   * Exports the binary machine code into an Object (`.ob`) file, formatted in hexadecimal.
   * Generates Entry (`.ent`) and External (`.ext`) files for the linker if applicable.

## Memory & Error Handling
* **Strict Validation:** The parser thoroughly checks for line-length limits (max 80 chars), valid numeric ranges (e.g., 16-bit limits for `.dh`), illegal comma placements, and reserved-word violations.
* **Continuous Error Scanning:** If a syntax error is found, the assembler logs the exact line number and error type to `stderr` but continues scanning the file to report all remaining errors. Output files are safely aborted if any error is detected.
* **No Memory Leaks:** Dynamic memory allocation for linked lists (macros, symbols, and externals) is strictly managed and freed using unified `goto cleanup` blocks.

## Project Structure
* `src/` - Contains all `.c` source files.
* `include/` - Contains all `.h` header files and type definitions.
* `tests/` - Contains some valid and invalid `.as` tests.
* `obj/` - Generated directory for compiled object files.
* `Makefile` - Compilation rules.

## How to Build
To compile the project, navigate to the root directory and run:

```bash
make
```

This will compile the project using `gcc -Wall -ansi -pedantic` and generate an executable named `assembler`.

To clean the build artifacts and generated output files, run:

```bash
make clean
```

### Advanced Make Targets
For development and debugging, this project includes additional Makefile targets:
* `make debug`: Compiles the assembler with debugging symbols (`-g`) and disables optimization (`-O0`).
* `make valgrind FILES="<path/to/file>"`: Compiles the project in debug mode and executes it through Valgrind to perform a memory leak and bounds check.


## Usage
Run the assembler by passing one or more base file names (with or without the `.as` extension) as command-line arguments:

```bash
./assembler file1 file2
```

For each valid input file (e.g., `file1.as`), the assembler will generate:
* `file1.am`: The source code after macro expansion.
* `file1.ob`: The compiled machine code.
* `file1.ent`: (Optional) Entry symbols table.
* `file1.ext`: (Optional) External symbols table.
