//
// Created by peo on 17-12-20.
//

#pragma once

#include <limits>
#include <memory>
#include <assert.h>
#include "Coord.h"
#include "Types.h"

/// This LeafNode only indicate the voxel binary state, Only light or dark
/// \tparam T
/// \tparam Log2X
/// \tparam Log2Y
/// \tparam Log2Z
template <typename T, Index Log2Dim=3>
class LeafNode {
public:
    using BuildType = T;
    using ValueType = T;
    using LeafNodeType = LeafNode<ValueType, Log2Dim>;
    using Ptr = SharedPtr<LeafNode>;

	static const ValueType EmptyValue = ValueType(0);
	static const ValueType DenseValue = ValueType(1);

    static const Index
        LOG2DIM     = Log2Dim,
        TOTAL       = Log2Dim,
        DIM         = 1 << TOTAL,
        NUM_VALUES  = 1 << 3 * Log2Dim,
        NUM_VOXELS  = NUM_VALUES,
        SIZE        = NUM_VALUES,
        LEVEL       = 0;

    LeafNode(Coord coord) { mOrigin[0] = coord[0];
                 mOrigin[1] = coord[1];
                 mOrigin[2] = coord[2];
               }

    LeafNode() { mOrigin[0] = 0;
                 mOrigin[1] = 0;
                 mOrigin[2] = 0;
               }

    LeafNode(T allocatedKey) {
        mainKey = allocatedKey;
    }

    explicit LeafNode(const Coord& coords, const ValueType& value);

	~LeafNode() { delete[] voxelStats; }

	bool deAllocate() {
		/*
		std::cout << "LeafNode: ["
			<< mOrigin[0] << "->" << (mOrigin[0] + DIM) <<", "
			<< mOrigin[1] << "->" << (mOrigin[1] + DIM) << ", "
			<< mOrigin[2] << "->" << (mOrigin[2] + DIM) << "]"
			<< "\t has been deleted" << std::endl;
		*/

		if (voxelStats != nullptr) {
			delete[] voxelStats;
			voxelStats = nullptr;
			return true;
		}
		return false;
	}

    //
    // Statistics
    //
    /// Return log2 of the dimension of this LeafNode, e.g. 3 if dimensions are 8^3
    static Index log2dim() { return Log2Dim; }
    /// Return the number of voxels in each coordinate dimension.
    static Index dim() { return DIM; }
    /// Return the total number of voxels represented by this LeafNode
    static Index size() { return SIZE; }
    /// Return the total number of voxels represented by this LeafNode
    static Index numValues() { return SIZE; }
    /// Return the level of this node, which by definition is zero for LeafNodes
    static Index getLevel() { return LEVEL; }
    /// Return the dimension of child nodes of this LeafNode, which is one for voxels.
    static Index getChildDim() { return 1; }
    /// Return the leaf count for this node, which is one.
    static Index32 leafCount() { return 1; }
    /// Return the non-leaf count for this node, which is zero.
    static Index32 nonLeafCount() { return 0; }

    // return the first key, keep it for generation
    T frontKey() const { return mainKey; }

	void printNode() {
		for (int i = 0; i < voxelStatsSize; i++) {
			printf("%X\t", voxelStats[i]);
		}
		printf("\n");
	}

    /// Light the voxel in key location
    /// \param key the offset transformed from the local coordinate.
    int Light(const Coord xyz) {
        Index lightOffset = coordToOffset(xyz);
        uint8_t xAxis = (xyz.x() & ((1u << sLog2X) - 1u));

		/*
		std::cout << "Node: ["
			<< xyz.x() << ", "
			<< xyz.y() << ", "
			<< xyz.z() << "]: ";
		*/

        if (!(voxelStats[(lightOffset>>sLog2X)] & (1u<<xAxis))) {
            voxelStats[(lightOffset>>sLog2X)] |= (1u<<xAxis);
            onVoxelNum++;

			// printf("+voxelStatus[%d : %d]:\t%X\n", (lightOffset >> sLog2X), xAxis, voxelStats[(lightOffset >> sLog2X)]);

			return 1;
        }
		/*
		else
		{
			// printf("-voxelStatus[%d : %d]:\t%X\n", (lightOffset >> sLog2X), xAxis, voxelStats[(lightOffset >> sLog2X)]);
		}
		*/
		return 0;
    }

    /// Dark the voxel in key location
    /// \param key the offset transformed from the local coordinate.
    void Dark(Coord xyz) {
        Index lightOffset = coordToOffset(xyz);
        uint8_t xAxis = (xyz.x() & ((1u << sLog2X) - 1u));
        if(voxelStats[(lightOffset>>sLog2X)] & (1u<<xAxis)) {
           voxelStats[(lightOffset>>sLog2X)] &= ~(1u<<xAxis);
           onVoxelNum--;
		   return 1;
        }
		return 0;
    }

    /// If the LeafNode here is Dense then Internal mark Cube here with only
    /// one value and this Leaf could be delete
    /// \return isDense;
    bool isDense() { 
		/*
		std::cout << "Node: ["
			<< mOrigin.x() << ", "
			<< mOrigin.y() << ", "
			<< mOrigin.z() << "]: "
			<< onVoxelNum << " Voxel On"
			<< std::endl;
		*/
		return onVoxelNum == sSize; 
	}

	ValueType fetchValue(Coord coord) {
		Index fetchOffset = coordToOffset(coord);
		uint8 xAxis = (coord.x() & ((1u << sLog2X) - 1u));

		if (voxelStats[(fetchOffset >> sLog2X)] & (1u << xAxis)) {
			return DenseValue;
		}
		return EmptyValue;
	}

    /// It the LeafNode here is Empty then subsititude pointer in InternalNode
    /// with one background value
    bool isEmpty() { return onVoxelNum == 0; }

    /// Set the grid index coordinates of this node's local origin.
    void setOrigin(const Coord& origin) { mOrigin = origin; }
    //@{
    /// Return the grid index coordinates of this node's local origin.
    const Coord& origin() const { return mOrigin; }
    void getOrigin(Coord& origin) const { origin = mOrigin; }
    void getOrigin(Int32& x, Int32& y, Int32& z) const { mOrigin.asXYZ(x, y, z); }
    //@}

    //{@
    void addLeaf(LeafNode*) {}
    //@}

	/// no reform need in LeafNode, It can be delete and subditude with
	/// parent node
	int reform();

protected:
    ///
    /// \brief mOrigin is global index of this Leaf
    ///
    Coord mOrigin;

private:

    unsigned int onVoxelNum = 0;
    unsigned int offset;
    T mainKey;
    LeafNodeType* SiblingPtrUp = nullptr;

	bool CLEARED;

    /// transform local coordinate to offset
    /// \param x
    /// \param y
    /// \param z
    /// \return
    static Index coordToOffset(const Coord& xyz);

    static Coord offsetToLocalCoord(Index n);

    Coord offsetToGlobalCoord(Index n) const;

    static const int sLog2X = Log2Dim,
            sLog2Y=Log2Dim,
            sLog2Z=Log2Dim,
			voxelStatsSize = 1 << (sLog2Z + sLog2Y),
            sSize = 1 << (sLog2X+sLog2Y+sLog2Z);

    // indicate the voxel cube state
    uint8_t* voxelStats;
};

template<typename T, Index Log2Dim>
inline LeafNode<T, Log2Dim>::LeafNode(const Coord & coord, const ValueType & value) 
	:mOrigin(coord[0] & ~(DIM-1), coord[1] & ~(DIM-1), coord[2] & ~(DIM-1)), 
	voxelStats(new uint8_t[voxelStatsSize])
{
	for (int i = 0; i < voxelStatsSize; ++i) {
		voxelStats[i] = 0;
	}
}

template<typename T, Index Log2Dim>
int LeafNode<T, Log2Dim>::reform()
{
	return 0;
}

template<typename T, Index Log2Dim>
inline Index
LeafNode<T, Log2Dim>::coordToOffset(const Coord& xyz)
{
    assert ((xyz[0] & (DIM-1u)) < DIM && (xyz[1] & (DIM-1u)) < DIM && (xyz[2] & (DIM-1u)) < DIM);
    return ((xyz[2] & (DIM-1u)) << 2*Log2Dim)
         + ((xyz[1] & (DIM-1u)) << Log2Dim)
         +  (xyz[0] & (DIM-1u));
}

template<typename T, Index Log2Dim>
inline Coord LeafNode<T, Log2Dim>::offsetToLocalCoord(Index n)
{
	assert(n < sSize);
	Index z = (n >> LOG2DIM*2)	& ((1 << LOG2DIM) - 1);
	Index y = (n >> LOG2DIM)	& ((1 << LOG2DIM) - 1);
	Index x = (n)				& ((1 << LOG2DIM) - 1);
	return Coord(x, y, z);
}

template<typename T, Index Log2Dim>
inline Coord
LeafNode<T, Log2Dim>::offsetToGlobalCoord(Index n) const
{
    assert(n < (1 << 3*Log2Dim));
    Coord xyz;
    xyz.setX(n >> 2*Log2Dim);
    n &= ((1 << 2*Log2Dim) - 1);
    xyz.setY(n >> Log2Dim);
    xyz.setZ(n & ((1 << Log2Dim) - 1));
    return xyz;
}
