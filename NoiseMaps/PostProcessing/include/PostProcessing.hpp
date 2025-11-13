// PostProcessing.hpp
// ------------------
// Post-processing utilities for noise maps
// Includes blur, erosion, terracing, normalization, and edge detection

#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

#include <vector>
#include <functional>

namespace Noise {

// ============================================================================
// Smoothing & Blur
// ============================================================================

// Apply Gaussian blur with specified radius
std::vector<std::vector<float>> gaussian_blur(
    const std::vector<std::vector<float>>& map,
    float radius = 1.0f
);

// Apply box blur (faster than Gaussian)
std::vector<std::vector<float>> box_blur(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// Apply median filter (good for noise reduction)
std::vector<std::vector<float>> median_filter(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// ============================================================================
// Erosion & Weathering
// ============================================================================

// Simulate thermal erosion (smooth steep slopes)
std::vector<std::vector<float>> thermal_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations = 5,
    float talusAngle = 0.7f,      // Max slope before erosion
    float erosionRate = 0.5f      // How much material moves
);

// Simulate hydraulic erosion (water-based erosion)
std::vector<std::vector<float>> hydraulic_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations = 10,
    float rainAmount = 0.01f,
    float solubility = 0.1f,
    float evaporation = 0.5f,
    float capacity = 0.1f
);

// Simple erosion (smooths peaks, deepens valleys)
std::vector<std::vector<float>> simple_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations = 5,
    float strength = 0.5f
);

// ============================================================================
// Terracing & Quantization
// ============================================================================

// Create terraced/stepped terrain (like rice paddies or plateaus)
std::vector<std::vector<float>> terrace(
    const std::vector<std::vector<float>>& map,
    int levels = 5,
    float smoothness = 0.1f  // 0 = hard steps, 1 = smooth
);

// Quantize values to specific levels
std::vector<std::vector<float>> quantize(
    const std::vector<std::vector<float>>& map,
    int levels
);

// Apply power curve to adjust contrast
std::vector<std::vector<float>> power_curve(
    const std::vector<std::vector<float>>& map,
    float power = 2.0f  // >1 = darker, <1 = brighter
);

// ============================================================================
// Normalization & Clamping
// ============================================================================

// Normalize to [0, 1] range
std::vector<std::vector<float>> normalize(
    const std::vector<std::vector<float>>& map
);

// Normalize to custom range
std::vector<std::vector<float>> normalize_range(
    const std::vector<std::vector<float>>& map,
    float minVal,
    float maxVal
);

// Clamp values to range
std::vector<std::vector<float>> clamp_values(
    const std::vector<std::vector<float>>& map,
    float minVal = 0.0f,
    float maxVal = 1.0f
);

// Remap values from one range to another
std::vector<std::vector<float>> remap(
    const std::vector<std::vector<float>>& map,
    float oldMin,
    float oldMax,
    float newMin,
    float newMax
);

// ============================================================================
// Edge Detection
// ============================================================================

// Sobel edge detection (finds edges in terrain)
std::vector<std::vector<float>> sobel_edge_detection(
    const std::vector<std::vector<float>>& map,
    float threshold = 0.1f  // Minimum edge strength
);

// Laplacian edge detection (finds rapid changes)
std::vector<std::vector<float>> laplacian_edge_detection(
    const std::vector<std::vector<float>>& map
);

// Extract collision edges for platformers
std::vector<std::vector<bool>> extract_collision_edges(
    const std::vector<std::vector<float>>& map,
    float solidThreshold = 0.5f
);

// Find contour lines at specific heights
std::vector<std::vector<bool>> find_contours(
    const std::vector<std::vector<float>>& map,
    float height,
    float tolerance = 0.05f
);

// ============================================================================
// Gradient & Slope
// ============================================================================

// Calculate gradient magnitude at each point
std::vector<std::vector<float>> calculate_gradient(
    const std::vector<std::vector<float>>& map
);

// Calculate slope angle (in radians)
std::vector<std::vector<float>> calculate_slope(
    const std::vector<std::vector<float>>& map
);

// Find areas with slope below threshold (flat areas)
std::vector<std::vector<bool>> find_flat_areas(
    const std::vector<std::vector<float>>& map,
    float maxSlope = 0.1f
);

// ============================================================================
// Morphological Operations
// ============================================================================

// Dilate (expand bright areas)
std::vector<std::vector<float>> dilate(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// Erode (shrink bright areas)
std::vector<std::vector<float>> erode(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// Open (erode then dilate - removes small bright spots)
std::vector<std::vector<float>> morphological_open(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// Close (dilate then erode - fills small dark spots)
std::vector<std::vector<float>> morphological_close(
    const std::vector<std::vector<float>>& map,
    int radius = 1
);

// ============================================================================
// Combining & Blending
// ============================================================================

// Add two maps together
std::vector<std::vector<float>> add_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2,
    float weight1 = 0.5f,
    float weight2 = 0.5f
);

// Multiply two maps
std::vector<std::vector<float>> multiply_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
);

// Take maximum of two maps at each point
std::vector<std::vector<float>> max_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
);

// Take minimum of two maps at each point
std::vector<std::vector<float>> min_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
);

// ============================================================================
// Utility Functions
// ============================================================================

// Apply custom function to each pixel
std::vector<std::vector<float>> apply_function(
    const std::vector<std::vector<float>>& map,
    std::function<float(float)> func
);

// Invert values (1 - x)
std::vector<std::vector<float>> invert(
    const std::vector<std::vector<float>>& map
);

// Get statistics about the map
struct MapStats {
    float min;
    float max;
    float mean;
    float stddev;
};

MapStats calculate_stats(const std::vector<std::vector<float>>& map);

// Create a copy of the map
std::vector<std::vector<float>> copy_map(
    const std::vector<std::vector<float>>& map
);

} // namespace Noise

#endif // POSTPROCESSING_HPP
