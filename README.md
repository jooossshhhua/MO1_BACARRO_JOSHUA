# MO1_BACARRO_JOSHUA

Developer: Joshua Bacarro  
Subject: CSOPESY  

This project is a **command-line CPU process scheduler simulation** that supports two scheduling algorithms: **First-Come-First-Serve (FCFS)** and **Round-Robin (RR)**. Users can interact with the simulation through a custom console UI and execute commands to create, manage, and monitor simulated processes.

---

## Features

- **Command Interpreter** — recognizes commands like `initialize`, `screen -s`, `scheduler-test`, etc.
- **Console UI** — displays each process with ID, status, declared variables, and execution logs.
- **Process Simulation** — supports dummy instructions such as `DECLARE`, `ADD`, `SUBTRACT`, and `PRINT`.
- **Round-Robin & FCFS Scheduling** — simulates multi-core CPU scheduling with quantum control.
- **Logging** — outputs CPU utilization and process execution summary in `csopesy-log.txt`.

---

## Commands Overview

| Command           | Description |
|------------------|-------------|
| `initialize`      | Loads configuration from `config.txt` |
| `screen -s NAME`  | Creates a new process named `NAME` |
| `screen -r NAME`  | Views the process screen for `NAME` |
| `scheduler-test`  | Starts automatic process creation |
| `scheduler-stop`  | Stops test process generation |
| `screen -ls`      | Lists all active and finished processes |
| `report-util`     | Logs CPU and process statistics |
| `process-smi`     | Shows live process status |

---

## How to Compile and Run

1. Make sure you have a C++ compiler installed (e.g., `g++`).
2. Open a terminal and navigate to the folder containing the source code.
3. Compile the program using the following command:

