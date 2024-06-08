#include "Input/Input.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/NativeScript.h"
#include "Scene/Scene.h"
#include "Utils/Log.h"

#include <glm/fwd.hpp>

#include <algorithm>
#include <glm/geometric.hpp>
#include <iostream>
#include <linux/limits.h>
#include <memory>
#include <random>
#include <vector>

namespace {

static constexpr int repeat = 256;

double fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double grad(int hash, double x, double y, double z)
{
    switch (hash & 0xF) {
    case 0x0:
        return x + y;
    case 0x1:
        return -x + y;
    case 0x2:
        return x - y;
    case 0x3:
        return -x - y;
    case 0x4:
        return x + z;
    case 0x5:
        return -x + z;
    case 0x6:
        return x - z;
    case 0x7:
        return -x - z;
    case 0x8:
        return y + z;
    case 0x9:
        return -y + z;
    case 0xA:
        return y - z;
    case 0xB:
        return -y - z;
    case 0xC:
        return y + x;
    case 0xD:
        return -y + z;
    case 0xE:
        return y - x;
    case 0xF:
        return -y - z;
    default:
        return 0; // never happens
    }
}

double lerp(double a, double b, double x)
{
    return a + x * (b - a);
}

int inc(int num)
{
    num++;
    if (repeat > 0)
        num %= repeat;

    return num;
}

// https://adrianb.io/2014/08/09/perlinnoise.html
double perlin(double x, double y, double z)
{
    static constexpr std::array p = { 151, 160, 137, 91, 90, 15, // Hash lookup table as defined by Ken Perlin.  This is a randomly
        131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, // arranged array of all numbers from 0-255 inclusive.
        190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
        88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
        77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
        102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
        135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
        5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
        223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
        129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
        251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
        49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
        138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
        151, 160, 137, 91, 90, 15,
        131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
        190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
        88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
        77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
        102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
        135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
        5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
        223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
        129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
        251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
        49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
        138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

    x = std::fmod(x, repeat);
    y = std::fmod(y, repeat);
    z = std::fmod(z, repeat);

    int xi = (int)x & 255; // Calculate the "unit cube" that the point asked will be located in
    int yi = (int)y & 255; // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = (int)z & 255; // The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    double xf = x - double(int(x));
    double yf = y - double(int(y));
    double zf = z - double(int(z));

    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    const auto aaa = p[p[p[xi] + yi] + zi];
    const auto aba = p[p[p[xi] + inc(yi)] + zi];
    const auto aab = p[p[p[xi] + yi] + inc(zi)];
    const auto abb = p[p[p[xi] + inc(yi)] + inc(zi)];
    const auto baa = p[p[p[inc(xi)] + yi] + zi];
    const auto bba = p[p[p[inc(xi)] + inc(yi)] + zi];
    const auto bab = p[p[p[inc(xi)] + yi] + inc(zi)];
    const auto bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

    auto x1 = lerp(grad(aaa, xf, yf, zf), // The gradient function calculates the dot product between a pseudorandom
        grad(baa, xf - 1, yf, zf), // gradient vector and the vector from the input coordinate to the 8
        u); // surrounding points in its unit cube.
    auto x2 = lerp(grad(aba, xf, yf - 1, zf), // This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
        grad(bba, xf - 1, yf - 1, zf), // values we made earlier.
        u);
    const auto y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1),
        grad(bab, xf - 1, yf, zf - 1),
        u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
        grad(bbb, xf - 1, yf - 1, zf - 1),
        u);
    const auto y2 = lerp(x1, x2, v);

    return (lerp(y1, y2, w) + 1) / 2;
}

double OctavePerlin(double x, double y, double z, int octaves, double persistence)
{
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0; // Used for normalizing result to 0.0 - 1.0
    for (int i = 0; i < octaves; i++) {
        total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}
}

class Terrain : public PEANUT::NativeScript {
public:
    double width = 10.0;
    double height = 10.0;
    int width_nodes = 10;
    int height_nodes = 10;
    int octaves = 1;
    double persistence = 1.0;
    bool regenerate = false;

    Terrain()
    {
        m_members.emplace_back(PEANUT::MemberVariable { "width", PEANUT::MemberVariable::Type::Double, &width });
        m_members.emplace_back(PEANUT::MemberVariable { "height", PEANUT::MemberVariable::Type::Double, &height });
        m_members.emplace_back(PEANUT::MemberVariable { "width_nodes", PEANUT::MemberVariable::Type::Int, &width_nodes });
        m_members.emplace_back(PEANUT::MemberVariable { "height_nodes", PEANUT::MemberVariable::Type::Int, &height_nodes });
        m_members.emplace_back(PEANUT::MemberVariable { "octaves", PEANUT::MemberVariable::Type::Int, &octaves });
        m_members.emplace_back(PEANUT::MemberVariable { "persistence", PEANUT::MemberVariable::Type::Double, &persistence });
        m_members.emplace_back(PEANUT::MemberVariable { "regenerate", PEANUT::MemberVariable::Type::Bool, &regenerate });
    }

    Terrain(const Terrain& other) = delete;

    void OnCreate() override
    {
        Regenerate();
    }

    void EditorUpdate() override
    {
        if (regenerate) {
            Regenerate();
            regenerate = false;
        }
    }

private:
    std::vector<std::vector<double>> height_map;

    void Regenerate()
    {
        LOG_DEBUG("Generating terrain");

        using namespace PEANUT;
        auto& model = m_ent.Has<CustomModelComponent>() ? m_ent.Get<CustomModelComponent>() : m_ent.Add<CustomModelComponent>();
        model.mesh.indices.clear();
        model.mesh.vertices.clear();

        height_map.clear();
        for (int i = 0; i < width_nodes; ++i) {
            height_map.emplace_back();
            for (int j = 0; j < height_nodes; ++j) {
                height_map[i].emplace_back(OctavePerlin(double(i) / double(width_nodes) * width, double(j) / double(height_nodes) * height, 0.0, octaves, persistence));
            }
        }

        for (int i = 0; i < (width_nodes - 1); ++i) {
            for (int j = 0; j < (height_nodes - 1); ++j) {
                const glm::vec3 point_1 = {
                    static_cast<double>(i) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j) / static_cast<double>(height_nodes) * height,
                    height_map[i][j]
                };
                const glm::vec3 point_2 = {
                    static_cast<double>(i + 1) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j) / static_cast<double>(height_nodes) * height,
                    height_map[i + 1][j]
                };
                const glm::vec3 point_3 = {
                    static_cast<double>(i + 1) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j + 1) / static_cast<double>(height_nodes) * height,
                    height_map[i + 1][j + 1]
                };
                const glm::vec3 point_4 = {
                    static_cast<double>(i) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j + 1) / static_cast<double>(height_nodes) * height,
                    height_map[i][j + 1]
                };

                const auto normal_1 = glm::normalize(glm::cross(glm::vec3 { point_2 - point_1 }, glm::vec3 { point_3 - point_1 }));
                const auto normal_2 = glm::normalize(glm::cross(glm::vec3 { point_4 - point_3 }, glm::vec3 { point_1 - point_3 }));

                auto& mesh = m_ent.Get<PEANUT::CustomModelComponent>().mesh;
                mesh.vertices.emplace_back(PEANUT::Vertex { point_1, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_2, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_3, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_3, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_4, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_1, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
            }
        }
    }
};

extern "C" const char* plugin_name()
{
    return "Terrain";
}

extern "C" PEANUT::NativeScript* get_new_component()
{
    LOG_DEBUG("Creating new TestComponent");
    return new Terrain {};
}
