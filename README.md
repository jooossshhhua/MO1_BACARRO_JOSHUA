# MO1_BACARRO_JOSHUA

This project is a **command-line CPU process scheduler simulation** for the subject CSOPESY. It supports two scheduling algorithms: **First-Come-First-Serve (FCFS)** and **Round-Robin (RR)**. Users can interact with the simulation through a custom console UI and execute commands to create, manage, and monitor simulated processes.

---

##  Features

- **Command Interpreter** — recognizes commands like `initialize`, `screen -s`, `scheduler-test`, etc.
- **Console UI** — processes are shown with details such as ID, status, variables, and logs.
- **Process Simulation** — supports execution of dummy instructions like `DECLARE`, `ADD`, `SUBTRACT`, and `PRINT`.
- **Round-Robin & FCFS Scheduling** — multi-threaded execution with CPU core management and quantum timing.
- **Logging** — generates a report in `csopesy-log.txt` showing CPU utilization, core usage, and process stats.

---

## Commands Overview

| Command           | Description |
|------------------|-------------|
| `initialize`      | Loads configuration from `config.txt` |
| `screen -s NAME`  | Creates a new process with name `NAME` |
| `screen -r NAME`  | Views a process screen by name |
| `scheduler-test`  | Starts auto-creating processes |
| `scheduler-stop`  | Stops test process generation |
| `screen -ls`      | Lists running and finished processes |
| `report-util`     | Writes CPU & process stats to a log file |
| `process-smi`     | Shows live process status (from screen view) |

---

##  Example `config.txt`

