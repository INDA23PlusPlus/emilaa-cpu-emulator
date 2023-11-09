#include "emulator.hpp"

#include <fstream>
#include <sstream>
#include <regex>

#include <cstdio>

namespace emu {

i32 Emulator::load_instructions(const std::string &path) {
    // Read file with instructions.
    std::ifstream f{path, std::ios::binary | std::ios::ate};
    if(!f.is_open()) {
        printf("Failed to open file: [%s]\n", path.c_str());
        return FAILED_TO_OPEN_FILE;
    }

    size_t file_size = static_cast<size_t>(f.tellg());
    std::vector<char> bytes(file_size);
    f.seekg(0, std::ios::beg);
    f.read(bytes.data(), file_size);
    f.close();
    std::string content(bytes.data(), file_size);

    // Format and load.
    content = std::regex_replace(content, std::regex(R"(\n)"), " ");
    content = std::regex_replace(content, std::regex(R"( {2,})"), " ");

    std::stringstream stream(content);
    std::string i;
    u64 counter = 0;
    b8 cmp_last = false;
    while(std::getline(stream, i, ' ')) {
        Instruction tmp{i, NA};
        if(determine(tmp, cmp_last)) {
            if(!cmp_last) {
                jmp_positions.emplace(i, counter);
            }
            cmp_last = false;
        }
        if(tmp.type > 9) { return UNKNOWN_INSTRUCTION; } 
        instruction_list.push_back(tmp);
        counter++;
    }
    instruction_list.shrink_to_fit();

    return SUCCESS;
}

i32 Emulator::execute() {
    u64 end = instruction_list.size();
    internal_state = EXPECTING_INSTRUCTION;

    for(u64 i = 0; i < end; i++) {
        switch (instruction_list[i].type) {
            case MOV: {
                internal_state = EXPECTING_TWO_ARGS;
                if(instruction_list[i+1].type != REG ||
                   (instruction_list[i+2].type != REG &&
                   instruction_list[i+2].type != HEX))
                    return UNEXPECTED_INSTRUCTION;

                const auto d_str = instruction_list[i+1].inst.substr(1);
                const auto s_str = instruction_list[i+2].inst.substr(1);

                u32 dst = std::stoi(d_str, 0, 16);
                if(dst > 7) { return REGISTER_DOES_NOT_EXIST; }
                
                u32 src = 0;
                if(instruction_list[i+2].type == REG) {
                    src = std::stoi(s_str, 0, 16);
                    if(src > 7) { return REGISTER_DOES_NOT_EXIST; }
                    reg_32[dst] = reg_32[src];
                } else {
                    reg_32[dst] = std::stoi(s_str, 0, 16);
                }

                i += 2;
                break;
            }

            case ADD: {
                internal_state = EXPECTING_TWO_ARGS;
                if(instruction_list[i+1].type != REG ||
                   (instruction_list[i+2].type != REG &&
                   instruction_list[i+2].type != HEX))
                    return UNEXPECTED_INSTRUCTION;

                const auto d_str = instruction_list[i+1].inst.substr(1);
                const auto s_str = instruction_list[i+2].inst.substr(1);

                u32 dst = std::stoi(d_str, 0, 16);
                if(dst > 7) { return REGISTER_DOES_NOT_EXIST; }
                
                u32 src = 0;
                if(instruction_list[i+2].type == REG) {
                    src = std::stoi(s_str, 0, 16);
                    if(src > 7) { return REGISTER_DOES_NOT_EXIST; }
                    reg_32[dst] += reg_32[src];
                } else {
                    reg_32[dst] += std::stoi(s_str, 0, 16);
                }

                i += 2;
                break;
            }

            case MUL: {
                internal_state = EXPECTING_TWO_ARGS;
                if(instruction_list[i+1].type != REG ||
                   (instruction_list[i+2].type != REG &&
                   instruction_list[i+2].type != HEX))
                    return UNEXPECTED_INSTRUCTION;

                const auto d_str = instruction_list[i+1].inst.substr(1);
                const auto s_str = instruction_list[i+2].inst.substr(1);

                u32 dst = std::stoi(d_str, 0, 16);
                if(dst > 7) { return REGISTER_DOES_NOT_EXIST; }
                
                u32 src = 0;
                if(instruction_list[i+2].type == REG) {
                    src = std::stoi(s_str, 0, 16);
                    if(src > 7) { return REGISTER_DOES_NOT_EXIST; }
                    reg_32[dst] *= reg_32[src];
                } else {
                    reg_32[dst] *= std::stoi(s_str, 0, 16);
                }

                i += 2;
                break;
            }

            case CLT: {
                internal_state = EXPECTING_TWO_ARGS;
                if((instruction_list[i+1].type != REG  &&
                    instruction_list[i+1].type != HEX) ||
                   (instruction_list[i+2].type != REG  &&
                    instruction_list[i+2].type != HEX))
                    return UNEXPECTED_INSTRUCTION;

                const auto a_str = instruction_list[i+1].inst.substr(1);
                const auto b_str = instruction_list[i+2].inst.substr(1);

                i32 A = 0, B = 0;
                if(instruction_list[i+1].type == REG) {
                    u32 u = std::stoi(a_str, 0, 16);
                    if(u > 7) { return REGISTER_DOES_NOT_EXIST; }
                    A = reg_32[u];
                } else {
                    A = std::stoi(a_str, 0, 16);
                }

                if(instruction_list[i+2].type == REG) {
                    u32 u = std::stoi(b_str, 0, 16);
                    if(u > 7) { return REGISTER_DOES_NOT_EXIST; }
                    B = reg_32[u];
                } else {
                    B = std::stoi(b_str, 0, 16);
                }

                cmp_state = (A < B) ? TRUE : FALSE;

                i += 2;
                break;
            }

            case JEQ: {
                internal_state = EXPECTING_ONE_ARG;
                if(instruction_list[i+1].type != LBL)
                    return UNEXPECTED_INSTRUCTION;

                if(cmp_state == TRUE) {
                    const auto label = instruction_list[i+1].inst;
                    i = jmp_positions.find(label)->second;
                }

                cmp_state = INDETERMINATE;
                break;
            }

            case JNE: {
                internal_state = EXPECTING_ONE_ARG;
                if(instruction_list[i+1].type != LBL)
                    return UNEXPECTED_INSTRUCTION;

                if(cmp_state == FALSE) {
                    const auto label = instruction_list[i+1].inst;
                    i = jmp_positions.find(label)->second;
                }

                cmp_state = INDETERMINATE;
                break;
            }

            case OUT: {
                internal_state = EXPECTING_ONE_ARG;
                if(instruction_list[i+1].type != REG &&
                   instruction_list[i+1].type != HEX)
                    return UNEXPECTED_INSTRUCTION;

                const auto inst = instruction_list[i+1].inst.substr(1);

                if(instruction_list[i+1].type == REG) {
                    u32 src = std::stoi(inst, 0, 16);
                    if(src > 7) { return REGISTER_DOES_NOT_EXIST; }
                    printf("%d\n", reg_32[src]);
                } else {
                    printf("%d\n", std::stoi(inst, 0, 16));
                }

                i += 1;
                break;
            }

            case LBL: {
                break;
            }

            default: {
                return UNKNOWN_ERROR;
                break;
            }
        }
    }

    return SUCCESS;
}

b8 Emulator::determine(Instruction &i, b8 &cmp_last) {
    if(i.inst[0] == 'A' || i.inst[0] == 'a') {
        i.type = LBL;
        return true;
    }
    
    try {
        u32 inst = std::stoi(i.inst, 0, 16);
        i.type = InstructionType(inst - 16);
    } catch(...) {}

    if     (i.inst[0] == 'R' || i.inst[0] == 'r')   { i.type = REG; }
    else if(i.inst[0] == 'C' || i.inst[0] == 'c')   { i.type = HEX; }

    cmp_last = (i.type == JNE || i.type == JEQ);

    return false;
}

}