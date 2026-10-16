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
constexpr size_t FONT_START_ADDRESS = 0x50;
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
constexpr auto CPU_HZ = 500;
constexpr auto CYCLES_PER_FRAME = CPU_HZ / TIMER_HZ;

using Byte = unsigned char;
using Word = unsigned short;
using Load_Program_Result = std::expected<void, std::string>;
using Clock = std::chrono::steady_clock;

struct Chip_Context {
  std::array<Byte, TOTAL_MEMORY_SIZE_BYTES> memory;
  std::array<Byte, NUMBER_OF_V_REGISTERS> v_registers;
  std::array<std::array<Byte, SCREEN_WIDTH>, SCREEN_HEIGHT> framebuffer;
  std::array<Word, MAX_STACK_DEPTH> stack;
  std::array<bool, KEYS_NUMBER> keypad;
  bool is_waiting_for_input;
  Word stack_pointer;
  // guess those should be 12 bits, not 16, as memory size is 4096, but nvm
  Word i_register;
  Word pc_register;
  std::mt19937 rng;
  Byte delay_timer;
  Byte sound_timer;
};

struct Multimedia_Backend {
  std::function<bool()> should_exit;
  std::function<void(Chip_Context &)> pool_input;
  std::function<void(Chip_Context &)> render_frame;
  std::function<void(Chip_Context &)> update_sound;
  std::function<void()> shutdown;
};

inline constexpr std::array<Byte, 80> FONT_SET = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize_context(Chip_Context &ctx);

Load_Program_Result load_program_from_file(Chip_Context &ctx,
                                           const std::filesystem::path &path);

void run_program(Chip_Context &ctx, Multimedia_Backend &backend);

}; // namespace chp
