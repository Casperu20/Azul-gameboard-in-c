# Azul Gameboard in C

A terminal-based implementation of the Azul board game written in C. This version supports **2 players**, simulates the full Azul gameplay loop, and features complete **tile drafting**, **pattern building**, **wall tiling**, and **scoring mechanics** — all with a color-coded terminal interface.

---

## Features:

- ✔️ Two-player mode
- ✔️ Factory and middle area tile drafting
- ✔️ Pattern line and wall placement validation
- ✔️ Floor line penalty system
- ✔️ Dynamic scoring (adjacency, rows, columns, full color sets)
- ✔️ End-game detection when a full wall row is completed
- ✔️ First player token handling for round order
- ✔️ Round transitions and randomized bag refill
- ✔️ Colorful terminal output (uses ANSI escape codes)

---

## How to Compile and Run:

```bash
gcc -Wall AZUL_FINAL.c -0 azul 
./azul
