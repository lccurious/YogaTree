//
// Created by peo on 17-12-28.
//

#pragma once

#include <string>
#include <iostream>
#include <vector>

typedef std::string Name;

inline Name readString(std::istream& is)
{
    uint32_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    std::string buffer(size, ' ');
    if (size>0) is.read(&buffer[0], size);
    return buffer;
}

inline void
writeString(std::ostream& os, const Name& name)
{
    uint32_t size = uint32_t(name.size());
    os.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    os.write(&name[0], size);
}
