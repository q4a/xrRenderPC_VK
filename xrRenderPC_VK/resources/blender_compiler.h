#ifndef RESOURCES_BLENDER_COMPILER_H_
#define RESOURCES_BLENDER_COMPILER_H_

#include <memory>
#include <string>
#include <vector>

#include "resources/blender.h"
#include "resources/element.h"
#include "resources/shader_traits.h"

class BlenderCompiler
{
public:
    void Compile(Element &element);
    void PassBegin(const std::string &vertex_shader, const std::string &fragment_shader);
    void PassTexture(const std::string &name, const std::string &texture);
    void PassSampler(const std::string &name) {}
    void PassZtest(bool ztest, bool zwrite) {}
    void PassBlend(bool alpha, u32 source, u32 destination, bool test, u32 key) {}
    void PassLightingFog(bool lighting, bool fog) {}
    void PassEnd();

    std::vector<std::string> textures;
    std::vector<std::string> constants;
    std::vector<std::string> matrices;

    // pass
    std::shared_ptr<VertexShader> vs;
    std::shared_ptr<FragmentShader> fs;

    std::shared_ptr<Blender> blender;
    bool detail = false;
};

#endif // RESOURCES_BLENDER_COMPILER_H_
