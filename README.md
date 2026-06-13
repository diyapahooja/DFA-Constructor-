# DFA-Constructor-
Automatic Construction of DFA from Regular Expression

# Project Overview
This project implements a complete pipeline for converting regular expressions to minimized Deterministic Finite Automata (DFA). It's a compiler construction lab project that demonstrates lexical analysis concepts.

# Features
✅ Convert Regular Expression to NFA using Thompson's Construction
✅ Convert NFA to DFA using Subset Construction Method
✅ Minimize DFA using Table Filling Algorithm
✅ Display NFA and DFA transition tables
✅ Validate input strings against minimized DFA
✅ Handle epsilon (ε) transitions
✅ Support for operators: | (union), * (Kleene star), () (grouping)

# Algorithms Implemented
Thompson's Construction - Regex to NFA
Subset Construction - NFA to DFA
Table Filling Method - DFA Minimization
Shunting-Yard Algorithm - Infix to Postfix conversion

# Supported Regular Expressions
Letters: a to z
Digits: 0 to 9
Union operator: | (e.g., a|b)
Kleene star: * (e.g., a*)
Grouping: () (e.g., (a|b)*abb)
Concatenation: Implicit (e.g., ab)

# How to Compile and Run
Using Dev-C++:
Open Dev-C++
Create new project
Add source code
Compile (F9) and Run (F10)

# Submitted by:
Tehreem Zubair Diya Pahooja

# Course:
Compiler Construction Lab - Open Ended Project

# License:
MIT License -feel free to use, modify, and distribute

# Using GCC (Command Line):
gcc -o dfa_constructor main.c
./dfa_constructor


