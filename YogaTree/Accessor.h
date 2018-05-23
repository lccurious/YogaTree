#pragma once
#include "Tree.h"
#include "RootNode.h"
#include "LeafNode.h"
#include "InternalNode.h"

template<typename ChildT>
class Accessor
{
public:
	Accessor() = default;
	~Accessor() = default;
	using LeafNodeT = ChildT::LeafNodeType;

private:

};