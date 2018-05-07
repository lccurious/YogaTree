//
// Created by peo on 17-12-20.
//

#pragma once

#include "LeafNode.h"
#include "RootNode.h"
#include "NodeUnion.h"
#include "Mask.h"


template <typename _ChildNodeType, Index Log2Dim>
class InternalNode {
public:
    using ChildNodeType = _ChildNodeType;
    using LeafNodeType = typename ChildNodeType::LeafNodeType;
    using ValueType = typename ChildNodeType::ValueType;
    using BuildType = typename ChildNodeType::BuildType;
    using UnionType = NodeUnion<ValueType, ChildNodeType>;
    using SiblingInternalNodeType = InternalNode<LeafNodeType, Log2Dim>;

	static const ValueType EmptyValue = ChildNodeType::EmptyValue;
	static const ValueType DenseValue = ChildNodeType::DenseValue;

    /// Borrow from OpenVDB
    static const Index
        LOG2DIM     = Log2Dim,
        TOTAL       = Log2Dim + ChildNodeType::TOTAL,
        DIM         = 1 << TOTAL,
        NUM_VALUES  = 1 << (3 * Log2Dim),
        LEVEL       = 1 + ChildNodeType::LEVEL;
    static const Index64
        NUM_VOXELS = uint64_t(1) << (3 * TOTAL);

    InternalNode();
    ~InternalNode();

    // explicit InternalNode(const ValueType& offValue);

    InternalNode(const Coord& origin, const ValueType& fillValue);

    /// Copy constructor
    ///
    /// @author Peo
    /// @date 2018/4/15
    ///
    /// @param parameter1 The first parameter.
    InternalNode(const InternalNode&);

    /// Adds a leaf
    ///
    /// @author Peo
    /// @date 2018/4/15
    ///
    /// @param [in,out] leaf If non-null, the leaf.
    void addLeaf(LeafNodeType* leaf);

    /// Adds an internal node
    ///
    /// @author Peo
    /// @date 2018/4/15
    ///
    /// @param [in,out] SiblingNode If non-null, the sibling node.
    void addInternalNode(SiblingInternalNodeType* SiblingNode);

    /// Return the linear table offset of the given global or local coordinates.
    static Index coordToOffset(const Coord& xyz);

    /// Offset to local coordinate
    ///
    /// @author Peo
    /// @date 2018/4/15
    ///
    /// @param 		    n   An Index to process.
    /// @param [in,out] xyz The xyz.
    static void offsetToLocalCoord(Index n, Coord& xyz);

    int Light(Coord xyz);

    void setChildNode(Index i, ChildNodeType* child);
    void resetChildNode(Index i, ChildNodeType* child);

    const Coord& origin() const { return mOrigin; }

    Index32 leafCount() const;
	bool isDense() { return ISDENSE; }

	bool deAllocate() {
		std::cout << "InternalNode: ["
			<< mOrigin[0] << "->" << (mOrigin[0] + DIM) << ", "
			<< mOrigin[1] << "->" << (mOrigin[1] + DIM) << ", "
			<< mOrigin[2] << "->" << (mOrigin[2] + DIM) << "]"
			<< "\t has been deleted" << std::endl;

		if (mNodes != nullptr) {
			delete[] mNodes;
			mNodes = nullptr;
			return true;
		}
		return false;
	}

	ValueType fetchValue(Coord coord) {
		Index32 n = coordToOffset(coord);
		if (mNodes[n].getChild() != nullptr) {
			if (mNodes[n].getChild()->isDense()) {
				return DenseValue;
			}
			return mNodes[n].getChild()->fetchValue(coord);
		}
		return EmptyValue;
	}

	/// check the node dense status and subsitude the dense node with constant
	int reform();

private:
    int onVoxelNum = 0;
    int offVoxelNum = 0;
    int voxelNum = 0;
    Index32 onLeafCount = 0;
	Index32 DenseNum = 0;
	// No need to change because voxel represented by bool
	ValueType LIGHT_VALUE = 1;

	bool ISDENSE = false;

    UnionType* mNodes;
    Coord mOrigin;

    /*union   InternalData    {
        ChildNodeType* child = nullptr;
        ValueType      valueFill;
    }   mInternalDAT[NUM_VALUES];*/
    SiblingInternalNodeType* SiblingInternalNodePtr = nullptr;

};

template<typename ChildT, Index Log2Dim>
inline void
InternalNode<ChildT, Log2Dim>::setChildNode(Index i, ChildNodeType *child)
{
    assert(child);
    mNodes[i].setChild(child);
}

template<typename ChildT, Index Log2Dim>
inline void
InternalNode<ChildT, Log2Dim>::resetChildNode(Index i, ChildNodeType *child)
{
    assert(child);
    delete mNodes[i].getChild();
    mNodes[i].setChild(child);
}

/* No Longer use it ***
 *template<typename ChildT, Index Log2Dim>
 *void InternalNode<ChildT, Log2Dim>::addLeafNodeNotFull(
 *        LeafNodeType *inNode) {
 *    int cmpKey = inNode->frontKey();
 *    int insertLoc = 1;
 *
 *    // *[]*[]*[]*[]
 *    // here must add some limitation
 *    while (mInternalDAT[insertLoc].valueFill < cmpKey &&
 *            insertLoc < voxelNum*2) {
 *        insertLoc+=2;
 *    }
 *
 *    for (int j = voxelNum*2; j > insertLoc; j-=2) {
 *        mInternalDAT[j].child = mInternalDAT[j-2].child;
 *        mInternalDAT[j+1].valueFill = mInternalDAT[j-1].valueFill;
 *    }
 *
 *    mInternalDAT[insertLoc].valueFill = cmpKey;
 *    mInternalDAT[insertLoc-1].child = inNode;
 *    voxelNum++;
 *
 *}
 */

template<typename _ChildNodeType, Index Log2Dim>
inline InternalNode<_ChildNodeType, Log2Dim>::InternalNode(const InternalNode &) : mNodes(new UnionType[NUM_VALUES])
{
}

/* No longer use it
 *template<typename ChildT, Index Log2Dim>
 *void InternalNode<ChildT, Log2Dim>::addLeafNode(
 *        LeafNodeType *inNode) {
 *    if (voxelNum*2 < NUM_VALUES) {
 *        addLeafNodeNotFull(inNode);
 *    }
 *    else
 *    {
 *        // TODO: split the InternalNode, add pointer to the new  *InternalNode
 *
 *        std::cerr << "Storage not enough" << std::endl;
 *        exit(1);
 *    }
 *}
 */

template<typename _ChildNodeType, Index Log2Dim>
InternalNode<_ChildNodeType, Log2Dim>::InternalNode(): mNodes(new UnionType[NUM_VALUES]) {

}

template<typename _ChildNodeType, Index Log2Dim>
inline InternalNode<_ChildNodeType, Log2Dim>::~InternalNode()
{
	if (mNodes != nullptr) {
		for (Index i = 0; i < NUM_VALUES; ++i) {
			delete mNodes[i].getChild();
		}
		delete[] mNodes;
	}
}

template<typename _ChildNodeType, Index Log2Dim>
void InternalNode<_ChildNodeType, Log2Dim>::addInternalNode(
        InternalNode::SiblingInternalNodeType *SiblingNode) {
    SiblingInternalNodePtr = SiblingNode;
}

template<typename _ChildNodeType, Index Log2Dim>
InternalNode<_ChildNodeType, Log2Dim>
::InternalNode(const Coord &origin, const ValueType &fillValue): 
	mOrigin(origin[0] & ~(DIM-1), origin[1] & ~(DIM-1), origin[2] & ~(DIM-1)),
	mNodes(new UnionType[NUM_VALUES])
{
	for (Index i = 0; i < NUM_VALUES; ++i) {
		mNodes[i].setValue(fillValue);
	}
}

template<typename ChildT, Index Log2Dim>
inline Index
InternalNode<ChildT, Log2Dim>::coordToOffset(const Coord& xyz)
{
    return (((xyz[2] & (DIM-1u)) >> ChildNodeType::TOTAL) << 2*Log2Dim)
        +  (((xyz[1] & (DIM-1u)) >> ChildNodeType::TOTAL) <<   Log2Dim)
        +   ((xyz[0] & (DIM-1u)) >> ChildNodeType::TOTAL);
}

template<typename ChildT, Index Log2Dim>
inline void
InternalNode<ChildT, Log2Dim>::offsetToLocalCoord(Index n, Coord &xyz)
{
    assert(n<(1<<3*Log2Dim));
    xyz.setX(n >> 2*Log2Dim);
    n &= ((1 << 2*Log2Dim) - 1);
    xyz.setY(n >> Log2Dim);
    xyz.setZ(n & ((1 << Log2Dim) - 1));
}

template<typename ChildT, Index Log2Dim>
inline int InternalNode<ChildT, Log2Dim>::Light(Coord xyz)
{
	int deltaVoxelNum = 0;
	const Index n = this->coordToOffset(xyz);
	ChildT* child = mNodes[n].getChild();
	if (child != nullptr) {
		deltaVoxelNum = child->Light(xyz);
		if (deltaVoxelNum) {
			onVoxelNum += deltaVoxelNum;
			return deltaVoxelNum;
		}
	} 
	else 
	{
		child = new ChildT(xyz, mNodes[n].getValue());
		onLeafCount++;
		this->setChildNode(n, child);
		// Add LeafNode Light return update local onvoxelNum
		deltaVoxelNum = child->Light(xyz);
		if (deltaVoxelNum) {
			onVoxelNum += deltaVoxelNum;
			return deltaVoxelNum;
		}
	}
	return 0;
}

template<typename ChildT, Index Log2Dim>
inline void
InternalNode<ChildT, Log2Dim>::addLeaf(LeafNodeType *leaf)
{
    assert(leaf != nullptr);
    const Coord& xyz = leaf->origin();
    const Index n = this->coordToOffset(xyz);
    ChildT* child = nullptr;
    if (ChildT::LEVEL > 0 ) {
        child = new ChildT(xyz, mNodes[n].getValue());
    } else {
        child = reinterpret_cast<ChildT*>(leaf);
    }
    this->setChildNode(n, child);
    onLeafCount ++;

    child->addLeaf(leaf);
}

template<typename ChildT, Index Log2Dim>
inline Index32
InternalNode<ChildT, Log2Dim>::leafCount() const
{
    return onLeafCount;
}

template<typename _ChildNodeType, Index Log2Dim>
int InternalNode<_ChildNodeType, Log2Dim>::reform()
{
	for (int i = 0; i < NUM_VALUES; i++) {
		if (mNodes[i].getChild() == nullptr) {
			continue;
		}
		if (mNodes[i].getChild()->isDense()) {
			// TODO:delete the ChildNode And replace with constant.
			// mNodes[i].getChild()->printNode();
			mNodes[i].getChild()->deAllocate();
			delete mNodes[i].getChild();
			mNodes[i].setChild(nullptr);
			// mNodes[i].setValue(LIGHT_VALUE);
			
			DenseNum++;
			
			/*
			std::cout << "Node: ["
				<< mOrigin.x() << ", "
				<< mOrigin.y() << ", "
				<< mOrigin.z() << "]: "
				<< DenseNum << "\tHas been deleted"
				<< std::endl;
			*/
		}
	}
	if (DenseNum == NUM_VALUES) {
		ISDENSE = true;
	}
	return 0;
}