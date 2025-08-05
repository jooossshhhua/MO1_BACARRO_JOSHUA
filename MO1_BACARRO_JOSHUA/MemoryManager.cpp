#include "MemoryManager.h"
#include <fstream>
#include <sys/stat.h>
#include <direct.h>
#include<iostream>
void create_swap_folder() {
#ifdef _WIN32
    _mkdir("swap");  // Windows
#else
    mkdir("swap", 0777);  // Linux/macOS
#endif
}

MemoryManager::MemoryManager(int max_mem, int frame_sz)
    : max_memory(max_mem), frame_size(frame_sz) {
    if (frame_size == 0) {
        throw std::runtime_error("MemoryManager: frame_size cannot be zero");
    }

    num_frames = max_memory / frame_size;
    frame_table.resize(num_frames, { false, "", -1 });
}

int MemoryManager::allocate_frame(const std::string& process_name, int virtual_page) {
    for (int i = 0; i < num_frames; ++i) {
        if (frame_size == 0) {
            std::cout << "[ERROR] Tried to allocate frame with frame_size = 0\n";
            exit(1);
        }
        if (!frame_table[i].used) {
            frame_table[i].used = true;
            frame_table[i].owner_process = process_name;
            frame_table[i].virtual_page = virtual_page;

            std::vector<uint16_t> data;
            if (!load_from_swap(process_name, virtual_page, data)) {
                data = std::vector<uint16_t>(frame_size, 0); // fresh page
            }
            frames[process_name][virtual_page] = data;

            return i;
        }
    }
    return -1; // no free frame
}


void MemoryManager::free_frame(int frame_index) {
    if (frame_index >= 0 && frame_index < num_frames) {
        std::string proc = frame_table[frame_index].owner_process;
        int page = frame_table[frame_index].virtual_page;

        // ✅ Only save to swap if it exists
        if (frames.count(proc) && frames[proc].count(page)) {
            save_to_swap(proc, page, frames[proc][page]);
            frames[proc].erase(page);
        }

        frame_table[frame_index] = { false, "", -1 };
    }
}


int MemoryManager::find_frame(const std::string& process_name, int virtual_page) {
    for (int i = 0; i < num_frames; ++i) {
        if (frame_table[i].used &&
            frame_table[i].owner_process == process_name &&
            frame_table[i].virtual_page == virtual_page) {
            return i;
        }
    }
    return -1;
}

int MemoryManager::evict_and_allocate(const std::string& process_name, int virtual_page) {
    for (int i = 0; i < num_frames; ++i) {
        if (frame_table[i].used) {
            std::string evicted_proc = frame_table[i].owner_process;
            int evicted_page = frame_table[i].virtual_page;

            // ✅ Only save if exists
            if (frames.count(evicted_proc) && frames[evicted_proc].count(evicted_page)) {
                save_to_swap(evicted_proc, evicted_page, frames[evicted_proc][evicted_page]);
                frames[evicted_proc].erase(evicted_page);
            }

            frame_table[i] = { true, process_name, virtual_page };

            std::vector<uint16_t> data;
            if (!load_from_swap(process_name, virtual_page, data)) {
                data = std::vector<uint16_t>(frame_size, 0);
            }
            frames[process_name][virtual_page] = data;

            return i;
        }
    }
    return -1;
}


bool MemoryManager::save_to_swap(const std::string& process_name, int page_number, const std::vector<uint16_t>& data) {
    create_swap_folder();

    if (data.empty()) return false; // ✅ Prevent writing an empty vector

    std::ofstream out("swap/" + process_name + "_page" + std::to_string(page_number) + ".bin", std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(uint16_t));
    return true;
}

bool MemoryManager::load_from_swap(const std::string& process_name, int page_number, std::vector<uint16_t>& data) {
    std::ifstream in("swap/" + process_name + "_page" + std::to_string(page_number) + ".bin", std::ios::binary);
    if (!in) return false;

    data.resize(frame_size); // ✅ make sure it's sized before writing into it
    in.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(uint16_t));

    // ✅ check if read was successful
    return in.gcount() == (std::streamsize)(frame_size * sizeof(uint16_t));
}


const std::vector<FrameTableEntry>& MemoryManager::get_frame_table() const {
    return frame_table;
}

std::vector<uint16_t>& MemoryManager::get_frame_data(const std::string& process_name, int virtual_page) {
    return frames[process_name][virtual_page];
}
