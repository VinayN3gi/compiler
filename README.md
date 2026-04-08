# Pascal-Style Compiler Pipeline

A complete and integrated compiler pipeline for a custom Pascal-style programming language built entirely in C. It provides a full translation experience, including lexical analysis, syntax analysis, intermediate code generation, target code generation, and direct execution via an integrated Virtual Machine.

## Features
- Complete compilation pipeline (Lexer, Parser, ICG, Target Code Generation)
- Integrated Virtual Machine (VM) for end-to-end execution of intermediate code
- Panic-mode error recovery allowing multiple syntax errors to be reported in one run
- Precise line-number reporting for both lexical and syntactic errors
- Target code generation using a round-robin register pool (R0-R3)
- Support for integer arithmetic, relational expressions, if-else conditionals, while loops, and I/O operations

## Tech Stack
- Language: C
- Build System: GNU Make
- Compiler: GCC

## Setup Instructions
1. Clone the project

2. Build the project
   make

3. Prepare your input
   - Ensure you have an input.txt file in the root directory containing your Pascal-style source code.

4. Run the compiler
   ./compiler
   or
   make run

## License
MIT License

## Notes
- Ensure GCC and GNU Make are installed globally on your system.
- The compiler currently supports integer types only.
- Only the first 32 characters of an identifier are significant.
- The ICG is stored in a fixed-size array limited to 500 instructions.
