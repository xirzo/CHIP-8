#include "interpreter.hpp"
#include "opcode.hpp"
#include <iostream>

constexpr auto TEST_PROGRAM_PATH = "./build/prog.chip";

int main(void) {
  chp::Memory_Context ctx;

  chp::initialize_context(ctx);

  chp::Load_Program_Result load_result =
      chp::load_program_from_file(ctx, TEST_PROGRAM_PATH);

  if (!load_result.has_value()) {
      std::cerr << load_result.error() << std::endl;
      return 1;
  }

  chp::word test = 0b0000110100000000;
  chp::interpret_3xnn(ctx, test);

  return 0;
}
