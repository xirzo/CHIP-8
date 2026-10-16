#pragma once

#include "interpreter.hpp"
#include <cassert>

namespace chp {

void interpret_0nnn(Memory_Context &ctx, word opcode);

void interpret_00e0(Memory_Context &ctx);

void interpret_00ee(Memory_Context &ctx);

void interpret_1nnn(Memory_Context &ctx, word opcode);

void interpret_2nnn(Memory_Context &ctx, word opcode);

void interpret_3xnn(Memory_Context &ctx, word opcode);

void interpret_4xnn(Memory_Context &ctx, word opcode);

void interpret_5xy0(Memory_Context &ctx, word opcode);

void interpret_6xnn(Memory_Context &ctx, word opcode);

void interpret_7xnn(Memory_Context &ctx, word opcode);

void interpret_8xy0(Memory_Context &ctx, word opcode);

void interpret_8xy1(Memory_Context &ctx, word opcode);

void interpret_8xy2(Memory_Context &ctx, word opcode);

void interpret_8xy3(Memory_Context &ctx, word opcode);

void interpret_8xy4(Memory_Context &ctx, word opcode);

void interpret_8xy5(Memory_Context &ctx, word opcode);

void interpret_8xy6(Memory_Context &ctx, word opcode);

void interpret_8xy7(Memory_Context &ctx, word opcode);

void interpret_8xye(Memory_Context &ctx, word opcode);

void interpret_9xy0(Memory_Context &ctx, word opcode);

void interpret_annn(Memory_Context &ctx, word opcode);

void interpret_bnnn(Memory_Context &ctx, word opcode);

void interpret_cxnn(Memory_Context &ctx, word opcode);

void interpret_dxyn(Memory_Context &ctx, word opcode);

void interpret_ex9e(Memory_Context &ctx, word opcode);

void interpret_exa1(Memory_Context &ctx, word opcode);

void interpret_fx07(Memory_Context &ctx, word opcode);

void interpret_fx0a(Memory_Context &ctx, word opcode);

void interpret_fx15(Memory_Context &ctx, word opcode);

void interpret_fx18(Memory_Context &ctx, word opcode);

void interpret_fx1e(Memory_Context &ctx, word opcode);

void interpret_fx29(Memory_Context &ctx, word opcode);

void interpret_fx33(Memory_Context &ctx, word opcode);

void interpret_fx55(Memory_Context &ctx, word opcode);

void interpret_fx65(Memory_Context &ctx, word opcode);

}; // namespace chp
