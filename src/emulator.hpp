#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "defines.hpp"

enum ErrorType {
    UNKNOWN_ERROR = -1,
    SUCCESS,
    NO_FILE_PROVIDED,
    FAILED_TO_OPEN_FILE,
    UNKNOWN_INSTRUCTION,
    UNEXPECTED_INSTRUCTION,
    REGISTER_DOES_NOT_EXIST
};

namespace emu {
    class Emulator {
        public:
            i32 load_instructions(const std::string &path);
            i32 execute();

        private:
            enum State {
                WAITING,
                EXPECTING_INSTRUCTION,
                EXPECTING_ONE_ARG,
                EXPECTING_TWO_ARGS,
                ARG_IS_CONSTANT,
                ARG_IS_REGISTER,
                ARG_IS_LABEL,
                CMP_TRUE,
                CMP_FALSE,
                ERROR
            };

            enum CmpState {
                FALSE = -1,
                INDETERMINATE,
                TRUE
            };

            enum InstructionType {
                MOV, ADD, MUL, 
                CLT, JNE, JEQ, 
                OUT, REG, HEX,
                LBL, NA
            };

            struct Instruction {
                std::string inst;
                InstructionType type;
            };

            b8 determine(Instruction &i, b8 &cmp_last);

            State internal_state{WAITING};
            CmpState cmp_state{INDETERMINATE};
            i8 arg_count{0};
            i32 reg_32[8];

            std::vector<Instruction> instruction_list{};
            std::unordered_map<std::string, u64> jmp_positions;
    };
}