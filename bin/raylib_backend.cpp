#include "raylib_backend.hpp"
#include "interpreter.hpp"
#include <array>
#include <memory>
#include <raylib.h>

namespace chp {

struct KeyMap {
  int raylib_key;
  Byte chip8_key;
};

constexpr KeyMap KEY_MAP[] = {
    {KEY_ONE, 0x1}, {KEY_TWO, 0x2}, {KEY_THREE, 0x3}, {KEY_FOUR, 0xC},
    {KEY_Q, 0x4},   {KEY_W, 0x5},   {KEY_E, 0x6},     {KEY_R, 0xD},
    {KEY_A, 0x7},   {KEY_S, 0x8},   {KEY_D, 0x9},     {KEY_F, 0xE},
    {KEY_Z, 0xA},   {KEY_X, 0x0},   {KEY_C, 0xB},     {KEY_V, 0xF},
};

constexpr int PIXEL_SCALE = 14;

constexpr float PHOSPHOR_DECAY = 0.6f;
constexpr float MIN_VISIBLE_INTENSITY = 0.02f;

struct Raylib_State {
  Sound beep_sound;
  std::array<std::array<float, SCREEN_WIDTH>, SCREEN_HEIGHT> intensity;
};

static Sound generate_beep_sound() {
  constexpr int SAMPLE_RATE = 44100;
  constexpr int FREQUENCY = 440;
  constexpr short AMPLITUDE = 6000;

  const int half_period = SAMPLE_RATE / FREQUENCY / 2;
  const int frame_count = half_period * 2 * 44;

  Wave wave = {};
  wave.frameCount = static_cast<unsigned int>(frame_count);
  wave.sampleRate = SAMPLE_RATE;
  wave.sampleSize = 16;
  wave.channels = 1;
  wave.data = MemAlloc(frame_count * sizeof(short));

  auto *samples = static_cast<short *>(wave.data);
  for (int i = 0; i < frame_count; i++) {
    bool high = (i / half_period) % 2 == 0;
    samples[i] = high ? AMPLITUDE : -AMPLITUDE;
  }

  Sound sound = LoadSoundFromWave(wave);
  UnloadWave(wave);
  return sound;
}

void poll_input(Chip_Context &ctx) {
  ctx.keypad.fill(false);

  for (auto [raylib_key, chip8_key] : KEY_MAP) {
    if (IsKeyDown(raylib_key)) {
      ctx.keypad[chip8_key] = true;
    }
  }
}

static void render_frame(Raylib_State &state, Chip_Context &ctx) {
  BeginDrawing();
  ClearBackground(BLACK);

  const int offset_x =
      (GetScreenWidth() - static_cast<int>(SCREEN_WIDTH) * PIXEL_SCALE) / 2;
  const int offset_y =
      (GetScreenHeight() - static_cast<int>(SCREEN_HEIGHT) * PIXEL_SCALE) / 2;

  for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
      float &value = state.intensity[y][x];
      if (ctx.framebuffer[y][x] != 0) {
        value = 1.0f;
      } else {
        value *= PHOSPHOR_DECAY;
      }
      if (value < MIN_VISIBLE_INTENSITY) {
        continue;
      }
      auto level = static_cast<unsigned char>(value * 255.0f);
      DrawRectangle(offset_x + static_cast<int>(x) * PIXEL_SCALE,
                    offset_y + static_cast<int>(y) * PIXEL_SCALE, PIXEL_SCALE,
                    PIXEL_SCALE, Color{level, level, level, 255});
    }
  }

  EndDrawing();
}

static void update_sound(Raylib_State &state, Chip_Context &ctx) {
  const bool is_playing = IsSoundPlaying(state.beep_sound);
  if (ctx.sound_timer > 0) {
    if (!is_playing) {
      PlaySound(state.beep_sound);
    }
  } else if (is_playing) {
    StopSound(state.beep_sound);
  }
}

void initialize_raylib_backend(Multimedia_Backend &backend) {
  InitWindow(900, 600, "Chip8");
  InitAudioDevice();

  auto state = std::make_shared<Raylib_State>();
  state->beep_sound = generate_beep_sound();
  state->intensity = {};
  SetSoundVolume(state->beep_sound, 0.5f);

  backend.pool_input = poll_input;
  backend.render_frame = [state](Chip_Context &ctx) {
    render_frame(*state, ctx);
  };
  backend.update_sound = [state](Chip_Context &ctx) {
    update_sound(*state, ctx);
  };
  backend.shutdown = [state]() { UnloadSound(state->beep_sound); };
  backend.should_exit = WindowShouldClose;
}

void destroy_raylib_backend(Multimedia_Backend &backend) {
  backend.shutdown();

  CloseAudioDevice();
  CloseWindow();
}

} // namespace chp
