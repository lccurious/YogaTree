//
// Created by peo on 17-12-21.
//

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

// One-dimensional scalar types
using Index32 = uint32_t;
using Index64 = uint64_t;
using Index   = Index32;
using Int16   = int16_t;
using Int32   = int32_t;
using Int64   = int64_t;
using Int     = Int32;
using Byte    = unsigned char;
using Real    = double;

template<typename T> using SharedPtr = std::shared_ptr<T>;
template<typename T> using WeakPtr = std::weak_ptr<T>;

struct RLEVec3{
    int z, y;
    int x[2];
};


struct NodeMat {
	Index x;
	Index y;
	Index z;
	Index Dim;
};

struct NodeDetail {
	Index x, y, z;
	uint8_t details[64];
};

using NodeMats = std::vector<NodeMat>;
using NodeDetails = std::vector<NodeDetail>;

struct ModelSkeleton {
	NodeMats skeleton;
	NodeDetails details;
};

using RLEVec3Array = typename std::vector<RLEVec3>;
