#ifndef RESOURCES_CONSTANT_TABLE_H_
#define RESOURCES_CONSTANT_TABLE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "backend/streams.h"


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
    bool IsEqual(const ConstantTable &reference) const;

    std::map<std::string, ShaderConstant> members;
    std::size_t size = 0;
    std::vector<std::shared_ptr<StreamBuffer>> buffers;

    std::uint32_t crc = 0; ///< table members CRC for equality check
};

#endif // RESOURCES_CONSTANT_TABLE_H_
