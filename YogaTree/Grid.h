#pragma once
#include <cassert>
#include <vector>
#include "Tree.h"


using TreeBase = tree::TreeBase;

template<typename> class Grid;	// forward declaration

template<typename GridType>
inline typename GridType::Ptr createGrid();