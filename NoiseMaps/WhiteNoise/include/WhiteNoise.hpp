// WhiteNoise.hpp
// ----------------
// A simple, standalone C++ header for generating 2D white noise maps.
//
// Usage Example:
// ---------------
// #include "WhiteNoise.hpp"
// auto noise = Noise::WhiteNoise::generate(512, 512, 42);
// Noise::WhiteNoise::save(noise, "white_noise.png");
//

#pragma once
#include <vector>
#include <string>

namespace Noise {

    class WhiteNoise {
    public:
        // Generate a 2D white noise map (values between 0.0 and 1.0)
        static std::vector<std::vector<float>> generate(int width, int height, int seed = -1);

        // Display (prints small preview as text)
        static void show(const std::vector<std::vector<float>>& noise);

        // Save to grayscale PNG or JPEG (auto-detected from extension)
        static void save(const std::vector<std::vector<float>>& noise, const std::string& filename = "white_noise.png");
    };

    // Helper wrapper (like your create_whitenoise in Python)
    std::vector<std::vector<float>> create_whitenoise(int width = 256,
        int height = 256,
        int seed = -1,
        const std::string& showMap = "map",
        const std::string& filename = "white_noise.png");

} // namespace Noise
