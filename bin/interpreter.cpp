#include "interpreter.hpp"
#include "opcode.hpp"
#include <algorithm>
#include <chrono>
#include <expected>
#include <format>
#include <fstream>
#include <random>
#include <utility>

namespace chp {

void initialize_context(Chip_Context &ctx) {
  std::fill(ctx.memory.begin(), ctx.memory.end(), 0);
  std::fill(ctx.v_registers.begin(), ctx.v_registers.end(), 0);
  std::fill(ctx.stack.begin(), ctx.stack.end(), 0);
  std::fill(ctx.keypad.begin(), ctx.keypad.end(), 0);
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
  std::chrono::time_point last_timer_tick = Clock::now();

  while (!backend.should_exit()) {
    backend.pool_input(ctx);
    interpret_program_tick(ctx);
    backend.render_frame(ctx);

    std::chrono::time_point now = Clock::now();
    while (now - last_timer_tick >= TIMER_PERIOD) {
      tick_timers(ctx);
      last_timer_tick += TIMER_PERIOD;
    }
  }
}

} // namespace chp
