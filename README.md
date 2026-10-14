# IZLO Project 1 – SAT Solver Reduction

First project for the **IZLO (Logical Foundations of Computer Science)** course, written in C.

## Overview
A C program that encodes a combinatorial problem into propositional logic in Conjunctive Normal Form (CNF). Outputs clauses in the standard **DIMACS CNF** format to be solved by a SAT solver (e.g., MiniSat, CaDiCaL).

## Build & Usage
```bash
make
./izlo1 <input_file> > formula.cnf
minisat formula.cnf
```
