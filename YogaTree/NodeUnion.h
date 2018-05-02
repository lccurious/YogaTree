//
// Created by peo on 18-1-6.
//

#pragma once

#include "Types.h"
#include <cstring>

// Implementation that stores the child pointer and the value separately
// (i.e., not in a union)
// This implementation is not used for POD
template<typename ValueT, typename ChildT, typename Enable = void>
class NodeUnion {
private:
    union { ChildT* mChild; ValueT mValue; };

public:
    NodeUnion(): mChild(nullptr) {}
    NodeUnion(const NodeUnion& other): mChild(nullptr)
    { std::memcpy(this, &other, sizeod(*this)); }

    ChildT* getChild() const { return mChild; }
    void setChild(ChildT* child) { mChild = child; }

    const ValueT& getValue() const { return mValue; }
    ValueT& getValue() { return mValue; }
    void setValue(const ValueT& val) { mValue = val; }
};

