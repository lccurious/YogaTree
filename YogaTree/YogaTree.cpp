// YogaTree.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TiffIn.h"
#include "Types.h"
#include "Tree.h"
#include "Coord.h"
#include "LeafNode.h"
#include "InternalNode.h"
#include "RootNode.h"
#include "YogaTree.h"
#include <random>
#include <iomanip>

void helpInfo()
{
	std::cout << "Usage: YogaTree.exe inputTiff outputYoga" << std::endl;
}

void welcome(int argc, char* argv[])
{
	std::cout << "Welcome to YoGa Tree!" << std::endl;

	BinaryTree testTree;
	Coord coord;
	Index xBegin, xEnd;

	TiffIn Reader;
	RLEVec3Array voxelStrip;

	if (argc != 3) {
		helpInfo();
		return;
	}

	Reader.readTif(argv[1]);

	std::cout << "Image loaded" << std::endl;

	Reader.clone(voxelStrip);
	int cnt = 0, maxSize = voxelStrip.size();
	std::string message = "";
	int gap = maxSize / 100;

	voxelStrip.erase(voxelStrip.begin());

	for (vector<RLEVec3>::iterator iter = voxelStrip.begin(); iter != voxelStrip.end(); ++iter)
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
		if (++cnt % gap == 0) {
			message += "#";
			std::cout << "\r [ " << cnt * 100 / maxSize << std::setw(3) << "% ] " << message;
		}
	}
	std::cout << "\r [ 100% ] " << message;

	std::cout << "\nTransform success! Count On Voxel number: "
		<< testTree.mRoot.onVoxelCount() << std::endl;

	std::cout << "\nBegin Reform the Tree ..." << std::endl;
	testTree.reformTree();
	std::cout << "Reformed success!" << std::endl;

	testTree.saveTo(argv[2]);
}

int main(int argc, char *argv[])
{
	welcome(argc, argv);

	return 0;
}
