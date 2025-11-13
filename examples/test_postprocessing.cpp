// test_postprocessing.cpp
// -----------------------
// Test suite for PostProcessing utilities

#include "Noise.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

#include "stb_image_write.h"

// Helper to generate Perlin noise map
std::vector<std::vector<float>> generate_perlin(int width, int height, unsigned int seed, float scale) {
    return Noise::generate_perlin_map(width, height, scale, 4, 1.0f, 0.5f, 2.0f, 0.0f, seed);
}

// Helper to save maps as PNG
void save_grayscale_png(const std::vector<std::vector<float>>& map, const std::string& filename) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<unsigned char> pixels(width * height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = std::max(0.0f, std::min(1.0f, map[y][x]));
            pixels[y * width + x] = static_cast<unsigned char>(value * 255);
        }
    }
    
    stbi_write_png(("ImageOutput/" + filename).c_str(), width, height, 1, pixels.data(), width);
    std::cout << "  Saved: ImageOutput/" << filename << std::endl;
}

// Test 1: Gaussian Blur
void test_gaussian_blur() {
    std::cout << "\n=== Test 1: Gaussian Blur ===" << std::endl;
    
    // Generate noisy terrain
    auto terrain = generate_perlin(256, 256, 42, 8.0f);
    save_grayscale_png(terrain, "pp_01_original.png");
    
    // Apply different blur radii
    auto blur_small = Noise::gaussian_blur(terrain, 1.0f);
    save_grayscale_png(blur_small, "pp_02_gaussian_r1.png");
    
    auto blur_medium = Noise::gaussian_blur(terrain, 2.0f);
    save_grayscale_png(blur_medium, "pp_03_gaussian_r2.png");
    
    auto blur_large = Noise::gaussian_blur(terrain, 4.0f);
    save_grayscale_png(blur_large, "pp_04_gaussian_r4.png");
    
    std::cout << "✓ Gaussian blur test passed" << std::endl;
}

// Test 2: Box Blur vs Median Filter
void test_blur_comparison() {
    std::cout << "\n=== Test 2: Blur Comparison ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 123, 6.0f);
    
    auto box = Noise::box_blur(terrain, 2);
    save_grayscale_png(box, "pp_05_box_blur.png");
    
    auto median = Noise::median_filter(terrain, 2);
    save_grayscale_png(median, "pp_06_median_filter.png");
    
    std::cout << "✓ Blur comparison test passed" << std::endl;
}

// Test 3: Thermal Erosion
void test_thermal_erosion() {
    std::cout << "\n=== Test 3: Thermal Erosion ===" << std::endl;
    
    // Generate mountainous terrain
    auto terrain = generate_perlin(256, 256, 456, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_07_before_thermal.png");
    
    // Apply thermal erosion
    auto eroded = Noise::thermal_erosion(normalized, 10, 0.05f, 0.3f);
    save_grayscale_png(eroded, "pp_08_thermal_erosion.png");
    
    // More aggressive erosion
    auto heavily_eroded = Noise::thermal_erosion(normalized, 30, 0.03f, 0.5f);
    save_grayscale_png(heavily_eroded, "pp_09_thermal_heavy.png");
    
    std::cout << "✓ Thermal erosion test passed" << std::endl;
}

// Test 4: Hydraulic Erosion
void test_hydraulic_erosion() {
    std::cout << "\n=== Test 4: Hydraulic Erosion ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 789, 5.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_10_before_hydraulic.png");
    
    // Apply hydraulic erosion
    auto eroded = Noise::hydraulic_erosion(normalized, 50, 0.01f, 0.5f, 0.01f, 0.1f);
    save_grayscale_png(eroded, "pp_11_hydraulic_erosion.png");
    
    std::cout << "✓ Hydraulic erosion test passed" << std::endl;
}

// Test 5: Simple Erosion
void test_simple_erosion() {
    std::cout << "\n=== Test 5: Simple Erosion ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 999, 8.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_12_before_simple.png");
    
    auto eroded = Noise::simple_erosion(normalized, 5, 0.3f);
    save_grayscale_png(eroded, "pp_13_simple_erosion.png");
    
    std::cout << "✓ Simple erosion test passed" << std::endl;
}

// Test 6: Terracing
void test_terracing() {
    std::cout << "\n=== Test 6: Terracing ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 111, 6.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_14_before_terrace.png");
    
    // Hard terracing
    auto terraced_hard = Noise::terrace(normalized, 8, 0.0f);
    save_grayscale_png(terraced_hard, "pp_15_terrace_hard.png");
    
    // Smooth terracing
    auto terraced_smooth = Noise::terrace(normalized, 8, 0.15f);
    save_grayscale_png(terraced_smooth, "pp_16_terrace_smooth.png");
    
    // Many levels
    auto terraced_many = Noise::terrace(normalized, 16, 0.1f);
    save_grayscale_png(terraced_many, "pp_17_terrace_many.png");
    
    std::cout << "✓ Terracing test passed" << std::endl;
}

// Test 7: Quantization
void test_quantization() {
    std::cout << "\n=== Test 7: Quantization ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 222, 7.0f);
    auto normalized = Noise::normalize(terrain);
    
    auto quantized = Noise::quantize(normalized, 4);
    save_grayscale_png(quantized, "pp_18_quantized_4.png");
    
    auto quantized8 = Noise::quantize(normalized, 8);
    save_grayscale_png(quantized8, "pp_19_quantized_8.png");
    
    std::cout << "✓ Quantization test passed" << std::endl;
}

// Test 8: Power Curves
void test_power_curves() {
    std::cout << "\n=== Test 8: Power Curves ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 333, 6.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_20_original.png");
    
    // Flatten (emphasize lows)
    auto flattened = Noise::power_curve(normalized, 0.5f);
    save_grayscale_png(flattened, "pp_21_power_0_5.png");
    
    // Sharpen (emphasize highs)
    auto sharpened = Noise::power_curve(normalized, 2.0f);
    save_grayscale_png(sharpened, "pp_22_power_2_0.png");
    
    // Extreme
    auto extreme = Noise::power_curve(normalized, 3.0f);
    save_grayscale_png(extreme, "pp_23_power_3_0.png");
    
    std::cout << "✓ Power curve test passed" << std::endl;
}

// Test 9: Normalization
void test_normalization() {
    std::cout << "\n=== Test 9: Normalization ===" << std::endl;
    
    // Create terrain with random range
    auto terrain = generate_perlin(256, 256, 444, 5.0f);
    
    auto stats_before = Noise::calculate_stats(terrain);
    std::cout << "  Before - Min: " << stats_before.min << ", Max: " << stats_before.max << std::endl;
    
    auto normalized = Noise::normalize(terrain);
    auto stats_after = Noise::calculate_stats(normalized);
    std::cout << "  After  - Min: " << stats_after.min << ", Max: " << stats_after.max << std::endl;
    
    assert(std::abs(stats_after.min) < 0.001f);
    assert(std::abs(stats_after.max - 1.0f) < 0.001f);
    
    // Test normalize_range
    auto ranged = Noise::normalize_range(terrain, -1.0f, 1.0f);
    auto stats_ranged = Noise::calculate_stats(ranged);
    std::cout << "  Ranged - Min: " << stats_ranged.min << ", Max: " << stats_ranged.max << std::endl;
    
    assert(std::abs(stats_ranged.min + 1.0f) < 0.001f);
    assert(std::abs(stats_ranged.max - 1.0f) < 0.001f);
    
    std::cout << "✓ Normalization test passed" << std::endl;
}

// Test 10: Edge Detection
void test_edge_detection() {
    std::cout << "\n=== Test 10: Edge Detection ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 555, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_24_terrain_for_edges.png");
    
    // Sobel edge detection
    auto sobel = Noise::sobel_edge_detection(normalized, 0.05f);
    auto sobel_norm = Noise::normalize(sobel);
    save_grayscale_png(sobel_norm, "pp_25_sobel_edges.png");
    
    // Laplacian edge detection
    auto laplacian = Noise::laplacian_edge_detection(normalized);
    auto laplacian_norm = Noise::normalize(laplacian);
    save_grayscale_png(laplacian_norm, "pp_26_laplacian_edges.png");
    
    // Collision edges
    auto collision = Noise::extract_collision_edges(normalized, 0.5f);
    
    // Convert bool to float for visualization
    std::vector<std::vector<float>> collision_vis(256, std::vector<float>(256, 0.0f));
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            collision_vis[y][x] = collision[y][x] ? 1.0f : 0.0f;
        }
    }
    save_grayscale_png(collision_vis, "pp_27_collision_edges.png");
    
    std::cout << "✓ Edge detection test passed" << std::endl;
}

// Test 11: Gradient & Slope
void test_gradient_slope() {
    std::cout << "\n=== Test 11: Gradient & Slope ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 666, 5.0f);
    auto normalized = Noise::normalize(terrain);
    
    auto gradient = Noise::calculate_gradient(normalized);
    auto gradient_norm = Noise::normalize(gradient);
    save_grayscale_png(gradient_norm, "pp_28_gradient.png");
    
    auto slope = Noise::calculate_slope(normalized);
    auto slope_norm = Noise::normalize(slope);
    save_grayscale_png(slope_norm, "pp_29_slope.png");
    
    auto flat_areas = Noise::find_flat_areas(normalized, 0.1f);
    
    // Visualize flat areas
    std::vector<std::vector<float>> flat_vis(256, std::vector<float>(256));
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            flat_vis[y][x] = flat_areas[y][x] ? 1.0f : 0.0f;
        }
    }
    save_grayscale_png(flat_vis, "pp_30_flat_areas.png");
    
    std::cout << "✓ Gradient & slope test passed" << std::endl;
}

// Test 12: Morphological Operations
void test_morphological() {
    std::cout << "\n=== Test 12: Morphological Operations ===" << std::endl;
    
    auto terrain = generate_perlin(256, 256, 777, 8.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_31_morph_original.png");
    
    auto dilated = Noise::dilate(normalized, 2);
    save_grayscale_png(dilated, "pp_32_dilated.png");
    
    auto eroded = Noise::erode(normalized, 2);
    save_grayscale_png(eroded, "pp_33_eroded.png");
    
    auto opened = Noise::morphological_open(normalized, 2);
    save_grayscale_png(opened, "pp_34_opened.png");
    
    auto closed = Noise::morphological_close(normalized, 2);
    save_grayscale_png(closed, "pp_35_closed.png");
    
    std::cout << "✓ Morphological operations test passed" << std::endl;
}

// Test 13: Map Combining
void test_combining() {
    std::cout << "\n=== Test 13: Map Combining ===" << std::endl;
    
    auto map1 = generate_perlin(256, 256, 888, 6.0f);
    auto map2 = generate_perlin(256, 256, 999, 12.0f);
    
    auto norm1 = Noise::normalize(map1);
    auto norm2 = Noise::normalize(map2);
    
    save_grayscale_png(norm1, "pp_36_combine_map1.png");
    save_grayscale_png(norm2, "pp_37_combine_map2.png");
    
    auto added = Noise::add_maps(norm1, norm2, 0.7f, 0.3f);
    auto added_norm = Noise::normalize(added);
    save_grayscale_png(added_norm, "pp_38_added.png");
    
    auto multiplied = Noise::multiply_maps(norm1, norm2);
    save_grayscale_png(multiplied, "pp_39_multiplied.png");
    
    auto max_map = Noise::max_maps(norm1, norm2);
    save_grayscale_png(max_map, "pp_40_max.png");
    
    auto min_map = Noise::min_maps(norm1, norm2);
    save_grayscale_png(min_map, "pp_41_min.png");
    
    std::cout << "✓ Map combining test passed" << std::endl;
}

// Test 14: Utility Functions
void test_utilities() {
    std::cout << "\n=== Test 14: Utility Functions ===" << std::endl;
    
    auto terrain = generate_perlin(128, 128, 123, 5.0f);
    
    // Test apply_function
    auto squared = Noise::apply_function(terrain, [](float x) { return x * x; });
    assert(squared.size() == terrain.size());
    
    // Test invert
    auto normalized = Noise::normalize(terrain);
    auto inverted = Noise::invert(normalized);
    
    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 128; x++) {
            float sum = normalized[y][x] + inverted[y][x];
            assert(std::abs(sum - 1.0f) < 0.001f);
        }
    }
    
    // Test stats
    auto stats = Noise::calculate_stats(terrain);
    std::cout << "  Stats - Min: " << stats.min << ", Max: " << stats.max 
              << ", Mean: " << stats.mean << ", StdDev: " << stats.stddev << std::endl;
    
    assert(stats.min <= stats.max);
    assert(stats.stddev >= 0.0f);
    
    // Test copy
    auto copied = Noise::copy_map(terrain);
    assert(copied.size() == terrain.size());
    assert(copied[0][0] == terrain[0][0]);
    
    std::cout << "✓ Utility functions test passed" << std::endl;
}

// Test 15: Real-world Scenario - Complete Terrain Pipeline
void test_complete_pipeline() {
    std::cout << "\n=== Test 15: Complete Terrain Pipeline ===" << std::endl;
    
    // 1. Generate base terrain
    auto terrain = generate_perlin(512, 512, 12345, 4.0f);
    auto normalized = Noise::normalize(terrain);
    save_grayscale_png(normalized, "pp_42_pipeline_01_base.png");
    
    // 2. Apply power curve to sharpen peaks
    auto sharpened = Noise::power_curve(normalized, 1.5f);
    save_grayscale_png(sharpened, "pp_43_pipeline_02_sharpened.png");
    
    // 3. Add terracing for plateaus
    auto terraced = Noise::terrace(sharpened, 6, 0.2f);
    save_grayscale_png(terraced, "pp_44_pipeline_03_terraced.png");
    
    // 4. Apply thermal erosion
    auto eroded = Noise::thermal_erosion(terraced, 15, 0.04f, 0.4f);
    save_grayscale_png(eroded, "pp_45_pipeline_04_eroded.png");
    
    // 5. Smooth slightly
    auto smoothed = Noise::simple_erosion(eroded, 2, 0.15f);
    save_grayscale_png(smoothed, "pp_46_pipeline_05_smoothed.png");
    
    // 6. Detect edges for collision
    auto edges = Noise::sobel_edge_detection(smoothed, 0.03f);
    auto edges_norm = Noise::normalize(edges);
    save_grayscale_png(edges_norm, "pp_47_pipeline_06_edges.png");
    
    // 7. Find flat areas for spawning
    auto flat = Noise::find_flat_areas(smoothed, 0.08f);
    std::vector<std::vector<float>> flat_vis(512, std::vector<float>(512));
    for (int y = 0; y < 512; y++) {
        for (int x = 0; x < 512; x++) {
            flat_vis[y][x] = flat[y][x] ? 1.0f : 0.0f;
        }
    }
    save_grayscale_png(flat_vis, "pp_48_pipeline_07_flat_spawns.png");
    
    std::cout << "✓ Complete pipeline test passed" << std::endl;
    std::cout << "\n🎮 Game-ready terrain with terracing, erosion, collision edges, and spawn points!" << std::endl;
}

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "   Post-Processing Test Suite" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        test_gaussian_blur();           // Tests 1-4 (4 images)
        test_blur_comparison();         // Tests 5-6 (2 images)
        test_thermal_erosion();         // Tests 7-9 (3 images)
        test_hydraulic_erosion();       // Tests 10-11 (2 images)
        test_simple_erosion();          // Tests 12-13 (2 images)
        test_terracing();               // Tests 14-17 (4 images)
        test_quantization();            // Tests 18-19 (2 images)
        test_power_curves();            // Tests 20-23 (4 images)
        test_normalization();           // No images (console output)
        test_edge_detection();          // Tests 24-27 (4 images)
        test_gradient_slope();          // Tests 28-30 (3 images)
        test_morphological();           // Tests 31-35 (5 images)
        test_combining();               // Tests 36-41 (6 images)
        test_utilities();               // No images (console output)
        test_complete_pipeline();       // Tests 42-48 (7 images)
        
        std::cout << "\n=====================================" << std::endl;
        std::cout << "   ✅ ALL 15 TESTS PASSED!" << std::endl;
        std::cout << "   📊 48 test images generated" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
