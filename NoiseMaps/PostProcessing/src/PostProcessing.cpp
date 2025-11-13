// PostProcessing.cpp
// ------------------
// Implementation of post-processing utilities

#include "PostProcessing.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>

namespace Noise {

// ============================================================================
// Helper Functions
// ============================================================================

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

// ============================================================================
// Smoothing & Blur
// ============================================================================

std::vector<std::vector<float>> gaussian_blur(
    const std::vector<std::vector<float>>& map,
    float radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    // Generate Gaussian kernel
    int kernelSize = static_cast<int>(radius * 3) * 2 + 1;
    int halfSize = kernelSize / 2;
    std::vector<std::vector<float>> kernel(kernelSize, std::vector<float>(kernelSize));
    
    float sigma = radius;
    float sum = 0.0f;
    
    for (int y = -halfSize; y <= halfSize; y++) {
        for (int x = -halfSize; x <= halfSize; x++) {
            float value = std::exp(-(x*x + y*y) / (2 * sigma * sigma));
            kernel[y + halfSize][x + halfSize] = value;
            sum += value;
        }
    }
    
    // Normalize kernel
    for (auto& row : kernel) {
        for (auto& val : row) {
            val /= sum;
        }
    }
    
    // Apply convolution
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = 0.0f;
            
            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    int ny = clamp(y + ky, 0, height - 1);
                    int nx = clamp(x + kx, 0, width - 1);
                    
                    value += map[ny][nx] * kernel[ky + halfSize][kx + halfSize];
                }
            }
            
            result[y][x] = value;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> box_blur(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    int kernelSize = (2 * radius + 1) * (2 * radius + 1);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0.0f;
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int ny = clamp(y + dy, 0, height - 1);
                    int nx = clamp(x + dx, 0, width - 1);
                    sum += map[ny][nx];
                }
            }
            
            result[y][x] = sum / kernelSize;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> median_filter(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::vector<float> values;
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int ny = clamp(y + dy, 0, height - 1);
                    int nx = clamp(x + dx, 0, width - 1);
                    values.push_back(map[ny][nx]);
                }
            }
            
            std::sort(values.begin(), values.end());
            result[y][x] = values[values.size() / 2];
        }
    }
    
    return result;
}

// ============================================================================
// Erosion & Weathering
// ============================================================================

std::vector<std::vector<float>> thermal_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations,
    float talusAngle,
    float erosionRate
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    auto result = map;
    
    const int dx[] = {0, 1, 0, -1, 1, 1, -1, -1};
    const int dy[] = {-1, 0, 1, 0, -1, 1, 1, -1};
    
    for (int iter = 0; iter < iterations; iter++) {
        auto newResult = result;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float maxDiff = 0.0f;
                int maxDir = 0;
                
                // Find steepest descent
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        float diff = result[y][x] - result[ny][nx];
                        if (diff > maxDiff) {
                            maxDiff = diff;
                            maxDir = i;
                        }
                    }
                }
                
                // Erode if slope exceeds talus angle
                if (maxDiff > talusAngle) {
                    float amount = erosionRate * (maxDiff - talusAngle);
                    int nx = x + dx[maxDir];
                    int ny = y + dy[maxDir];
                    
                    newResult[y][x] -= amount;
                    newResult[ny][nx] += amount;
                }
            }
        }
        
        result = newResult;
    }
    
    return result;
}

std::vector<std::vector<float>> hydraulic_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations,
    float rainAmount,
    float solubility,
    float evaporation,
    float capacity
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    auto terrain = map;
    std::vector<std::vector<float>> water(height, std::vector<float>(width, 0.0f));
    std::vector<std::vector<float>> sediment(height, std::vector<float>(width, 0.0f));
    
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    for (int iter = 0; iter < iterations; iter++) {
        // Add rain
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                water[y][x] += rainAmount;
            }
        }
        
        // Simulate water flow
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (water[y][x] > 0.01f) {
                    // Find lowest neighbor
                    float minHeight = terrain[y][x] + water[y][x];
                    int flowX = x, flowY = y;
                    
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            float nHeight = terrain[ny][nx] + water[ny][nx];
                            if (nHeight < minHeight) {
                                minHeight = nHeight;
                                flowX = nx;
                                flowY = ny;
                            }
                        }
                    }
                    
                    // Flow water to lower point
                    if (flowX != x || flowY != y) {
                        float flowAmount = std::min(water[y][x], 0.5f);
                        
                        // Dissolve terrain
                        float dissolved = solubility * flowAmount;
                        terrain[y][x] -= dissolved;
                        sediment[y][x] += dissolved;
                        
                        // Transport water and sediment
                        float sedFlow = sediment[y][x] * (flowAmount / water[y][x]);
                        water[flowY][flowX] += flowAmount;
                        sediment[flowY][flowX] += sedFlow;
                        water[y][x] -= flowAmount;
                        sediment[y][x] -= sedFlow;
                        
                        // Deposit sediment if capacity exceeded
                        if (sediment[flowY][flowX] > capacity * water[flowY][flowX]) {
                            float deposit = sediment[flowY][flowX] - capacity * water[flowY][flowX];
                            terrain[flowY][flowX] += deposit;
                            sediment[flowY][flowX] -= deposit;
                        }
                    }
                }
                
                // Evaporation
                water[y][x] *= (1.0f - evaporation);
            }
        }
    }
    
    return terrain;
}

std::vector<std::vector<float>> simple_erosion(
    const std::vector<std::vector<float>>& map,
    int iterations,
    float strength
) {
    auto result = map;
    
    for (int i = 0; i < iterations; i++) {
        auto blurred = box_blur(result, 1);
        
        int height = static_cast<int>(map.size());
        int width = static_cast<int>(map[0].size());
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                result[y][x] = result[y][x] * (1.0f - strength) + blurred[y][x] * strength;
            }
        }
    }
    
    return result;
}

// ============================================================================
// Terracing & Quantization
// ============================================================================

std::vector<std::vector<float>> terrace(
    const std::vector<std::vector<float>>& map,
    int levels,
    float smoothness
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = map[y][x] * levels;
            float terraceLevel = std::floor(value);
            float fraction = value - terraceLevel;
            
            // Smooth transition between levels
            if (smoothness > 0.0f) {
                float t = clamp(fraction / smoothness, 0.0f, 1.0f);
                t = t * t * (3.0f - 2.0f * t);  // Smoothstep
                result[y][x] = (terraceLevel + t) / levels;
            } else {
                result[y][x] = terraceLevel / levels;
            }
        }
    }
    
    return result;
}

std::vector<std::vector<float>> quantize(
    const std::vector<std::vector<float>>& map,
    int levels
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int level = static_cast<int>(map[y][x] * levels);
            result[y][x] = static_cast<float>(level) / levels;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> power_curve(
    const std::vector<std::vector<float>>& map,
    float power
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = std::pow(map[y][x], power);
        }
    }
    
    return result;
}

// ============================================================================
// Normalization & Clamping
// ============================================================================

std::vector<std::vector<float>> normalize(
    const std::vector<std::vector<float>>& map
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    float minVal = map[0][0];
    float maxVal = map[0][0];
    
    for (const auto& row : map) {
        for (float val : row) {
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
        }
    }
    
    float range = maxVal - minVal;
    if (range < 0.0001f) return map;
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = (map[y][x] - minVal) / range;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> normalize_range(
    const std::vector<std::vector<float>>& map,
    float minVal,
    float maxVal
) {
    auto normalized = normalize(map);
    return remap(normalized, 0.0f, 1.0f, minVal, maxVal);
}

std::vector<std::vector<float>> clamp_values(
    const std::vector<std::vector<float>>& map,
    float minVal,
    float maxVal
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = clamp(map[y][x], minVal, maxVal);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> remap(
    const std::vector<std::vector<float>>& map,
    float oldMin,
    float oldMax,
    float newMin,
    float newMax
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    float oldRange = oldMax - oldMin;
    float newRange = newMax - newMin;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float normalized = (map[y][x] - oldMin) / oldRange;
            result[y][x] = newMin + normalized * newRange;
        }
    }
    
    return result;
}

// ============================================================================
// Edge Detection
// ============================================================================

std::vector<std::vector<float>> sobel_edge_detection(
    const std::vector<std::vector<float>>& map,
    float threshold
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width, 0.0f));
    
    // Sobel kernels
    const int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float gx = 0.0f, gy = 0.0f;
            
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    float val = map[y + dy][x + dx];
                    gx += val * sobelX[dy + 1][dx + 1];
                    gy += val * sobelY[dy + 1][dx + 1];
                }
            }
            
            float magnitude = std::sqrt(gx * gx + gy * gy);
            result[y][x] = magnitude > threshold ? magnitude : 0.0f;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> laplacian_edge_detection(
    const std::vector<std::vector<float>>& map
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width, 0.0f));
    
    // Laplacian kernel
    const int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0.0f;
            
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += map[y + dy][x + dx] * kernel[dy + 1][dx + 1];
                }
            }
            
            result[y][x] = std::abs(sum);
        }
    }
    
    return result;
}

std::vector<std::vector<bool>> extract_collision_edges(
    const std::vector<std::vector<float>>& map,
    float solidThreshold
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<bool>> result(height, std::vector<bool>(width, false));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isSolid = map[y][x] > solidThreshold;
            
            if (isSolid) {
                // Check if any neighbor is air
                bool hasAirNeighbor = false;
                
                for (int dy = -1; dy <= 1 && !hasAirNeighbor; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int ny = y + dy;
                        int nx = x + dx;
                        
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            if (map[ny][nx] <= solidThreshold) {
                                hasAirNeighbor = true;
                                break;
                            }
                        }
                    }
                }
                
                result[y][x] = hasAirNeighbor;
            }
        }
    }
    
    return result;
}

std::vector<std::vector<bool>> find_contours(
    const std::vector<std::vector<float>>& map,
    float height,
    float tolerance
) {
    int mapHeight = static_cast<int>(map.size());
    int mapWidth = static_cast<int>(map[0].size());
    
    std::vector<std::vector<bool>> result(mapHeight, std::vector<bool>(mapWidth, false));
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (std::abs(map[y][x] - height) <= tolerance) {
                result[y][x] = true;
            }
        }
    }
    
    return result;
}

// ============================================================================
// Gradient & Slope
// ============================================================================

std::vector<std::vector<float>> calculate_gradient(
    const std::vector<std::vector<float>>& map
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width, 0.0f));
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float dx = map[y][x + 1] - map[y][x - 1];
            float dy = map[y + 1][x] - map[y - 1][x];
            
            result[y][x] = std::sqrt(dx * dx + dy * dy);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> calculate_slope(
    const std::vector<std::vector<float>>& map
) {
    auto gradient = calculate_gradient(map);
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = std::atan(gradient[y][x]);
        }
    }
    
    return result;
}

std::vector<std::vector<bool>> find_flat_areas(
    const std::vector<std::vector<float>>& map,
    float maxSlope
) {
    auto slope = calculate_slope(map);
    
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<bool>> result(height, std::vector<bool>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = slope[y][x] < maxSlope;
        }
    }
    
    return result;
}

// ============================================================================
// Morphological Operations
// ============================================================================

std::vector<std::vector<float>> dilate(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float maxVal = map[y][x];
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int ny = clamp(y + dy, 0, height - 1);
                    int nx = clamp(x + dx, 0, width - 1);
                    maxVal = std::max(maxVal, map[ny][nx]);
                }
            }
            
            result[y][x] = maxVal;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> erode(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float minVal = map[y][x];
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int ny = clamp(y + dy, 0, height - 1);
                    int nx = clamp(x + dx, 0, width - 1);
                    minVal = std::min(minVal, map[ny][nx]);
                }
            }
            
            result[y][x] = minVal;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> morphological_open(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    return dilate(erode(map, radius), radius);
}

std::vector<std::vector<float>> morphological_close(
    const std::vector<std::vector<float>>& map,
    int radius
) {
    return erode(dilate(map, radius), radius);
}

// ============================================================================
// Combining & Blending
// ============================================================================

std::vector<std::vector<float>> add_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2,
    float weight1,
    float weight2
) {
    int height = static_cast<int>(map1.size());
    int width = static_cast<int>(map1[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = map1[y][x] * weight1 + map2[y][x] * weight2;
        }
    }
    
    return result;
}

std::vector<std::vector<float>> multiply_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
) {
    int height = static_cast<int>(map1.size());
    int width = static_cast<int>(map1[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = map1[y][x] * map2[y][x];
        }
    }
    
    return result;
}

std::vector<std::vector<float>> max_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
) {
    int height = static_cast<int>(map1.size());
    int width = static_cast<int>(map1[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = std::max(map1[y][x], map2[y][x]);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> min_maps(
    const std::vector<std::vector<float>>& map1,
    const std::vector<std::vector<float>>& map2
) {
    int height = static_cast<int>(map1.size());
    int width = static_cast<int>(map1[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = std::min(map1[y][x], map2[y][x]);
        }
    }
    
    return result;
}

// ============================================================================
// Utility Functions
// ============================================================================

std::vector<std::vector<float>> apply_function(
    const std::vector<std::vector<float>>& map,
    std::function<float(float)> func
) {
    int height = static_cast<int>(map.size());
    int width = static_cast<int>(map[0].size());
    
    std::vector<std::vector<float>> result(height, std::vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result[y][x] = func(map[y][x]);
        }
    }
    
    return result;
}

std::vector<std::vector<float>> invert(
    const std::vector<std::vector<float>>& map
) {
    return apply_function(map, [](float x) { return 1.0f - x; });
}

MapStats calculate_stats(const std::vector<std::vector<float>>& map) {
    MapStats stats;
    
    if (map.empty() || map[0].empty()) {
        stats.min = stats.max = stats.mean = stats.stddev = 0.0f;
        return stats;
    }
    
    stats.min = map[0][0];
    stats.max = map[0][0];
    float sum = 0.0f;
    int count = 0;
    
    for (const auto& row : map) {
        for (float val : row) {
            stats.min = std::min(stats.min, val);
            stats.max = std::max(stats.max, val);
            sum += val;
            count++;
        }
    }
    
    stats.mean = sum / count;
    
    // Calculate standard deviation
    float variance = 0.0f;
    for (const auto& row : map) {
        for (float val : row) {
            float diff = val - stats.mean;
            variance += diff * diff;
        }
    }
    
    stats.stddev = std::sqrt(variance / count);
    
    return stats;
}

std::vector<std::vector<float>> copy_map(
    const std::vector<std::vector<float>>& map
) {
    return map;
}

} // namespace Noise
