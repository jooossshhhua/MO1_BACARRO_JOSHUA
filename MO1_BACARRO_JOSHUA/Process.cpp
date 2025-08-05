#include "process.h"
#include <iomanip>
#include <ctime>
#include <sstream>
#include <iostream>
#include "Instruction.h"
int Process::next_process_id = 1;
Process::Process(const std::string& pname, int commands, int mem_size)
    : name(pname), total_commands(commands), executed_commands(0),
    core_id(-1), process_id(next_process_id++), memory_size(mem_size) {
    start_time = std::chrono::system_clock::now();
}

Process::~Process() {
    log_file.close();
}

std::string Process::get_start_time() const {
    auto now = std::chrono::system_clock::to_time_t(start_time);
    std::tm local_tm;
    localtime_s(&local_tm, &now);
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p");
    return oss.str();
}

std::string Process::get_status() const {
    std::ostringstream oss;
    if (executed_commands == total_commands) {
        oss << "Finished " << total_commands << "/" << total_commands;
    }
    else {
        oss << "Core: " << (core_id == -1 ? "N/A" : std::to_string(core_id)) << " " << executed_commands << " / " << total_commands;
    }
    return oss.str();
}
void Process::displayProcessInfo() const {
    //print the process information

    std::cout << "Process name: " << name << "\n";
    std::cout << "Process ID: " << process_id << "\n\n";

    /*  if (executed_commands < total_commands) {
          std::cout << "Current Instruction Line: " << executed_commands << "\n";
          std::cout << "Lines of Code: " << total_commands << "\n";

      }
      else {
          std::cout << "Finished!\n";
      }*/
    if (executed_commands < total_commands) {
        std::cout << "Current Instruction Line: " << executed_commands << "\n";
        std::cout << "Lines of Code: " << total_commands << "\n\n";
    }
    else {
        std::cout << "Finished!\n\n";
    }

    std::cout << "PRINT Logs:\n";
    for (const auto& log : printLogs) {
        std::cout << "  " << log << "\n";
    }

    std::cout << "\nVariables:\n";
    for (const auto& pair : variables) {
        std::cout << "  " << pair.first << " = " << pair.second << "\n";
    }



}
void Process::displayDetailedMemoryInfo() const {
    std::cout << "\n----- Variable Table -----\n";
    for (const auto& pair : variables) {
        std::cout << "  " << pair.first << " = " << pair.second << "\n";
    }

    std::cout << "\n----- Memory Map -----\n";
    for (const auto& pair : memory_map) {
        std::cout << "  0x" << std::hex << pair.first << " = " << std::dec << pair.second << "\n";
    }

    std::cout << "\n----- Page Table -----\n";
    for (const auto& entry : page_table) {
        int vp = entry.first;
        int frame = entry.second;
        std::cout << "  Virtual Page " << vp << " → Frame " << frame << "\n";
    }

    std::cout << "-------------------------\n";
}

