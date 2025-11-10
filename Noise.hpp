// Noise.hpp
// ----------
// Single include header for the PackageNoise library
// Gives access to all available noise map types.
// Usage:
//   #include "Noise.hpp"
//   auto noise = Noise::create_whitenoise(...);

#pragma once

#include "NoiseMaps/WhiteNoise/include/WhiteNoise.hpp"
#include "NoiseMaps/PerlinNoise/include/PerlinNoise.hpp"
#include "NoiseMaps/SimplexNoise/include/SimplexNoise.hpp"
