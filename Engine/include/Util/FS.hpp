#pragma once

#include <vector>
#include <fstream>

namespace Engine::Util::FS {
    inline std::vector<char> ReadAllBytes(const char* filename)
    {
        using namespace std;

        ifstream stream(filename, ios::binary | ios::ate);
        auto sz = stream.tellg();

        std::vector<char> ret(sz);

        stream.seekg(0, ios::beg);
        stream.read(ret.data(), sz);

        return ret;
    }
}