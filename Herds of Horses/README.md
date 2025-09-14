# 🐎 Wild Horse Herd Tracker

This project implements a dynamic data structure system in C++ for managing wild horse herds, developed as part of the Technion course **Data Structures (Winter 2024–2025)**.

## 📖 Overview

Simulates a system for tracking herds and horses with operations including:

- Adding/removing herds and horses
- Joining and leaving herds
- Following relationships between horses
- Checking herd leadership and run-together conditions
- Querying horse speed and leadership chains

Supports efficient memory management and adheres to strict time and space complexity constraints.

## 🛠️ Tech Stack

- **C++11** (compiled with `g++ -std=c++11 -DNDEBUG -Wall`)
- Object-oriented design
- Manual memory management (no STL)
- Custom data structures (no external libraries)

## ✅ Testing

The project includes input/output test files and a Python script `run_tests.py` for automated validation using:

```bash
python3 run_tests.py
```
```bash
You can compare your program’s output with expected results using:
diff --strip-trailing-cr -B -Z test1.out test1.expected
```
