#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

#include "geometry/ray.hpp"
#include "geometry/sphere.hpp"
#include "geometry/triangle.hpp"
#include "integrator/path.hpp"
#include "scenes/cornell.hpp"
#include "utils/scene.hpp"
#include "utils/vec3.hpp"

const int h = 400;
const int w = h;
const int f = h;

const int samples = 128;

int main() {
    Scene scene = Cornell::scene();
    std::array<Vec3, w * h> image;
    Vec3 camera{0, 0, -3};

    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int s = 0; s < samples; s++) {
                int pixel = (h - y - 1) * w + x;
                Vec3 direction = Vec3{x - w / 2.f, y - h / 2.f, f}.norm();
                image[pixel] = image[pixel] + Path::radiance(Ray(camera, direction), scene) * (1.f / samples);
            }
        }
    }

    std::FILE *file = std::fopen("../res.ppm", "w");
    std::fprintf(file, "P3\n%d %d\n255\n", w, h);

    auto format = [](float v) {
        return static_cast<int>(std::pow(std::clamp(v, 0.f, 1.f), 1 / 2.2) * 255 + 0.5f);
    };

    for (int pixel = 0; pixel < w * h; pixel++) {
        Vec3 color = image[pixel];
        std::fprintf(file, "%d\n%d\n%d\n", format(color.x), format(color.y), format(color.z));
    }

    return 0;
}
