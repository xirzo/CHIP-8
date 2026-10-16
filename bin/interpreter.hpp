#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <expected>
#include <filesystem>
#include <functional>
#include <random>
#include <string>

namespace chp {

constexpr size_t TOTAL_MEMORY_SIZE_BYTES = 4096;
constexpr size_t DISPLAY_RESFRESH_MEMORY_SIZE_BYTES = 256;
constexpr size_t MISC_MEMORY_SIZE_BYTES = 96;
constexpr size_t PROGRAM_START_ADDRESS = 512;
constexpr size_t PROGRAM_END_ADDRESS = TOTAL_MEMORY_SIZE_BYTES -
                                       DISPLAY_RESFRESH_MEMORY_SIZE_BYTES -
                                       MISC_MEMORY_SIZE_BYTES;
constexpr size_t PROGRAM_MEMORY_SIZE_BYTES =
    PROGRAM_END_ADDRESS - PROGRAM_START_ADDRESS;
constexpr size_t NUMBER_OF_V_REGISTERS = 16;
constexpr size_t MAX_STACK_DEPTH = 16;
constexpr size_t SCREEN_WIDTH = 64;
constexpr size_t SCREEN_HEIGHT = 32;
constexpr size_t KEYS_NUMBER = 16;
constexpr auto NANOSECONDS_IN_SECOND = 1000000000;
constexpr auto TIMER_HZ = 60;
constexpr auto TIMER_PERIOD =
    std::chrono::nanoseconds(NANOSECONDS_IN_SECOND / TIMER_HZ);

using byte = unsigned char;
using word = unsigned short;
using Load_Program_Result = std::expected<void, std::string>;
using Clock = std::chrono::steady_clock;

struct Memory_Context {
  std::array<byte, TOTAL_MEMORY_SIZE_BYTES> memory;
  std::array<byte, NUMBER_OF_V_REGISTERS> v_registers;
  std::array<std::array<byte, SCREEN_WIDTH>, SCREEN_HEIGHT> framebuffer;
  std::array<word, MAX_STACK_DEPTH> stack;
  std::array<bool, KEYS_NUMBER> keypad;
  bool is_waiting_for_input;
  word stack_pointer;
  // guess those should be 12 bits, not 16, as memory size is 4096, but nvm
  word i_register;
  word pc_register;
  std::mt19937 rng;
  byte delay_timer;
  byte sound_timer;
};

struct Multimedia_Backend {
  std::function<bool()> should_exit;
  std::function<void(Memory_Context &)> pool_input;
  std::function<void(Memory_Context &)> render_frame;
};

void initialize_context(Memory_Context &ctx);

Load_Program_Result load_program_from_file(Memory_Context &ctx,
                                           const std::filesystem::path &path);

void run_program(Memory_Context &ctx, Multimedia_Backend &backend);

}; // namespace chp
