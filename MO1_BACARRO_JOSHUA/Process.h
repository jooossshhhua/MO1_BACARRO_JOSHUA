#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stack>
#include <unordered_map>
#include "Instruction.h"
class Process {
private:

    static int next_process_id;

public:

    std::string name;
    int total_commands;
    int executed_commands;
    std::ofstream log_file;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    int core_id;
    int process_id;

    // NEW for instruction execution:
    std::unordered_map<std::string, uint16_t> variables;
    std::vector<std::string> printLogs;
    std::vector<Instruction> instructions;
    size_t instructionPointer = 0;
    std::stack<std::pair<size_t, int>> loopStack;




    Process(const std::string& pname, int commands);
    ~Process();
    std::string get_start_time() const;
    std::string get_status() const;

    void displayProcessInfo() const;
};
