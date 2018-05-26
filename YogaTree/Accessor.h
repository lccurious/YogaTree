#pragma once
#include "Tree.h"
#include "RootNode.h"
#include "LeafNode.h"
#include "InternalNode.h"

/// An accessor.
/// This accessor using default configure, the @mOrigin indicates the original 
/// Coordination of the Nodes.
///
/// @author Peo
/// @date 2018/5/24
///
/// @tparam ChildT Type of the child t.

template<typename ChildT>
class Accessor
{
public:
	Accessor() = default;
	~Accessor() = default;
	using LeafNodeT = ChildT::LeafNodeType;

	/// The dump flag, this flag indicates the data output format.
	/// in normal scenes, its impossible to fill ever bit of mDim(which is 64 bits).
	const Index _DUMP_FLAG = 0x8000;

private:
	Coord mOrigin;
	/// The Dim indicates the range of this node.
	Index mDim = 0;
};