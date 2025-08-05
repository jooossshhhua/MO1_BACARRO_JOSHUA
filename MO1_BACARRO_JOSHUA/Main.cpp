#include<iostream>
#include <string>
#include "ConsoleManager.h";
#include "Commands.h"
#include "FCFSScheduler.h"
#include "Console.h"
#include "Config.h"
#include "RRScheduler.h"
#include <random>
#include "Instruction.h"
#include <vector>
#include <string>
#include <random>
#include <unordered_set>
#include "MemoryManager.h"

int frame_size;
MemoryManager* memory_manager = nullptr;
 // initialized with dummy values

bool isPowerOfTwo(int x) {
    return x >= 64 && x <= 65536 && (x & (x - 1)) == 0;
}
InstructionType get_instruction_type(const std::string& token) {
    if (token == "DECLARE") return InstructionType::DECLARE;
    if (token == "ADD") return InstructionType::ADD;
    if (token == "SUBTRACT") return InstructionType::SUBTRACT;
    if (token == "PRINT") return InstructionType::PRINT;
    if (token == "SLEEP") return InstructionType::SLEEP;
    if (token == "READ") return InstructionType::READ;
    if (token == "WRITE") return InstructionType::WRITE;
    return InstructionType::PRINT; // fallback
}
std::vector<Instruction> parse_instruction_string(const std::string& input) {
    std::vector<Instruction> instructions;
    std::stringstream ss(input);
    std::string segment;

    while (std::getline(ss, segment, ';')) {
        std::stringstream line(segment);
        std::string token;
        std::vector<std::string> tokens;

        while (line >> token) tokens.push_back(token);
        if (tokens.empty()) continue;

        Instruction instr;
        instr.type = get_instruction_type(tokens[0]);

        for (size_t i = 1; i < tokens.size(); ++i)
            instr.args.push_back(tokens[i]);

        instructions.push_back(instr);
    }

    return instructions;
}
/*

std::vector<Instruction> generate_dummy_instructions(int count) {
    std::vector<Instruction> instructions;
    std::vector<std::string> declared_vars;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> val_dist(0, 100);
    std::uniform_int_distribution<> type_dist(0, 3); // 0=DECLARE, 1=ADD, 2=SUBTRACT, 3=PRINT

    for (int i = 0; i < count; ++i) {
        int type = type_dist(gen);

        if (type == 0 || declared_vars.empty()) {
            // DECLARE new variable
            std::string var = "v" + std::to_string(declared_vars.size());
            std::string value = std::to_string(val_dist(gen));
            instructions.push_back({ InstructionType::DECLARE, { var, value } });
            declared_vars.push_back(var);
        }
        else if (type == 1 && declared_vars.size() >= 1) {
            // ADD
            std::string dest = "v" + std::to_string(declared_vars.size());
            std::string a = declared_vars[gen() % declared_vars.size()];
            std::string b = (gen() % 2 == 0) ? declared_vars[gen() % declared_vars.size()] : std::to_string(val_dist(gen));
            instructions.push_back({ InstructionType::ADD, { dest, a, b } });
            declared_vars.push_back(dest);
        }
        else if (type == 2 && declared_vars.size() >= 1) {
            // SUBTRACT
            std::string dest = "v" + std::to_string(declared_vars.size());
            std::string a = declared_vars[gen() % declared_vars.size()];
            std::string b = (gen() % 2 == 0) ? declared_vars[gen() % declared_vars.size()] : std::to_string(val_dist(gen));
            instructions.push_back({ InstructionType::SUBTRACT, { dest, a, b } });
            declared_vars.push_back(dest);
        }
        else if (type == 3 && !declared_vars.empty()) {
            // PRINT a declared variable
            std::string var = declared_vars[gen() % declared_vars.size()];
            instructions.push_back({ InstructionType::PRINT, { "Hello from " + var } });
        }
    }



    return instructions;
}
*/
/*
std::vector<Instruction> generate_dummy_instructions(int count) {
    std::vector<Instruction> instructions;

    // Declare x, y, z first — these are only done once
    instructions.push_back({ InstructionType::DECLARE, { "x", "0" } });
    instructions.push_back({ InstructionType::DECLARE, { "y", "0" } });
    instructions.push_back({ InstructionType::DECLARE, { "z", "0" } });

    // Fixed 6-instruction pattern per loop iteration
    std::vector<Instruction> pattern = {
        { InstructionType::ADD, { "x", "x", "1" } },
        { InstructionType::PRINT, { "VALUE FROM X: x" } },
        { InstructionType::ADD, { "y", "y", "1" } },
        { InstructionType::PRINT, { "VALUE FROM Y: y" } },
        { InstructionType::ADD, { "z", "z", "1" } },
        { InstructionType::PRINT, { "VALUE FROM Z: z" } }
    };

    int patternSize = pattern.size();
    int fullRepeats = count / patternSize;

    for (int i = 0; i < fullRepeats; ++i) {
        instructions.insert(instructions.end(), pattern.begin(), pattern.end());
    }

    // If count is not a multiple of 6, fill in the remaining
    int remaining = count % patternSize;
    for (int i = 0; i < remaining; ++i) {
        instructions.push_back(pattern[i]);
    }

    return instructions;
}
*/
std::vector<Instruction> generate_dummy_instructions(int count) {
    std::vector<Instruction> instructions;

    // Pre-declare x, y, z — always 3 instructions
    std::vector<Instruction> declarations = {
        { InstructionType::DECLARE, { "x", "0" } },
        { InstructionType::DECLARE, { "y", "0" } },
        { InstructionType::DECLARE, { "z", "0" } }
    };

    std::vector<Instruction> pattern = {
        { InstructionType::ADD, { "x", "x", "1" } },
        { InstructionType::PRINT, { "VALUE FROM X: x" } },
        { InstructionType::ADD, { "y", "y", "1" } },
        { InstructionType::PRINT, { "VALUE FROM Y: y" } },
        { InstructionType::ADD, { "z", "z", "1" } },
        { InstructionType::PRINT, { "VALUE FROM Z: z" } }
    };

    int patternSize = pattern.size();
    int remaining = count - (int)declarations.size();  // <-- subtract DECLAREs

    int fullRepeats = remaining / patternSize;
    int leftover = remaining % patternSize;

    // Add DECLAREs first
    instructions.insert(instructions.end(), declarations.begin(), declarations.end());

    // Then pattern
    for (int i = 0; i < fullRepeats; ++i) {
        instructions.insert(instructions.end(), pattern.begin(), pattern.end());
    }

    for (int i = 0; i < leftover; ++i) {
        instructions.push_back(pattern[i]);
    }
    instructions.push_back({ InstructionType::WRITE, { "0x1000", "x" } });
    instructions.push_back({ InstructionType::READ, { "temp", "0x1000" } });
    return instructions;
}

int main() {
    displayHeader();
    std::string command;

    ConsoleManager console_manager;
    bool running = true;

    // Initialize min and max process instructions

    // Initialize scheduler
    FCFS_Scheduler fcfs_scheduler(0);
    RR_Scheduler rr_scheduler(0, 0);


    // Initialize scheduling test
    std::thread scheduler_thread;
    int process_count = 0;
    bool scheduler_testing = false;
    std::string screen_process_name = "";
    bool initialized = false;
    while (running) {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        // Input tokens
        std::stringstream ss(command);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (command != "initialize" && command != "exit" && initialized == false) {
            if (Config::GetConfigParameters().num_cpu == 0 ||
                Config::GetConfigParameters().scheduler == "" ||
                Config::GetConfigParameters().quantum_cycles == 0 ||
                Config::GetConfigParameters().batch_process_freq == 0 ||
                Config::GetConfigParameters().min_ins == 0 ||
                Config::GetConfigParameters().max_ins == 0 ||
                Config::GetConfigParameters().delay_per_exec == 0 ||
                Config::GetConfigParameters().max_overall_mem == 0 ||
                Config::GetConfigParameters().mem_per_frame == 0 ||
                Config::GetConfigParameters().min_mem_per_proc == 0 ||
                Config::GetConfigParameters().max_mem_per_proc == 0) {



                std::cout << "Initialize the program with command \"initialize\"" << std::endl;
            }

        }

        std::vector<std::shared_ptr<Console>> consoles = console_manager.getConsoles();

        // Current console is not the main menu and the exit command is entered.
        if (command == "exit" && (console_manager.getCurrentConsoleName() != "MAIN_MENU")) {
            system("cls");
            // Find the console with the name "MAIN_MENU"
            for (const auto& consolePtr : consoles) {
                if (consolePtr->getName() == "MAIN_MENU") {
                    console_manager.setCurrentConsole(consolePtr);
                    break;
                }
            }
            // If "MAIN_MENU" console is found, draw its main content
            if (console_manager.getCurrentConsole() && console_manager.getCurrentConsoleName() == "MAIN_MENU") {
                displayHeader();
            }
        }

        // Current console is the main menu and the exit command is entered.
        else if (command == "exit" && console_manager.getCurrentConsoleName() == "MAIN_MENU") {
            return false;
        }

        // Current console is the main menu and the screen -s (create process) is entered.
        else if (tokens.size() == 4 && tokens[0] == "screen" && tokens[1] == "-s") {
            std::string pname = tokens[2];
            int mem_size;

            try {
                mem_size = std::stoi(tokens[3]);
            }
            catch (...) {
                std::cout << "invalid memory allocation" << std::endl;
                continue;
            }

            if (!isPowerOfTwo(mem_size)) {
                std::cout << "invalid memory allocation" << std::endl;
                continue;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(Config::GetConfigParameters().min_ins, Config::GetConfigParameters().max_ins);

            int commands_per_process = dist(gen);

            if (Config::GetConfigParameters().scheduler == "fcfs") {
                if (!fcfs_scheduler.isValidProcessName(pname)) {
                    std::cout << "Process with name \"" + pname + "\" already exists" << std::endl;
                }
                else {
                    Process* new_process = new Process(pname, commands_per_process, mem_size);
                    new_process->instructions = generate_dummy_instructions(commands_per_process);
                    new_process->total_commands = new_process->instructions.size();

                    fcfs_scheduler.add_process(new_process);

                    std::shared_ptr<Console> new_console(new Console(
                        new_process->name, new_process->executed_commands,
                        new_process->total_commands, new_process->process_id));
                    screen_process_name = pname;
                    console_manager.setCurrentConsole(new_console);
                    fcfs_scheduler.print_process_details(pname, 0);
                }
            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                if (!rr_scheduler.isValidProcessName(pname)) {
                    std::cout << "Process with name \"" + pname + "\" already exists" << std::endl;
                }
                else {
                    Process* new_process = new Process(pname, commands_per_process, mem_size);
                    new_process->instructions = generate_dummy_instructions(commands_per_process);
                    new_process->total_commands = new_process->instructions.size();

                    rr_scheduler.add_process(new_process);

                    std::shared_ptr<Console> new_console(new Console(
                        new_process->name, new_process->executed_commands,
                        new_process->total_commands, new_process->process_id));
                    screen_process_name = pname;
                    console_manager.setCurrentConsole(new_console);
                    rr_scheduler.print_process_details(pname, 0);
                }
            }
        }
        else if (tokens.size() >= 5 && tokens[0] == "screen" && tokens[1] == "-c") {
            std::string pname = tokens[2];
            int mem_size;
            std::string instr_string = command.substr(command.find(tokens[4]));

            try {
                mem_size = std::stoi(tokens[3]);
            }
            catch (...) {
                std::cout << "invalid memory allocation" << std::endl;
                continue;
            }

            if (!isPowerOfTwo(mem_size)) {
                std::cout << "invalid memory allocation" << std::endl;
                continue;
            }

            std::vector<Instruction> parsed_instr = parse_instruction_string(instr_string);
            if (parsed_instr.empty() || parsed_instr.size() > 50) {
                std::cout << "invalid command" << std::endl;
                continue;
            }

            if (Config::GetConfigParameters().scheduler == "fcfs") {
                if (!fcfs_scheduler.isValidProcessName(pname)) {
                    std::cout << "Process with name \"" + pname + "\" already exists" << std::endl;
                }
                else {
                    Process* new_proc = new Process(pname, parsed_instr.size(), mem_size);
                    new_proc->instructions = parsed_instr;
                    new_proc->total_commands = parsed_instr.size();

                    fcfs_scheduler.add_process(new_proc);
                    std::shared_ptr<Console> new_console(new Console(pname, 0, new_proc->total_commands, new_proc->process_id));
                    screen_process_name = pname;
                    console_manager.setCurrentConsole(new_console);
                    fcfs_scheduler.print_process_details(pname, 0);
                }
            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                if (!rr_scheduler.isValidProcessName(pname)) {
                    std::cout << "Process with name \"" + pname + "\" already exists" << std::endl;
                }
                else {
                    Process* new_proc = new Process(pname, parsed_instr.size(), mem_size);
                    new_proc->instructions = parsed_instr;
                    new_proc->total_commands = parsed_instr.size();

                    rr_scheduler.add_process(new_proc);
                    std::shared_ptr<Console> new_console(new Console(pname, 0, new_proc->total_commands, new_proc->process_id));
                    screen_process_name = pname;
                    console_manager.setCurrentConsole(new_console);
                    rr_scheduler.print_process_details(pname, 0);
                }
            }
        }


        else if (command == "show") {
            console_manager.drawAllConsoles();
        }

        // The current console is not the main menu and the screen -s command is entered.
        else if (tokens.size() == 3 && tokens[0] == "screen" && tokens[1] == "-s" && tokens[2] != "" && console_manager.getCurrentConsoleName() != "MAIN_MENU") {

            std::cout << "Unkown Command" << std::endl;
        }

        // The current ocnsole is not the main menu and the screen -r command is entered.
        else if (tokens.size() == 3 && tokens[0] == "screen" && tokens[1] == "-r" && tokens[2] != "" && console_manager.getCurrentConsoleName() != "MAIN_MENU") {

            std::cout << "Unkown Command" << std::endl;
        }
        else if (command == "clear") {
            clearScreen();
        }

        else if (console_manager.getCurrentConsoleName() != "MAIN_MENU" && command == "process-smi") {
            if (Config::GetConfigParameters().scheduler == "fcfs") {
                fcfs_scheduler.print_process_details(screen_process_name, 1);

            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                rr_scheduler.print_process_details(screen_process_name, 1);
            }


        }

        else if (command == "initialize") {
            initialized = true;
            Config::Initialize();
            frame_size = Config::GetConfigParameters().mem_per_frame;
        
            memory_manager = new MemoryManager(
                Config::GetConfigParameters().max_overall_mem,
                frame_size
            );
            std::cout << "Config initialized with \"" << "config.txt\" parameters" << std::endl;

            if (Config::GetConfigParameters().scheduler == "fcfs") {
                fcfs_scheduler.SetCpuCore(Config::GetConfigParameters().num_cpu);
                fcfs_scheduler.start();
            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                rr_scheduler.SetCpuCore(Config::GetConfigParameters().num_cpu);
                rr_scheduler.SetQuantum(Config::GetConfigParameters().quantum_cycles);
                rr_scheduler.start();
            }
        }

        // "scheduler-stop"
        else if (command == "scheduler-stop") {
            if (scheduler_testing) {
                scheduler_testing = false;
                if (scheduler_thread.joinable()) {
                    scheduler_thread.join();
                }

                std::cout << "Scheduler test execution stopped.\n";
            }
            else {
                std::cout << "Scheduler test is not currently running.\n";
            }
        }
        //BALIKAN TO PLEASE LANG MAY ERROR SA PROCESS
        // "scheduler-start"
        else if (command == "scheduler-start") {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(Config::GetConfigParameters().min_ins, Config::GetConfigParameters().max_ins);

            if (!scheduler_testing) {
                scheduler_testing = true;
                scheduler_thread = std::thread([&]() {
                    while (scheduler_testing) {
                        int commands_per_process = dist(gen);
                        int mem_size = 1024;
                        if (Config::GetConfigParameters().scheduler == "fcfs") {
                            //fcfs_scheduler.add_process(new Process("process" + std::to_string(++process_count), commands_per_process));
                            Process* p = new Process("process" + std::to_string(++process_count), commands_per_process, mem_size);
                            p->instructions = generate_dummy_instructions(commands_per_process);
                            p->total_commands = p->instructions.size();
                            fcfs_scheduler.add_process(p);

                        }

                        if (Config::GetConfigParameters().scheduler == "rr") {
                            Process* p = new Process("process" + std::to_string(++process_count), commands_per_process, mem_size);
                            p->instructions = generate_dummy_instructions(commands_per_process);
                            p->total_commands = p->instructions.size();
                            rr_scheduler.add_process(p);
                        }



                        std::this_thread::sleep_for(std::chrono::milliseconds((int)(Config::GetConfigParameters().batch_process_freq * 1000)));

                    }
                    });
                scheduler_thread.detach();

                std::cout << "Scheduler test execution started.\n";
            }
            else {
                std::cout << "Scheduler test is already running.\n";
            }
        }
        

        // "screen -ls"
        else if (tokens[0] == "screen" && tokens[1] == "-ls") {
            if (Config::GetConfigParameters().scheduler == "fcfs") {
                fcfs_scheduler.screen_ls();
            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                rr_scheduler.screen_ls();
            }


        }

        // "screen -r"
        else if (tokens.size() == 3 && tokens[0] == "screen" && tokens[1] == "-r") {
            std::cout << "screen -r " << tokens[2];
            if (console_manager.getCurrentConsoleName() == "MAIN_MENU") {
                if (Config::GetConfigParameters().scheduler == "fcfs") {
                    fcfs_scheduler.print_process_details(tokens[2], 0);
                }

                if (Config::GetConfigParameters().scheduler == "rr") {
                    rr_scheduler.print_process_details(tokens[2], 0);
                }

            }
            else {
                if (Config::GetConfigParameters().scheduler == "fcfs") {
                    fcfs_scheduler.print_process_details(tokens[2], 1);
                }

                if (Config::GetConfigParameters().scheduler == "rr") {
                    rr_scheduler.print_process_details(tokens[2], 1);
                }


            }

            screen_process_name = tokens[2];
            std::shared_ptr<Console> new_console(new Console("VIEW_SCREEN", 0, 0, 0));
            console_manager.setCurrentConsole(new_console);
        }
        else if (tokens.size() == 1 && tokens[0] == "vmstat") {
            Console vmstatConsole("vmstat", 0, 0, -1);  // dummy console object
            vmstatConsole.command_vmstat();            // calls actual memory manager
        }

        else if (command == "report-util") {
            if (Config::GetConfigParameters().scheduler == "fcfs") {
                fcfs_scheduler.ReportUtil();
            }

            if (Config::GetConfigParameters().scheduler == "rr") {
                rr_scheduler.ReportUtil();
            }

        }

        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    return 0;
}
