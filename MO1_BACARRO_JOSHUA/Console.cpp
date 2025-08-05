#include "Console.h"
#include <iostream>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>
#include "MemoryManager.h"

extern MemoryManager* memory_manager;
extern int frame_size;

// Constructor
Console::Console(std::string n, int cLine, int tLines, int id)
    : name(n), currentLine(cLine), totalLines(tLines), processId(id) {

    std::time_t now = std::time(nullptr);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);
    char timeStr[64];
    std::strftime(timeStr, sizeof(timeStr), "%m/%d/%Y, %I:%M:%S %p", &timeInfo);
    timestamp = timeStr;
}

std::string Console::getName() const {
    return name;
}

void Console::drawProcess() {
    system("cls");
    std::cout << "Process: " << name << std::endl;
    std::cout << "ID: " << processId << std::endl;
    std::cout << "Current instruction line: " << currentLine << std::endl;
    std::cout << "Total line of instruction: " << totalLines << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
}

void Console::command_vmstat() {
    const auto& frame_table = memory_manager->get_frame_table();
    int used = 0;
    std::unordered_map<std::string, std::vector<int>> process_pages;

    for (size_t i = 0; i < frame_table.size(); ++i) {
        if (frame_table[i].used) {
            used++;
            process_pages[frame_table[i].owner_process].push_back(frame_table[i].virtual_page);
        }
    }

    int total = frame_table.size();
    int free = total - used;

    std::cout << "------ VM STATISTICS ------\n";
    std::cout << "Total Frames      : " << total << "\n";
    std::cout << "Used Frames       : " << used << "\n";
    std::cout << "Free Frames       : " << free << "\n";
    std::cout << "Frame Size (bytes): " << frame_size << "\n\n";

    std::cout << "Active Pages per Process:\n";
    for (auto it = process_pages.begin(); it != process_pages.end(); ++it) {
        const std::string& proc = it->first;
        const std::vector<int>& pages = it->second;

        std::cout << "  " << proc << ": ";
        for (int vp : pages) {
            std::cout << "[Page " << vp << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "---------------------------\n";
}
