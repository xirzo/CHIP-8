#include "opcode.hpp"
#include "interpreter.hpp"
#include <cassert>

namespace chp {

constexpr byte get_x_from_opcode(word opcode) {
  return static_cast<byte>((opcode & 0b0000111100000000) >> 8);
}

constexpr byte get_y_from_opcode(word opcode) {
  return static_cast<byte>((opcode & 0b0000000011110000) >> 4);
}

// noop
void interpret_0nnn(Memory_Context &ctx, word opcode) {
    (void)ctx;
    (void)opcode;
}

void interpret_00e0(Memory_Context &ctx) {
  for (auto &row : ctx.framebuffer) {
    std::fill(row.begin(), row.end(), 0);
  }
}

void interpret_00ee(Memory_Context &ctx) {
  if (ctx.stack_pointer == 0) {
    assert(false && "stack_pointer will go below zero, aborting...");
    return;
  }

  ctx.stack_pointer--;
  ctx.pc_register = ctx.stack[ctx.stack_pointer];
}

void interpret_1nnn(Memory_Context &ctx, word opcode) {
  ctx.pc_register = opcode & 0b0000111111111111;
}

void interpret_2nnn(Memory_Context &ctx, word opcode) {
  if (ctx.stack_pointer >= MAX_STACK_DEPTH) {
    assert(false && "stack_pointer will go above the top, aborting...");
    return;
  }

  ctx.stack[ctx.stack_pointer] = ctx.pc_register;
  ctx.stack_pointer++;
  ctx.pc_register = opcode & 0b0000111111111111;
}

void interpret_3xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word comparison_value = (opcode & 0b0000000011111111);

  if (ctx.v_registers[x] == comparison_value) {
    ctx.pc_register += 2;
  }
}

void interpret_4xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word comparison_value = (opcode & 0b0000000011111111);

  if (ctx.v_registers[x] != comparison_value) {
    ctx.pc_register += 2;
  }
}

void interpret_5xy0(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);

  if (ctx.v_registers[x] == ctx.v_registers[y]) {
    ctx.pc_register += 2;
  }
}

void interpret_6xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word value = (opcode & 0b0000000011111111);
  ctx.v_registers[x] = value;
}

void interpret_7xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word value = (opcode & 0b0000000011111111);
  ctx.v_registers[x] += value;
}

void interpret_8xy0(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] = ctx.v_registers[y];
}

void interpret_8xy1(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] |= ctx.v_registers[y];
}

void interpret_8xy2(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] &= ctx.v_registers[y];
}

void interpret_8xy3(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] ^= ctx.v_registers[y];
}

void interpret_8xy4(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);

  word sum = ctx.v_registers[x] + ctx.v_registers[y];
  ctx.v_registers[x] = static_cast<byte>(sum);
  ctx.v_registers[0xF] = (sum > 0b11111111) ? 1 : 0;
}

void interpret_8xy5(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  byte vx = ctx.v_registers[x];
  byte vy = ctx.v_registers[y];
  ctx.v_registers[x] = vx - vy;
  ctx.v_registers[0xF] = (vx >= vy) ? 1 : 0;
}

void interpret_8xy6(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte shifted_bit = ctx.v_registers[x] & 0b0000000000000001;
  ctx.v_registers[x] >>= 1;
  ctx.v_registers[0xF] = shifted_bit;
}

void interpret_8xy7(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  byte vx = ctx.v_registers[x];
  byte vy = ctx.v_registers[y];
  ctx.v_registers[x] = vy - vx;
  ctx.v_registers[0xF] = (vy >= vx) ? 1 : 0;
}
}

}; // namespace chp
