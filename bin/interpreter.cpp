#include "interpreter.hpp"
#include "opcode.hpp"
#include <algorithm>
#include <chrono>
#include <expected>
#include <format>
#include <fstream>
#include <random>
#include <thread>
#include <utility>

namespace chp {

void initialize_context(Chip_Context &ctx) {
  std::fill(ctx.memory.begin(), ctx.memory.end(), 0);
  std::fill(ctx.v_registers.begin(), ctx.v_registers.end(), 0);
  std::fill(ctx.stack.begin(), ctx.stack.end(), 0);
  std::fill(ctx.keypad.begin(), ctx.keypad.end(), 0);
  std::copy(FONT_SET.begin(), FONT_SET.end(),
            ctx.memory.begin() + FONT_START_ADDRESS);
  ctx.is_waiting_for_input = false;
  ctx.i_register = 0;
  ctx.delay_timer = 0;
  ctx.sound_timer = 0;
  ctx.stack_pointer = 0;
  ctx.pc_register = PROGRAM_START_ADDRESS;
  for (auto &row : ctx.framebuffer) {
    std::fill(row.begin(), row.end(), 0);
  }
  ctx.rng.seed(std::random_device{}());
}

Load_Program_Result load_program_from_file(Chip_Context &ctx,
                                           const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    return std::unexpected<std::string>(
        std::format("File by path \"{}\" does not exist", path));
  }

  if (!std::filesystem::is_regular_file(path)) {
    return std::unexpected<std::string>(
        std::format("File by path \"{}\" is not a regular file", path));
  }

  uintmax_t file_size = std::filesystem::file_size(path);
  if (file_size > PROGRAM_MEMORY_SIZE_BYTES) {
    return std::unexpected<std::string>(std::format(
        "File at path \"{}\" exceeds size of CHIP-8 program", path));
  }

  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return std::unexpected<std::string>(
        std::format("Failed to open file by path \"{}\"", path));
  }

  auto program_ptr =
      reinterpret_cast<char *>(ctx.memory.data() + PROGRAM_START_ADDRESS);
  file.read(program_ptr, file_size);

  std::streamsize bytes_read = file.gcount();
  if (std::cmp_less(bytes_read, file_size)) {
    return std::unexpected<std::string>(
        std::format("Could not read all data from file by path \"{}\"", path));
  }

  return {};
}

static void tick_timers(Chip_Context &ctx) {
  if (ctx.delay_timer > 0) {
    ctx.delay_timer--;
  }
  if (ctx.sound_timer > 0) {
    ctx.sound_timer--;
  }
}

void run_program(Chip_Context &ctx, Multimedia_Backend &backend) {
  auto next_frame = Clock::now() + TIMER_PERIOD;

  while (!backend.should_exit()) {
    backend.pool_input(ctx);

    for (int i = 0; i < CYCLES_PER_FRAME; i++) {
      interpret_program_tick(ctx);
    }

    tick_timers(ctx);
    backend.render_frame(ctx);
    backend.update_sound(ctx);

    auto now = Clock::now();
    if (now < next_frame) {
      std::this_thread::sleep_until(next_frame);
    } else if (now - next_frame > TIMER_PERIOD * 5) {
      next_frame = now;
    }
    next_frame += TIMER_PERIOD;
  }
}

} // namespace chp
