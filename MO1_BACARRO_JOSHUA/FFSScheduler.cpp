#include "FCFSScheduler.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream> 
#include <random>
#include "Config.h"
#include "Console.h"
#include "Instruction.h"
#include <algorithm>  // for clamp or fallback to min/max
#include <cctype>     // for isdigit
#include "MemoryManager.h"

extern int frame_size;
extern MemoryManager* memory_manager;
FCFS_Scheduler::FCFS_Scheduler(int cores) : num_cores(cores), running(true) {}

FCFS_Scheduler::~FCFS_Scheduler() {
    stop();
}

void FCFS_Scheduler::add_process(Process* proc) {
    std::lock_guard<std::mutex> lock(mtx);
    process_queue.push(proc);
    cv.notify_one();
    //std::cout << "Added process " << proc->name << " to the queue.\n";
}

void FCFS_Scheduler::start() {
    for (int i = 0; i < num_cores; ++i) {
        cpu_threads.emplace_back(&FCFS_Scheduler::cpu_worker, this, i);
    }
}

void FCFS_Scheduler::print_CPU_UTIL() {
    int numOfRunningProcess = running_processes.size();
    int cpuUtilization = static_cast<int>((static_cast<double>(numOfRunningProcess) / num_cores) * 100);

    std::cout << "CPU Utilization: " << cpuUtilization << "%\n";
    std::cout << "Cores Used: " << numOfRunningProcess << "\n";
    std::cout << "Cores Available: " << num_cores - numOfRunningProcess << "\n";
    std::cout << "----------------\n";
}


void FCFS_Scheduler::stop() {
    running = false;
    cv.notify_all();
    for (auto& thread : cpu_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    //std::cout << "Scheduler stopped.\n";
}
void FCFS_Scheduler::cpu_worker(int core_id) {
    // Set up the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> exec_dist(1, 10); // Random number of commands to execute per iteration

    while (running) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !process_queue.empty() || !running; });

            if (!running && process_queue.empty()) break;

            proc = process_queue.front();
            process_queue.pop();
            proc->core_id = core_id; // Assign core_id to the process
            proc->start_time = std::chrono::system_clock::now();
            running_processes.push_back(proc);
        }

        while (proc->executed_commands < proc->total_commands) {
            {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm local_tm;
                localtime_s(&local_tm, &now);
                // proc->log_file << "(" << std::put_time(&local_tm, "%m/%d/%Y %I:%M:%S%p") << ") Core:" << core_id << " \"Hello world from " << proc->name << "!\"\n";

                std::lock_guard<std::mutex> lock(mtx);
                if (proc->instructionPointer < proc->instructions.size()) {
                    Instruction& instr = proc->instructions[proc->instructionPointer];
                    execute_instruction(proc, instr); // Call your handler
                    proc->executed_commands++;
                    proc->instructionPointer++;
                }
            }

            // Sleep for a DURATION CHANGE THIS IN THE CONFIG DELAYS-PER-EXEC
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(Config::GetConfigParameters().delay_per_exec * 1000)));
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            running_processes.remove(proc);
            finished_processes.push_back(proc);



        }
    }
}

void FCFS_Scheduler::print_running_processes() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Running processes:\n";
    for (auto& proc : running_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Core: "
            << (proc->core_id == -1 ? "N/A" : std::to_string(proc->core_id))
            << " " << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}
void FCFS_Scheduler::print_finished_processes() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Finished processes:\n";
    for (auto& proc : finished_processes) {
        std::cout << proc->name << " (" << proc->get_start_time() << ") Finished "
            << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    std::cout << "----------------\n";
}
void FCFS_Scheduler::screen_ls() {
    print_CPU_UTIL();
    print_running_processes();
    print_finished_processes();
}
void FCFS_Scheduler::print_process_details(const std::string& process_name, int screen) {
    std::lock_guard<std::mutex> lock(mtx);


    // Check process_queue
    std::queue<Process*> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        Process* proc = temp_queue.front();
        temp_queue.pop();
        if (proc->name == process_name) {
            system("cls");
            proc->displayProcessInfo();
            proc->displayDetailedMemoryInfo();
            return;
        }
    }

    // Check running_processes 
    for (auto& proc : running_processes) {
        if (proc->name == process_name && screen == 0) {
            system("cls");
            proc->displayProcessInfo();
            proc->displayDetailedMemoryInfo();
            return;
        }
        else if (proc->name == process_name && screen == 1) {
            proc->displayProcessInfo();
            proc->displayDetailedMemoryInfo();
            return;
        }
    }

    // Check finished_processes
    for (auto& proc : finished_processes) {
        if (proc->name == process_name && screen == 1) {
            proc->displayProcessInfo();
            proc->displayDetailedMemoryInfo();
            std::cout << "Process " << process_name << " has finished and cannot be accessed after exiting this screen.\n";
            return;
        }
        else if (proc->name == process_name && screen == 0) {
            std::cout << "Process " << process_name << " not found.\n";
            return;
        }
    }

    // If process not found in any list
    std::cout << "Process " << process_name << " not found.\n";

}

void FCFS_Scheduler::print_process_queue_names() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Processes in the queue:\n";
    std::queue<Process*> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        std::cout << temp_queue.front()->name << "\n";
        temp_queue.pop();
    }
    std::cout << "----------------\n";
}

void FCFS_Scheduler::SetCpuCore(int cpu_core) {
    this->num_cores = cpu_core;
}

void FCFS_Scheduler::ReportUtil() {
    int numOfRunningProcess = running_processes.size();
    int cpuUtilization = static_cast<int>((static_cast<double>(numOfRunningProcess) / num_cores) * 100);

    std::vector<int> cores_used;
    int total_executed_commands = 0;
    int total_commands = 0;

    {
        std::lock_guard<std::mutex> lock(mtx);

        for (auto& proc : running_processes) {
            total_executed_commands += proc->executed_commands;
            total_commands += proc->total_commands;

            if (std::count(cores_used.begin(), cores_used.end(), proc->core_id) == 0) {
                cores_used.push_back(proc->core_id);
            }
        }

        for (auto& proc : finished_processes) {
            total_executed_commands += proc->executed_commands;
            total_commands += proc->total_commands;
        }
    }

    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream log("csopesy-log.txt", std::ios::app);
    log << "CPU Utilization: " << cpuUtilization << "%" << std::endl;
    log << "Cores Used: " << cores_used.size() << std::endl;
    log << "Cores Available: " << num_cores - cores_used.size() << std::endl;
    log << "----------------\n";
    log << "Running processes:\n";
    for (auto& proc : running_processes) {
        log << proc->name << " (" << proc->get_start_time() << ") Core: "
            << (proc->core_id == -1 ? "N/A" : std::to_string(proc->core_id))
            << " " << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    log << std::endl;
    log << "Finished processes:\n";
    for (auto& proc : finished_processes) {
        log << proc->name << " (" << proc->get_start_time() << ") Finished "
            << proc->executed_commands << " / " << proc->total_commands << "\n";
    }
    log << "----------------\n";
    log << std::endl;
    std::cout << "Report generated at /csopesy-log.txt" << std::endl;
}


bool FCFS_Scheduler::isValidProcessName(const std::string& process_name)
{
    std::lock_guard<std::mutex> lock(mtx);

    // Check process_queue
    std::queue<Process*> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        Process* proc = temp_queue.front();
        temp_queue.pop();
        if (proc->name == process_name) {
            return false;
        }
    }

    // Check running_processes
    for (auto& proc : running_processes) {
        if (proc->name == process_name) {
            return false;
        }
    }

    // Check finished_processes
    for (auto& proc : finished_processes) {
        if (proc->name == process_name) {
            return false;
        }
    }

    return true;
}
void FCFS_Scheduler::execute_instruction(Process* proc, const Instruction& instr) {
    auto& vars = proc->variables;

    if (instr.type == InstructionType::PRINT) {
        std::string msg = instr.args[0];
        for (const auto& pair : vars) {
            size_t pos = msg.find(pair.first);
            if (pos != std::string::npos) {
                msg.replace(pos, pair.first.length(), std::to_string(pair.second));
            }
        }

        proc->printLogs.push_back(msg);
    }
    else if (instr.type == InstructionType::DECLARE) {
        const std::string& var = instr.args[0];
        uint16_t val = static_cast<uint16_t>(std::stoi(instr.args[1]));
        vars[var] = val;
    }
    else if (instr.type == InstructionType::ADD || instr.type == InstructionType::SUBTRACT) {
        const std::string& dest = instr.args[0];
        uint16_t lhs = isdigit(instr.args[1][0]) ? std::stoi(instr.args[1]) : vars[instr.args[1]];
        uint16_t rhs = isdigit(instr.args[2][0]) ? std::stoi(instr.args[2]) : vars[instr.args[2]];
        int result = (instr.type == InstructionType::ADD) ? lhs + rhs : lhs - rhs;
        vars[dest] = static_cast<uint16_t>(std::max(0, std::min(result, 65535)));
    }
    else if (instr.type == InstructionType::SLEEP) {
        int ticks = std::stoi(instr.args[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(ticks * 1000));
    }
    else if (instr.type == InstructionType::FOR) {
        if (proc->loopStack.empty() || proc->loopStack.top().second > 0) {
            if (proc->loopStack.empty() || proc->loopStack.top().first != proc->instructionPointer) {
                proc->loopStack.push({ proc->instructionPointer, instr.repeatCount });
            }

            for (auto& nestedInstr : instr.nestedInstructions) {
                execute_instruction(proc, nestedInstr);
            }

            proc->loopStack.top().second--;
            if (proc->loopStack.top().second > 0) {
                // Repeat loop
                proc->instructionPointer--; // So it repeats again
            }
            else {
                proc->loopStack.pop(); // Done with loop
            }
        }
    }

    else if (instr.type == InstructionType::READ) {
        const std::string& var_name = instr.args[0];
        std::string addr_str = instr.args[1];

        try {
            uint32_t addr = std::stoul(addr_str, nullptr, 16);
            if (addr >= (uint32_t)proc->memory_size) {
                std::cout << "Process " << proc->name << " shut down due to memory access violation error at "
                    << proc->get_start_time() << ". " << addr_str << " invalid.\n";
                proc->executed_commands = proc->total_commands;
                return;
            }

            // ✅ Demand paging logic
            int virtual_page = addr / frame_size;
            if (proc->page_table.count(virtual_page) == 0) {
                int frame_index = memory_manager->allocate_frame(proc->name, virtual_page);
                if (frame_index == -1) {
                    frame_index = memory_manager->evict_and_allocate(proc->name, virtual_page);
                }
                if (frame_index == -1) {
                    std::cout << "Memory full. Cannot allocate frame for page " << virtual_page << "\n";
                    proc->executed_commands = proc->total_commands;
                    return;
                }
                proc->page_table[virtual_page] = frame_index;
            }

            proc->variables[var_name] = proc->memory_map.count(addr) ? proc->memory_map[addr] : 0;
        }
        catch (...) {
            std::cout << "READ instruction failed (invalid address).\n";
        }
    }

    else if (instr.type == InstructionType::WRITE) {
        std::string addr_str = instr.args[0];
        std::string val_str = instr.args[1];

        try {
            uint32_t addr = std::stoul(addr_str, nullptr, 16);
            if (addr >= (uint32_t)proc->memory_size) {
                std::cout << "Process " << proc->name << " shut down due to memory access violation error at "
                    << proc->get_start_time() << ". " << addr_str << " invalid.\n";
                proc->executed_commands = proc->total_commands;
                return;
            }

            // ✅ Demand paging logic
            int virtual_page = addr / frame_size;
            if (proc->page_table.count(virtual_page) == 0) {
                int frame_index = memory_manager->allocate_frame(proc->name, virtual_page);
                if (frame_index == -1) {
                    frame_index = memory_manager->evict_and_allocate(proc->name, virtual_page);
                }
                if (frame_index == -1) {
                    std::cout << "Memory full. Cannot allocate frame for page " << virtual_page << "\n";
                    proc->executed_commands = proc->total_commands;
                    return;
                }
                proc->page_table[virtual_page] = frame_index;
            }

            uint16_t value = 0;
            if (isdigit(val_str[0]) || (val_str[0] == '-' && isdigit(val_str[1]))) {
                value = static_cast<uint16_t>(std::stoi(val_str));
            }
            else {
                value = proc->variables.count(val_str) ? proc->variables[val_str] : 0;
            }

            proc->memory_map[addr] = std::max((uint16_t)0, std::min(value, (uint16_t)65535));
        }
        catch (...) {
            std::cout << "WRITE instruction failed (invalid address/value).\n";
        }
    }

}

