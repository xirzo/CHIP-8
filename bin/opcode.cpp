#include "opcode.hpp"
#include <algorithm>
#include "interpreter.hpp"
#include <cassert>

namespace chp {

constexpr byte get_key_from_register(word reg) {
  return static_cast<byte>(reg & 0b0000000000001111);
}

constexpr byte get_x_from_opcode(word opcode) {
  return static_cast<byte>((opcode & 0b0000111100000000) >> 8);
}

constexpr byte get_y_from_opcode(word opcode) {
  return static_cast<byte>((opcode & 0b0000000011110000) >> 4);
}

constexpr word get_nn_from_opcode(word opcode) {
  return (opcode & 0b0000000011111111);
}

constexpr word get_nnn_from_opcode(word opcode) {
  return (opcode & 0b0000111111111111);
}

constexpr byte get_n_from_opcode(word opcode) {
  return static_cast<byte>(opcode);
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
  ctx.pc_register = get_nnn_from_opcode(opcode);
}

void interpret_2nnn(Memory_Context &ctx, word opcode) {
  if (ctx.stack_pointer >= MAX_STACK_DEPTH) {
    assert(false && "stack_pointer will go above the top, aborting...");
    return;
  }

  ctx.stack[ctx.stack_pointer] = ctx.pc_register;
  ctx.stack_pointer++;
  ctx.pc_register = get_nnn_from_opcode(opcode);
}

void interpret_3xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word comparison_value = get_nn_from_opcode(opcode);

  if (ctx.v_registers[x] == comparison_value) {
    ctx.pc_register += 2;
  }
}

void interpret_4xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word comparison_value = get_nn_from_opcode(opcode);

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
  word value = get_nn_from_opcode(opcode);
  ctx.v_registers[x] = value;
}

void interpret_7xnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word value = get_nn_from_opcode(opcode);
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

void interpret_8xye(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte shifted_bit = (ctx.v_registers[x] & 0b10000000) ? 1 : 0;
  ctx.v_registers[x] <<= 1;
  ctx.v_registers[0xF] = shifted_bit;
}

void interpret_9xy0(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte y = get_y_from_opcode(opcode);
  byte vx = ctx.v_registers[x];
  byte vy = ctx.v_registers[y];
  if (vx != vy) {
    ctx.pc_register += 2;
  }
}

void interpret_annn(Memory_Context &ctx, word opcode) {
  ctx.i_register = get_nnn_from_opcode(opcode);
}

void interpret_bnnn(Memory_Context &ctx, word opcode) {
  word nnn = get_nnn_from_opcode(opcode);
  ctx.pc_register = (ctx.v_registers[0] + nnn) & 0x0000111111111111;
}

void interpret_cxnn(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  word nn = get_nn_from_opcode(opcode);

  std::uniform_int_distribution<int> distribution(0, 255);
  byte random_num = distribution(ctx.rng);
  ctx.v_registers[x] = static_cast<byte>(random_num & nn);
}

void interpret_dxyn(Memory_Context &ctx, word opcode) {
  byte coord_x = ctx.v_registers[get_x_from_opcode(opcode)];
  byte coord_y = ctx.v_registers[get_y_from_opcode(opcode)];
  byte height = get_n_from_opcode(opcode);

  if (ctx.i_register + height > TOTAL_MEMORY_SIZE_BYTES) {
    return;
  }

  ctx.v_registers[15] = 0;

  for (int row = 0; row < height; row++) {
    byte sprite_byte = ctx.memory[ctx.i_register + row];

    for (int col = 0; col < 8; col++) {
      if ((sprite_byte & (0x80 >> col)) == 0) {
        continue;
      }

      byte x = (coord_x + col) % SCREEN_WIDTH;
      byte y = (coord_y + row) % SCREEN_HEIGHT;

      if (ctx.framebuffer[x][y] == 1) {
        ctx.v_registers[15] = 1;
      }

      ctx.framebuffer[x][y] ^= 1;
    }
  }
}

void interpret_ex9e(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte key = get_key_from_register(ctx.v_registers[x]);
  if (ctx.keypad[key]) {
    ctx.pc_register += 2;
  }
}

void interpret_exa1(Memory_Context &ctx, word opcode) {
  byte x = get_x_from_opcode(opcode);
  byte key = get_key_from_register(ctx.v_registers[x]);
  if (!ctx.keypad[key]) {
    ctx.pc_register += 2;
  }
}

void interpret_fx07(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

// TODO: don't forget to handle blocking with "is_waiting_for_input"
void interpret_fx0a(Memory_Context &ctx, word opcode) {
  if (!ctx.is_waiting_for_input) {
    ctx.is_waiting_for_input = true;
  }

  byte x = get_x_from_opcode(opcode);
  for (byte i = 0; i < ctx.keypad.size(); i++) {
    if (ctx.keypad[i]) {
      ctx.is_waiting_for_input = false;
      ctx.v_registers[x] = i;
      break;
    }
  }
}

void interpret_fx15(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx18(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx1e(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx29(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx33(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx55(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

void interpret_fx65(Memory_Context &ctx, word opcode) {
  (void)ctx;
  (void)opcode;
}

}; // namespace chp
