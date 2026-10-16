#pragma once

#include "interpreter.hpp"
#include <cassert>

namespace chp {

void interpret_program_tick(Chip_Context &ctx);

void interpret_0nnn(Chip_Context &ctx);

void interpret_00e0(Chip_Context &ctx);

void interpret_00ee(Chip_Context &ctx);

void interpret_1nnn(Chip_Context &ctx, Word opcode);

void interpret_2nnn(Chip_Context &ctx, Word opcode);

void interpret_3xnn(Chip_Context &ctx, Word opcode);

void interpret_4xnn(Chip_Context &ctx, Word opcode);

void interpret_5xy0(Chip_Context &ctx, Word opcode);

void interpret_6xnn(Chip_Context &ctx, Word opcode);

void interpret_7xnn(Chip_Context &ctx, Word opcode);

void interpret_8xy0(Chip_Context &ctx, Word opcode);

void interpret_8xy1(Chip_Context &ctx, Word opcode);

void interpret_8xy2(Chip_Context &ctx, Word opcode);

void interpret_8xy3(Chip_Context &ctx, Word opcode);

void interpret_8xy4(Chip_Context &ctx, Word opcode);

void interpret_8xy5(Chip_Context &ctx, Word opcode);

void interpret_8xy6(Chip_Context &ctx, Word opcode);

void interpret_8xy7(Chip_Context &ctx, Word opcode);

void interpret_8xye(Chip_Context &ctx, Word opcode);

void interpret_9xy0(Chip_Context &ctx, Word opcode);

void interpret_annn(Chip_Context &ctx, Word opcode);

void interpret_bnnn(Chip_Context &ctx, Word opcode);

void interpret_cxnn(Chip_Context &ctx, Word opcode);

void interpret_dxyn(Chip_Context &ctx, Word opcode);

void interpret_ex9e(Chip_Context &ctx, Word opcode);

void interpret_exa1(Chip_Context &ctx, Word opcode);

void interpret_fx07(Chip_Context &ctx, Word opcode);

void interpret_fx0a(Chip_Context &ctx, Word opcode);

void interpret_fx15(Chip_Context &ctx, Word opcode);

void interpret_fx18(Chip_Context &ctx, Word opcode);

void interpret_fx1e(Chip_Context &ctx, Word opcode);

void interpret_fx29(Chip_Context &ctx, Word opcode);

void interpret_fx33(Chip_Context &ctx, Word opcode);

void interpret_fx55(Chip_Context &ctx, Word opcode);

void interpret_fx65(Chip_Context &ctx, Word opcode);

}; // namespace chp
