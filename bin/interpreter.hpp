#pragma once

#include <array>
#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>

namespace chp {

using byte = unsigned char;
using word = unsigned short;

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

using Load_Program_Result = std::expected<void, std::string>;

struct Memory_Context {
  std::array<byte, TOTAL_MEMORY_SIZE_BYTES> memory;
  std::array<byte, NUMBER_OF_V_REGISTERS> v_registers;
  std::array<std::array<byte, SCREEN_WIDTH>, SCREEN_HEIGHT> framebuffer;
  std::array<word, MAX_STACK_DEPTH> stack;
  word stack_pointer;
  // guess those should be 12 bits, not 16, as memory size is 4096, but nvm
  word i_register;
  word pc_register;
};

void initialize_context(Memory_Context &ctx);

Load_Program_Result load_program_from_file(Memory_Context &ctx,
                                         const std::filesystem::path &path);

}; // namespace chp
