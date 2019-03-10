#ifndef RESOURCES_CONSTANT_TABLE_H_
#define RESOURCES_CONSTANT_TABLE_H_

#include <map>
#include <string>

struct ShaderResource
{
    std::uint32_t set;
    std::uint32_t binding;
    vk::ShaderStageFlags stage;
    vk::DescriptorType type;
};

struct ShaderConstant
{
    std::size_t offset;
    std::size_t size;
};

struct ConstantTable
    : public ShaderResource
{
    std::map<std::string, ShaderConstant> constants;
    std::size_t size;
};

#endif // RESOURCES_CONSTANT_TABLE_H_
