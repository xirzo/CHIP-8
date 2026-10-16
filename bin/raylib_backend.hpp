#pragma once

#include "interpreter.hpp"
#include <raylib.h>

namespace chp {

void initialize_raylib_backend(Multimedia_Backend &backend);

void destroy_raylib_backend(Multimedia_Backend &backend);

void pool_input();

}
