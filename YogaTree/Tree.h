//
// Created by peo on 17-12-20.
//

#pragma once

#include "Types.h"
#include "Coord.h"
#include "RootNode.h"
#include "InternalNode.h"
#include "LeafNode.h"
#include <cassert>


namespace tree {

class TreeBase{
public:
    using Ptr = SharedPtr<TreeBase>;
    using ConstPtr = SharedPtr<const TreeBase>;

    TreeBase() = default;
    TreeBase(const TreeBase&) = default;
    TreeBase& operator=(const TreeBase&) = delete; // disallow assignment
    virtual ~TreeBase() = default;

    /// Statistics
    ///
    /// @brief Return the depth of this tree.
    ///
    virtual Index treeDepth() const = 0;


    virtual Index32 leafCount() const = 0;

    //
    // I/O methods
    //
    /// @brief Read the tree topology from a stream
    ///
    /// @todo Add some IO methods
};

template <typename _RootNodeType>
class Tree: public TreeBase {
public:
    using Ptr = SharedPtr<Tree>;
    using ConstPtr = SharedPtr<const Tree>;

    using RootNodeType = _RootNodeType;
    using ValueType = typename RootNodeType::ValueType;
    using BuildType = typename RootNodeType::BuildType;
    using LeafNodeType = typename RootNodeType::LeafNodeType;
	using RootCIter = typename RootNodeType::MapCIter;

    static const Index DEPTH = RootNodeType::LEVEL + 1;

	static const ValueType EmptyValue = RootNodeType::EmptyValue;
	static const ValueType DenseValue = RootNodeType::DenseValue;

    Tree() {}
    ~Tree();

    void addLeaf(LeafNodeType* leaf) { assert(leaf); mRoot.addLeaf(leaf); }

    /// Lights the given coordinate.
    ///
    /// @author Peo
    /// @date 2018/4/14
    ///
    /// @param coord The coordinate.
    void Light(Coord coord) { mRoot.Light(coord); }

	/// Print all coordinate witch were light in the tree
	///
	/// @author Peo
	/// @date 2018/5/2
	///
	/// @return An int.

	int printAllCoord();

	/// Reform tree iterate the whole tree, subsitute the dense node with
	/// a constant value.
	///
	/// @author Peo
	/// @date 2018/5/2

	void reformTree();

	ValueType fetchValue(Coord coord) { return mRoot.fetchValue(coord); }

	int saveTo(const char* filename);

    //
    // Statistics
    //
    Index treeDepth() const override { return DEPTH; }
    Index32 leafCount() const override { return mRoot.leafCount(); }
	Index64 onVoxelCount() const { return mRoot.onVoxelCount(); }

    //
    // Data members
    //
    RootNodeType mRoot;

	//
	// IO compress
	// 
	void stealNodes(ModelSkeleton& stealArray) { mRoot.stealNodes(stealArray); };

	/// Decode Skel datatype and Detail datatype, and these 2 file will appear
	/// in same time. the extention will be added automatically
	///
	/// @author Peo
	/// @date 2018/5/26
	///
	/// @param fileSkel The file skel.

	void decodeSD(const char* fileSkel);

private:
    LeafNodeType* LeafNodeGardPtr;
    RootNodeType* RootNodePtr;
	ModelSkeleton treeModel;
};

template<typename _RootNodeType>
inline Tree<_RootNodeType>::~Tree()
{
	mRoot.clear();
}

template<typename _RootNodeType>
int Tree<_RootNodeType>::printAllCoord()
{
	// TODO: add iteration method to print out all coordinates in tree
	return 0;
}

template<typename _RootNodeType>
void Tree<_RootNodeType>::reformTree()
{
	mRoot.reformRoot();
}

template<typename _RootNodeType>
int Tree<_RootNodeType>::saveTo(const char* filename)
{
	std::ofstream outStream;
	outStream.open(filename, std::ios::out | std::ios::binary);
	mRoot.saveTo(outStream);

	outStream.close();

	return 0;
}

template<typename _RootNodeType>
void Tree<_RootNodeType>::decodeSD(const char * fileSkel)
{
#ifdef _DEBUG
	std::cout << "============ DEBUG decodeSD: =============" << std::endl;
#endif
	char skelName[50];
	char detailName[50];
	sprintf(skelName, "%s.skel", fileSkel);
	sprintf(detailName, "%s.detail", fileSkel);
	std::ifstream skelIn(skelName, std::ios::in | std::ios::binary);
	std::ifstream detailIn(detailName, std::ios::in | std::ios::binary);
	NodeMat AMat;
	NodeDetail ADetail;
	while (!skelIn.eof()) {
		skelIn.read((char*)&AMat, sizeof(NodeMat));
		treeModel.skeleton.push_back(AMat);
#ifdef _DEBUG
		std::cout << "[ " << AMat.x << ", " << AMat.y << ", "
			<< AMat.z << "]: " << AMat.Dim << std::endl;
#endif // _DEBUG

	}
	while (!detailIn.eof()) {
		detailIn.read((char*)&ADetail, sizeof(NodeDetail));
		treeModel.details.push_back(ADetail);
#ifdef _DEBUG
		std::cout << "[ " << ADetail.x << ", " << ADetail.y << ", "
			<< ADetail.z << "]: ";
		for (int i = 0; i < 64; i++) {
			printf("%x", ADetail.details[i]);
		}
		std::cout << std::endl;
#endif // _DEBUG

	}
}

/// @brief Tree3<T, N1, N2>::Type is the type of a three-level tree
/// (Root, Internal, Leaf) with value type T and
/// internal and leaf node log dimensions N1 and N2, respectively.
/// @note This is NOT the standard tree configuration (Tree4 is).
template<typename T, Index N1 = 3, Index N2 = 3>
struct Tree3 
{
	using Type = Tree<RootNode<InternalNode<LeafNode<T, N2>, N1>>>;
};

}
