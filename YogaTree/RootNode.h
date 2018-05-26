//
// Created by peo on 17-12-20.
//

#pragma once

#include <map>
#include <set>
#include "InternalNode.h"
#include "LeafNode.h"
#include "Coord.h"
#include "Types.h"

using std::map;

template <typename ChildType>
class RootNode {
public:
    using ChildNodeType = ChildType;
    using LeafNodeType = typename ChildType::LeafNodeType;
    // Binary voxel tree only use bool
    using ValueType = Byte;
    using Tile = ValueType;
    using BuildType = typename ChildType::BuildType;
	static const ValueType EmptyValue = ChildType::EmptyValue;
	static const ValueType DenseValue = ChildType::DenseValue;

    static const Index LEVEL = 1 + ChildType::LEVEL;

    /// initialize the tree with zero type;
    RootNode();
    ~RootNode();

    struct RootData {
        ChildType*  child;
        /// Let make tile be the number of this voxel this tile contain
        Tile   tile;

        RootData(): child(nullptr) {}
        RootData(ChildType& c): child(&c) {}
        RootData(const ValueType& t): child(nullptr), tile(t) {}
        RootData(const RootData&) = default;
        RootData& operator=(const RootData&) = default;
        ~RootData() {}

        bool isChild() const { return child != nullptr; }
        bool isTile() const { return child != nullptr; }
        bool isTileOff() const { return isTile() && !tile; }
        bool isTileOn() const { return isTile() && tile; }

        void set(ChildType& c) { delete child; child = &c; }
        void set(const ValueType& t) { delete child; child = nullptr; tile = t; }
        ChildType& steal(const Tile& t) { ChildType* c = child; child = nullptr; tile = t; return *c; }
    };

    using MapType = std::map<Coord, RootData>;
    using MapIter = typename MapType::iterator;
    using MapCIter = typename MapType::const_iterator;

    using CoordSet = std::set<Coord>;
    using CoordSetIter = typename CoordSet::iterator;
    using CoordSetCIter = typename CoordSet::const_iterator;

    static void             setTile(const MapIter& i, const ValueType& t) { i->second.set(t); }
    static void             setChild(const MapIter& i, ChildType& c) { i->second.set(c); }
    static Tile             getTile(const MapIter& i) { return i->second.tile; }
    static const Tile&      getTile(const MapCIter& i) { return i->second.tile; }
    static ChildType&       getChild(const MapIter& i) { return *(i->second.child); }
    static const ChildType& getChild(const MapCIter& i) { return *(i->second.child); }

    static bool isChild(const MapCIter& i) { return i->second.isChild(); }
    static bool isChild(const MapIter& i) { return i->second.isChild(); }

    Index64 onVoxelCount();
	void clear();

    ///
    /// \brief coordToKey
    /// \details Coordinate was represented by
    /// x: 0011000101100110 | so only the heigher coord can be kept
    ///    -------          |
    /// \param xyz
    /// \return

    static Coord coordToKey(const Coord& xyz) { return xyz & ~(ChildType::DIM - 1); }

    /// Inserts the keys described by keys
    ///
    /// @author Peo
    /// @date 2018/5/2
    ///
    /// @param [in,out] keys The keys.

    void insertKeys(CoordSet& keys) const;

    //@{
    ///
    /// \brief findCoord
    /// \param xyz
    /// \return an iterator pointing to the matching mTable entry or to mTable.end()
    ///
    MapIter findCoord(const Coord& xyz) { return mTable.find(coordToKey(xyz)); }
    MapCIter findCoord(const Coord& xyz) const { return mTable.find(coordToKey(xyz)); }
    //@}

    ///
    /// \brief findOrAddCoord
    /// \param xyz
    /// \return
    ///
    MapIter findOrAddCoord(const Coord& xyz);

    /// Adds a leaf, recursive Until a LeafNode reached.
    ///
    /// @author Peo
    /// @date 2018/5/2
    ///
    /// @param [in,out] leaf If non-null, the leaf.

    void addLeaf(LeafNodeType* leaf);

    /// Lights the given coordinate
    ///
    /// @author Peo
    /// @date 2018/5/2
    ///
    /// @param coord The coordinate.

    void Light(Coord coord);

    /// Return the leaf node number create by this tree
    ///
    /// @author Peo
    /// @date 2018/5/2
    ///
    /// @return An Index32.
    
	int saveTo(std::ofstream &outStream);

	void stealNodes(ModelSkeleton& stealArray);

    Index32 leafCount() const;

	ValueType fetchValue(Coord coord) {
		MapCIter fetchCIter;
		fetchCIter = findCoord(coord);
		if (fetchCIter != mTable.end()) {
			if (fetchCIter->second.child->isDense()) {
				return DenseValue;
			}
			return fetchCIter->second.child->fetchValue(coord);
		}
		return EmptyValue;
	}

	void reformRoot();


private:
    unsigned int    voxelNum    = 0;
    Index64			onVoxelNum  = 0;
    unsigned int    offVoxelNum = 0;
	Index64 Numerator = 0;
	Index64 Denominator = 0;

    ValueType mBackground;
    MapType mTable;
}; // end of RootNode class

template<typename ChildT>
inline
RootNode<ChildT>::RootNode() : mBackground(0) {

}

template<typename ChildType>
inline RootNode<ChildType>::~RootNode()
{
	this->clear();
}

template<typename ChildT>
inline void
RootNode<ChildT>::Light(Coord coord)
{
	Index32 deltaVoxelNum = 0;
    MapIter lightIter;
    lightIter = findOrAddCoord(coord);
	ChildT* lChild = nullptr;
	RootData LightField = lightIter->second;
    if (LightField.isChild()) {
		lChild = LightField.child;
		deltaVoxelNum = lChild->Light(coord);
		if (deltaVoxelNum) {
			onVoxelNum += deltaVoxelNum;
			Numerator += coord.x();
			Denominator += deltaVoxelNum;
		}
    } else {
        LeafNodeType* tmpLeafNode = new LeafNodeType(coord, 0);
		deltaVoxelNum = tmpLeafNode->Light(coord);
        addLeaf(tmpLeafNode);
		if (deltaVoxelNum) {
			onVoxelNum += deltaVoxelNum;
			Numerator += coord.x();
			Denominator += deltaVoxelNum;
		}
    }
}

template<typename ChildType>
inline Index64 RootNode<ChildType>::onVoxelCount()
{
	return onVoxelNum;
}

template<typename ChildType>
inline void RootNode<ChildType>::clear()
{
	for (MapIter i = mTable.begin(), e = mTable.end(); i != e; ++i) {
		delete i->second.child;
	}
	mTable.clear();
}

template<typename ChildT>
inline void
RootNode<ChildT>::insertKeys(CoordSet& keys) const
{
    for (MapCIter iter = mTable.begin(), e = mTable.end(); iter != e; ++iter) {
        keys.insert(iter->first);
    }
}

template<typename ChildT>
inline typename RootNode<ChildT>::MapIter
RootNode<ChildT>::findOrAddCoord(const Coord& xyz)
{
    const Coord key = coordToKey(xyz);
    std::pair<MapIter, bool> result = mTable.insert
            (typename MapType::value_type(key, RootData()));
	
	return result.first;
}

template<typename ChildT>
inline void
RootNode<ChildT>::addLeaf(LeafNodeType* leaf)
{
    if (leaf == nullptr) return;
    ChildT* child = nullptr;
    const Coord& xyz = leaf->origin();
    MapIter iter = this->findCoord(xyz);
	Coord insertKey = coordToKey(xyz);
    if (iter == mTable.end()) {
        if (ChildT::LEVEL > 0) {
            child = new ChildT(xyz, mBackground);
        } else {
            child = reinterpret_cast<ChildT*>(leaf);
        }
        mTable[this->coordToKey(xyz)] = RootData(*child);
    } else if (isChild(iter)) {
        if (ChildT::LEVEL>0) {
            child = &getChild(iter);
        } else {
            child = reinterpret_cast<ChildT*>(leaf);
            setChild(iter, *child);
        }
    } else {
		if (ChildT::LEVEL > 0) {
			child = new ChildT(xyz, Byte(0));
		} else { 
			child = reinterpret_cast<ChildT*>(leaf);
        }
		setChild(iter, *child);
    }
    child->addLeaf(leaf);
}

template<typename ChildType>
int RootNode<ChildType>::saveTo(std::ofstream &outStream)
{	
	for (MapIter i = mTable.begin(), e = mTable.end(); i != e; ++i) {
#ifdef _DEBUG
		if (isChild(i)) {
			std::cout << "writing RootNode: [" 
				<< i->first[0] << ", "
				<< i->first[1] << ", "
				<< i->first[2] << "]"
				<< std::endl;
			getChild(i).saveTo(outStream);
		}
#endif // _DEBUG

	}
	return 0;
}

template<typename ChildType>
inline void RootNode<ChildType>::stealNodes(ModelSkeleton & stealArray)
{
	for (MapIter i = mTable.begin(), e = mTable.end(); i != e; ++i) {
		if (isChild(i)) {
#ifdef _DEBUG
			std::cout << "writing RootNode: ["
				<< i->first[0] << ", "
				<< i->first[1] << ", "
				<< i->first[2] << "]"
				<< std::endl;
#endif // _DEBUG
			getChild(i).stealNodes(stealArray);

		}
	}
}

template<typename ChildT>
inline Index32
RootNode<ChildT>::leafCount() const
{
    Index32 sum = 0;
    for (MapCIter i = mTable.begin(), e = mTable.end(); i != e; ++i) {
        if (isChild(i)) sum += getChild(i).leafCount();
    }
    return sum;
}

template<typename ChildType>
void RootNode<ChildType>::reformRoot()
{
	Index64 delInternalNum = 0;
	Index64 delLeafNum = 0;
	Index64 delKeyNum = 0;
	for (MapCIter i = mTable.begin(); i != mTable.end(); i++) {
		// std::cout << i->second.child << std::endl;
		/*
		std::cout << "[" << i->first.x() << ", "
			<< i->first.y() << ", "
			<< i->first.z() << "]" << "\t";
		*/

		if (i->second.child) {
			// std::cout << "Is Dense: " << i->second.child->isDense() ? "True" : "False";
			/*
			std::cout << "Leaf Count: " << i->second.child->leafCount() << std::endl;
			*/
			i->second.child->reform();
#ifdef _DEBUG
			if (i->second.child->isDense()) {
				i->second.child->deAllocate();
				std::cout << "InternalNode has been delete" << std::endl;
			}
#endif // _DEBUG
		}
	}
}

