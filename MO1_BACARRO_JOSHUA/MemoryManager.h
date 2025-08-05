#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>


struct FrameTableEntry {
    bool used;
    std::string owner_process;
    int virtual_page;
};

class MemoryManager {
private:
    int max_memory;
    int frame_size;
    int num_frames;
    std::vector<FrameTableEntry> frame_table;

    // Tracks data for each frame: [process][virtual_page] = frame content (uint16_t array)
    std::unordered_map<std::string, std::unordered_map<int, std::vector<uint16_t>>> frames;

public:
    MemoryManager(int max_mem, int frame_sz);

    int allocate_frame(const std::string& process_name, int virtual_page);
    void free_frame(int frame_index);
    int find_frame(const std::string& process_name, int virtual_page);
    int evict_and_allocate(const std::string& process_name, int virtual_page);

    const std::vector<FrameTableEntry>& get_frame_table() const;

    // Swap file support
    bool save_to_swap(const std::string& process_name, int page_number, const std::vector<uint16_t>& data);
    bool load_from_swap(const std::string& process_name, int page_number, std::vector<uint16_t>& data);

    // Optional: Access to actual frame contents if needed
    std::vector<uint16_t>& get_frame_data(const std::string& process_name, int virtual_page);
};
