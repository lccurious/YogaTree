//
// Created by peo on 17-12-28.
//

#pragma once

#include <string>

enum { DEFAULT_WIDTH = 900, DEFAULT_HEIGHT = 800 };

class Viewer;

/// \brief Initialize and return a viewer.
/// \param progName
/// \param background   if true, run the viewer in a separate thread
/// \return
Viewer init(const std::string& progName, bool background);

/// \brief Destroy all viewer windows and release resources.
/// \details This should be called from the main thread before your program exits.
void exit();

/// Manager for a window that displays OpenVDB grids
class Viewer {
public:
    /// set the size of and open the window associated with this viewer.
    void open(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);

    /// Display the give grids
};

