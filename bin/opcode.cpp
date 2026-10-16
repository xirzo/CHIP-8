#include "opcode.hpp"
#include "interpreter.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ios>
#include <iostream>

namespace chp {

constexpr Byte get_key_from_register(Word reg) {
  return static_cast<Byte>(reg & 0b0000000000001111);
}

constexpr Byte get_x_from_opcode(Word opcode) {
  return static_cast<Byte>((opcode & 0b0000111100000000) >> 8);
}

constexpr Byte get_y_from_opcode(Word opcode) {
  return static_cast<Byte>((opcode & 0b0000000011110000) >> 4);
}

constexpr Word get_nn_from_opcode(Word opcode) {
  return (opcode & 0b0000000011111111);
}

constexpr Word get_nnn_from_opcode(Word opcode) {
  return (opcode & 0b0000111111111111);
}

constexpr Byte get_n_from_opcode(Word opcode) {
  return static_cast<Byte>(opcode & 0b0000000000001111);
}

constexpr Byte get_opcode_family(Word opcode) {
  return static_cast<Byte>((opcode >> 12) & 0b0000000000001111);
}

void interpret_program_tick(Chip_Context &ctx) {
  Word opcode = (static_cast<Word>(ctx.memory[ctx.pc_register]) << 8) |
                static_cast<Word>(ctx.memory[ctx.pc_register + 1]);
  ctx.pc_register += 2;

  Byte opcode_family = get_opcode_family(opcode);
  Byte n = get_n_from_opcode(opcode);
  Byte nn = get_nn_from_opcode(opcode);

  switch (opcode_family) {
  case 0x0:
    if (nn == 0xE0)
      interpret_00e0(ctx);
    else if (nn == 0xEE)
      interpret_00ee(ctx);
    else
      interpret_0nnn(ctx);
    break;
  case 0x1:
    interpret_1nnn(ctx, opcode);
    break;
  case 0x2:
    interpret_2nnn(ctx, opcode);
    break;
  case 0x3:
    interpret_3xnn(ctx, opcode);
    break;
  case 0x4:
    interpret_4xnn(ctx, opcode);
    break;
  case 0x5:
    interpret_5xy0(ctx, opcode);
    break;
  case 0x6:
    interpret_6xnn(ctx, opcode);
    break;
  case 0x7:
    interpret_7xnn(ctx, opcode);
    break;
  case 0x8:
    if (n == 0x0)
      interpret_8xy0(ctx, opcode);
    else if (n == 0x1)
      interpret_8xy1(ctx, opcode);
    else if (n == 0x2)
      interpret_8xy2(ctx, opcode);
    else if (n == 0x3)
      interpret_8xy3(ctx, opcode);
    else if (n == 0x4)
      interpret_8xy4(ctx, opcode);
    else if (n == 0x5)
      interpret_8xy5(ctx, opcode);
    else if (n == 0x6)
      interpret_8xy6(ctx, opcode);
    else if (n == 0x7)
      interpret_8xy7(ctx, opcode);
    else
      interpret_8xye(ctx, opcode);
    break;
  case 0x9:
    interpret_9xy0(ctx, opcode);
    break;
  case 0xA:
    interpret_annn(ctx, opcode);
    break;
  case 0xB:
    interpret_bnnn(ctx, opcode);
    break;
  case 0xC:
    interpret_cxnn(ctx, opcode);
    break;
  case 0xD:
    interpret_dxyn(ctx, opcode);
    break;
  case 0xE:
    if (nn == 0x9e)
      interpret_ex9e(ctx, opcode);
    else
      interpret_exa1(ctx, opcode);
    break;
  case 0xF:
    if (nn == 0x07)
      interpret_fx07(ctx, opcode);
    else if (nn == 0x0A)
      interpret_fx0a(ctx, opcode);
    else if (nn == 0x15)
      interpret_fx15(ctx, opcode);
    else if (nn == 0x18)
      interpret_fx18(ctx, opcode);
    else if (nn == 0x1E)
      interpret_fx1e(ctx, opcode);
    else if (nn == 0x29)
      interpret_fx29(ctx, opcode);
    else if (nn == 0x33)
      interpret_fx33(ctx, opcode);
    else if (nn == 0x55)
      interpret_fx55(ctx, opcode);
    else
      interpret_fx65(ctx, opcode);
    break;
  default:
    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    std::abort();
  }

  if (ctx.is_waiting_for_input) {
    ctx.pc_register -= 2;
  }
}

// noop
void interpret_0nnn(Chip_Context &ctx) { (void)ctx; }

void interpret_00e0(Chip_Context &ctx) {
  for (auto &row : ctx.framebuffer) {
    std::fill(row.begin(), row.end(), 0);
  }
}

void interpret_00ee(Chip_Context &ctx) {
  if (ctx.stack_pointer == 0) {
    assert(false && "stack_pointer will go below zero, aborting...");
    return;
  }

  ctx.stack_pointer--;
  ctx.pc_register = ctx.stack[ctx.stack_pointer];
}

void interpret_1nnn(Chip_Context &ctx, Word opcode) {
  ctx.pc_register = get_nnn_from_opcode(opcode);
}

void interpret_2nnn(Chip_Context &ctx, Word opcode) {
  if (ctx.stack_pointer >= MAX_STACK_DEPTH) {
    assert(false && "stack_pointer will go above the top, aborting...");
    return;
  }

  ctx.stack[ctx.stack_pointer] = ctx.pc_register;
  ctx.stack_pointer++;
  ctx.pc_register = get_nnn_from_opcode(opcode);
}

void interpret_3xnn(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Word comparison_value = get_nn_from_opcode(opcode);

  if (ctx.v_registers[x] == comparison_value) {
    ctx.pc_register += 2;
  }
}

void interpret_4xnn(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Word comparison_value = get_nn_from_opcode(opcode);

  if (ctx.v_registers[x] != comparison_value) {
    ctx.pc_register += 2;
  }
}

void interpret_5xy0(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);

  if (ctx.v_registers[x] == ctx.v_registers[y]) {
    ctx.pc_register += 2;
  }
}

void interpret_6xnn(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Word value = get_nn_from_opcode(opcode);
  ctx.v_registers[x] = value;
}

void interpret_7xnn(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Word value = get_nn_from_opcode(opcode);
  ctx.v_registers[x] += value;
}

void interpret_8xy0(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] = ctx.v_registers[y];
}

void interpret_8xy1(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] |= ctx.v_registers[y];
}

void interpret_8xy2(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] &= ctx.v_registers[y];
}

void interpret_8xy3(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  ctx.v_registers[x] ^= ctx.v_registers[y];
}

void interpret_8xy4(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);

  Word sum = ctx.v_registers[x] + ctx.v_registers[y];
  ctx.v_registers[x] = static_cast<Byte>(sum);
  ctx.v_registers[0xF] = (sum > 0b11111111) ? 1 : 0;
}

void interpret_8xy5(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  Byte vx = ctx.v_registers[x];
  Byte vy = ctx.v_registers[y];
  ctx.v_registers[x] = vx - vy;
  ctx.v_registers[0xF] = (vx >= vy) ? 1 : 0;
}

void interpret_8xy6(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte shifted_bit = ctx.v_registers[x] & 0b0000000000000001;
  ctx.v_registers[x] >>= 1;
  ctx.v_registers[0xF] = shifted_bit;
}

void interpret_8xy7(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  Byte vx = ctx.v_registers[x];
  Byte vy = ctx.v_registers[y];
  ctx.v_registers[x] = vy - vx;
  ctx.v_registers[0xF] = (vy >= vx) ? 1 : 0;
}

void interpret_8xye(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte shifted_bit = (ctx.v_registers[x] & 0b10000000) ? 1 : 0;
  ctx.v_registers[x] <<= 1;
  ctx.v_registers[0xF] = shifted_bit;
}

void interpret_9xy0(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte y = get_y_from_opcode(opcode);
  Byte vx = ctx.v_registers[x];
  Byte vy = ctx.v_registers[y];
  if (vx != vy) {
    ctx.pc_register += 2;
  }
}

void interpret_annn(Chip_Context &ctx, Word opcode) {
  ctx.i_register = get_nnn_from_opcode(opcode);
}

void interpret_bnnn(Chip_Context &ctx, Word opcode) {
  Word nnn = get_nnn_from_opcode(opcode);
  ctx.pc_register = (ctx.v_registers[0] + nnn) & 0x0FFF;
}

void interpret_cxnn(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Word nn = get_nn_from_opcode(opcode);

  std::uniform_int_distribution<int> distribution(0, 255);
  Byte random_num = distribution(ctx.rng);
  ctx.v_registers[x] = static_cast<Byte>(random_num & nn);
}

void interpret_dxyn(Chip_Context &ctx, Word opcode) {
  Byte coord_x = ctx.v_registers[get_x_from_opcode(opcode)];
  Byte coord_y = ctx.v_registers[get_y_from_opcode(opcode)];
  Byte height = get_n_from_opcode(opcode);

  if (ctx.i_register + height > TOTAL_MEMORY_SIZE_BYTES) {
    return;
  }

  ctx.v_registers[15] = 0;

  for (int row = 0; row < height; row++) {
    Byte sprite_byte = ctx.memory[ctx.i_register + row];

    for (int col = 0; col < 8; col++) {
      if ((sprite_byte & (0x80 >> col)) == 0) {
        continue;
      }

      Byte x = (coord_x + col) % SCREEN_WIDTH;
      Byte y = (coord_y + row) % SCREEN_HEIGHT;

      if (ctx.framebuffer[y][x] == 1) {
        ctx.v_registers[15] = 1;
      }

      ctx.framebuffer[y][x] ^= 1;
    }
  }
}

void interpret_ex9e(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte key = get_key_from_register(ctx.v_registers[x]);
  if (ctx.keypad[key]) {
    ctx.pc_register += 2;
  }
}

void interpret_exa1(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte key = get_key_from_register(ctx.v_registers[x]);
  if (!ctx.keypad[key]) {
    ctx.pc_register += 2;
  }
}

void interpret_fx07(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  ctx.v_registers[x] = ctx.delay_timer;
}

void interpret_fx0a(Chip_Context &ctx, Word opcode) {
  if (!ctx.is_waiting_for_input) {
    ctx.is_waiting_for_input = true;
  }

  Byte x = get_x_from_opcode(opcode);
  for (Byte i = 0; i < ctx.keypad.size(); i++) {
    if (ctx.keypad[i]) {
      ctx.is_waiting_for_input = false;
      ctx.v_registers[x] = i;
      break;
    }
  }
}

void interpret_fx15(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  ctx.delay_timer = ctx.v_registers[x];
}

void interpret_fx18(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  ctx.sound_timer = ctx.v_registers[x];
}

// NOTE: some emulators do check for overflow, some don't
// there are games that use both of these behaviours
void interpret_fx1e(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  ctx.i_register += ctx.v_registers[x];
}

void interpret_fx29(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  ctx.i_register = FONT_START_ADDRESS + (ctx.v_registers[x] * 5);
}

void interpret_fx33(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);
  Byte value = ctx.v_registers[x];
  ctx.memory[ctx.i_register] = value / 100;
  ctx.memory[ctx.i_register + 1] = (value / 10) % 10;
  ctx.memory[ctx.i_register + 2] = value % 10;
}

void interpret_fx55(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);

  Byte register_pointer = 0;
  Word memory_pointer = ctx.i_register;
  while (register_pointer <= x) {
    ctx.memory[memory_pointer] = ctx.v_registers[register_pointer];
    register_pointer++;
    memory_pointer++;
  }
}

void interpret_fx65(Chip_Context &ctx, Word opcode) {
  Byte x = get_x_from_opcode(opcode);

  Byte register_pointer = 0;
  Word memory_pointer = ctx.i_register;
  while (register_pointer <= x) {
    ctx.v_registers[register_pointer] = ctx.memory[memory_pointer];
    register_pointer++;
    memory_pointer++;
  }
}

}; // namespace chp
