#ifndef RESOURCES_CONSTANT_TABLE_H_
#define RESOURCES_CONSTANT_TABLE_H_

#include <map>
#include <string>


struct ShaderConstant
{
    std::size_t offset;
    std::size_t size;
};

struct ConstantTable
{
    void Merge(const ConstantTable &source);

    std::map<std::string, ShaderConstant> constants;
    std::size_t table_size;
    std::uint32_t set;
    std::uint32_t binding;
};

#endif // RESOURCES_CONSTANT_TABLE_H_
