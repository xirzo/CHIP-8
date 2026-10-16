#include "raylib_backend.hpp"
#include "interpreter.hpp"
#include <raylib.h>

namespace chp {

struct KeyMap {
  int raylib_key;
  byte chip8_key;
};

static constexpr KeyMap KEY_MAP[] = {
    {KEY_ONE, 0x1}, {KEY_TWO, 0x2}, {KEY_THREE, 0x3}, {KEY_FOUR, 0xC},
    {KEY_Q, 0x4},   {KEY_W, 0x5},   {KEY_E, 0x6},     {KEY_R, 0xD},
    {KEY_A, 0x7},   {KEY_S, 0x8},   {KEY_D, 0x9},     {KEY_F, 0xE},
    {KEY_Z, 0xA},   {KEY_X, 0x0},   {KEY_C, 0xB},     {KEY_V, 0xF},
};

void poll_input(Memory_Context &ctx) {
  (void)ctx;
  ctx.keypad.fill(false);

  for (auto [raylib_key, chip8_key] : KEY_MAP) {
    if (IsKeyDown(raylib_key)) {
      ctx.keypad[chip8_key] = true;
    }
  }
}

static void render_frame(Memory_Context &ctx) {
  (void)ctx;
  BeginDrawing();
  ClearBackground(BLACK);
  EndDrawing();
}

void initialize_raylib_backend(Multimedia_Backend &backend) {
  InitWindow(900, 600, "Chip8");
  InitAudioDevice();

  backend.pool_input = poll_input;
  backend.render_frame = render_frame;
  backend.should_exit = WindowShouldClose;
}

void destroy_raylib_backend(Multimedia_Backend &backend) {
  (void)backend;

  CloseAudioDevice();
  CloseWindow();
}

} // namespace chp
