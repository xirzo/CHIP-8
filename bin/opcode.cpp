#include "opcode.hpp"
#include "interpreter.hpp"
#include <algorithm>
#include <cassert>

namespace chp {

void interpret_program_tick(Chip_Context& ctx) {
    (void)ctx;
}

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
  return static_cast<Byte>(opcode);
}

// noop
void interpret_0nnn(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

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
  ctx.pc_register = (ctx.v_registers[0] + nnn) & 0x0000111111111111;
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

      if (ctx.framebuffer[x][y] == 1) {
        ctx.v_registers[15] = 1;
      }

      ctx.framebuffer[x][y] ^= 1;
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
  (void)ctx;
  (void)opcode;
}

// TODO: don't forget to handle blocking with "is_waiting_for_input"
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
  (void)ctx;
  (void)opcode;
}

void interpret_fx18(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx1e(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx29(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx33(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx55(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx65(Chip_Context &ctx, Word opcode) {
  (void)ctx;
  (void)opcode;
}

}; // namespace chp
