<H1> COP3530 - Project 2 </h1>

## Overview
This project compares the search performance of two manually implemented data structures. 
- B+ Tree
- Hash Map

Both are used to store and query Pokemon data from 2 CSV datasets:
- "Pokemon.csv" -> Base Pokemon data (unique species, forms, and evolutions)
- "Pokemon_100k_builds_competitive_moves.csv" -> 100k randomized Pokemon builds (with movesets, items, and natures)

The program allows the user to select which data structure to use and how to search (by Pokemon ID, name, or type).
It then measures and prints the time taken to perform the search for both data structures.

## How the Program Works
