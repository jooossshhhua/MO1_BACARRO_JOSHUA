#pragma once
#include <string>
#include <vector>

enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR, READ, WRITE };

struct Instruction {
    InstructionType type;
    std::vector<std::string> args; // e.g., {"x", "1"} for DECLARE(x, 1)
    std::vector<Instruction> nestedInstructions; // For FOR loops
    int repeatCount = 0; // FOR only
};
#pragma once
