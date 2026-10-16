#include "interpreter.hpp"
#include "raylib_backend.hpp"
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

  chp::Multimedia_Backend backend;
  chp::initialize_raylib_backend(backend);

  chp::run_program(ctx, backend);

  chp::destroy_raylib_backend(backend);
  return 0;
}
