#include "Noise.hpp"

int main() {
    using namespace Noise;
    
	//White noise test image
    create_whitenoise(256, 256, 21, "image", "whiteNoise.png");
    
    //Perlin noise test image 
	create_perlinnoise(512, 512, 50.0f, 6, 1.0f, 0.5f, 2.0f, 0.0f, 42, "image", "perlinNoise.png");
    create_perlinnoise(512, 512, 75.0f, 3, 1.0f, 0.5f, 1.5f, 0.2f, 89, "image", "perlin_noise.jpg");

	//Simplex noise test image
	create_simplexnoise(512, 512, 60.0f, 4, 0.5f, 2.0f, 0.0f, 33, "image", "simplexNoise.png");
	create_simplexnoise(512, 512, 60.0f, 2, 1.0f, 1.0f, 0.0f, 52, "image", "simplex_Noise.png");
    return 0;
}
