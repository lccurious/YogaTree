#include "pch.h"

#include "../Types.h"
#include "../LeafNode.h"
#include "../InternalNode.h"
#include "../RootNode.h"
#include "../NodeUnion.h"
#include "../Tree.h"
#include "../TiffIn.h"
#include "../YogaTree.h"
#include <random>


class TreeTest : public ::testing::Test
{
protected:
	virtual void SetUp() {
		Reader.rleFileIn("RLECompressed.robj");
		Reader.clone(rawRLEData);
	}

	BinaryTree testTree;
	RLEVec3Array rawRLEData;
	TiffIn Reader;
	const BinaryTree::ValueType DenseValue = BinaryTree::DenseValue;
	/*
	virtual void TearDown() {

	}
	*/
};

class IOTest : public ::testing::Test
{
protected:
	virtual void SetUp() {

	}
	TiffIn Reader;
};

TEST_F(IOTest, ReadTiff)
{
	ASSERT_EQ(0, Reader.readTif("../test.tif"));
	ASSERT_EQ(0, Reader.toFile("RLECompressed.robj"));
}

TEST(RandomLight, 64)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 64; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}

TEST(RandomLight, 640)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 640; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}

TEST(RandomLight, 6400)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 6400; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}

TEST(RandomLight, 64000)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 64000; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}

TEST(RandomLight, 640000)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 640000; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}

TEST(RandomLight, 6400000)
{
	BinaryTree testTree;
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 2000),
		distribY(0, 2000), distribZ(0, 2000);
	Index unifyX, unifyY, unifyZ;
	for (int i = 0; i < 6400000; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		testTree.Light(coord);
	}
}


TEST_F(TreeTest, RegisteKey)
{
	Coord    coord;
	for (int i = 0; i<120; i++) {
		coord.setZ(i);
		for (int j = 0; j < 120; j++) {
			coord.setY(j);
			for (int k = 0; k < 100; k++) {
				coord.setX(k);
				testTree.mRoot.findOrAddCoord(coord);
			}
		}
	}
}

TEST_F(TreeTest, FillTree) 
{
	int xBegin;
	int xEnd;
	Coord coord;

	ASSERT_EQ(0, Reader.rleFileIn("RLECompressed.robj"));
	ASSERT_EQ(0, Reader.clone(rawRLEData));
	EXPECT_GT(rawRLEData.size(), 1);

	for (vector<RLEVec3>::iterator iter = rawRLEData.begin(); iter != rawRLEData.end(); ++iter)
	{
		xBegin	= (*iter).x[0];
		xEnd = (*iter).x[1];
		coord.setY((*iter).y);
		coord.setZ((*iter).z);
		for (; xBegin < xEnd; xBegin++)
		{
			coord.setX(xBegin);
			testTree.Light(coord);
		}
	}
}

TEST_F(TreeTest, Traverse)
{
	int xBegin;
	int xEnd;
	Coord coord;
	ASSERT_GT(rawRLEData.size(), 1);

	// Skip the Cude information
	rawRLEData.erase(rawRLEData.begin());

	for (vector<RLEVec3>::iterator iter = rawRLEData.begin(); iter != rawRLEData.end(); ++iter)
	{
		xBegin = (*iter).x[0];
		xEnd = (*iter).x[1];
		coord.setY((*iter).y);
		coord.setZ((*iter).z);
		for (; xBegin < xEnd; xBegin++)
		{
			coord.setX(xBegin);
			testTree.Light(coord);
		}
	}

	testTree.reformTree();

	std::cout << "Voxel Number: " << testTree.mRoot.onVoxelCount() << std::endl;
}


TEST_F(TreeTest, RandomFetch)
{
	Coord coord;
	std::default_random_engine generator;
	std::uniform_int_distribution<Index> distribX(0, 1000),
		distribY(0, 1000), distribZ(0, 1000);
	Index unifyX, unifyY, unifyZ;
	std::vector<Coord> LightedCoord;
	for (int i = 0; i < 64000; i++) {
		unifyX = distribX(generator);
		unifyY = distribY(generator);
		unifyZ = distribZ(generator);
		Coord coord(unifyX, unifyY, unifyZ);
		LightedCoord.push_back(coord);
		testTree.Light(coord);
	}

	
	for (auto iter : LightedCoord)
	{
		EXPECT_EQ(DenseValue, testTree.fetchValue(iter));
	}
}