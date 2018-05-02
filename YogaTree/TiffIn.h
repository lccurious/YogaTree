//
// Created by peo on 17-12-28.
//

#pragma once

#include <tiff.h>
#include <tiffio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ios>
#include "Types.h"

using std::vector;

class TiffIn {
public:
    TiffIn() = default;
    ~TiffIn() = default;

    /// Reads a TIFF image from a file prase all data into memory
    /// with original data but store with original value, call @createBuffer
    ///
    /// @author Peo
    /// @date 2018/4/29
    ///
    /// @param filename Filename of the file.
    ///
    /// @return The TIF.

    int readTif(const char* filename);

    /// Converts a filename to a file
    ///
    /// @author Peo
    /// @date 2018/4/29
    ///
    /// @param filename Filename of the file.
    ///
    /// @return Filename as an int.

    int toFile(const char* filename);

    /// In memory
    ///
    /// @author Peo
    /// @date 2018/4/29
    ///
    /// @param filename Filename of the file.
    ///
    /// @return An int.

    int rleFileIn(const char* filename);

	/// In memory
	///
	/// @author Peo
	/// @date 2018/4/29
	///
	/// @param 		    filename Filename of the file.
	/// @param [in,out] RLEArray Array of rles.
	///
	/// @return An int.

	int rleFileIn(const char* filename, RLEVec3Array& RLEArray);

	/// Makes a deep copy of this object
	///
	/// @author Peo
	/// @date 2018/4/29
	///
	/// @param [in,out] RLEArray Array of rles.
	///
	/// @return A copy of this object.

	int clone(RLEVec3Array& RLEArray);

private:

    /// Creates a buffer, this gona to create RLE compressed buffer
    ///
    /// @author Peo
    /// @date 2018/4/15
    ///
    /// @param filename Filename of the file.
    ///
    /// @return The new buffer.
    int createBuffer(const char* filename);

    tdata_t buf;
    Index32 tifXSize, tifYSize, tifZSize;
    vector<RLEVec3> RLEdata;
    vector<uint8> blockElements;
};

int TiffIn::createBuffer(const char* filename) {
    TIFF* tif = TIFFOpen(filename, "r");
    TIFFSetWarningHandler(NULL);
    if (tif) {
        uint32 row, col;
        uint32 width, height, dircount=0;
        uint16 bitspersample = 1;
        uint16 sampleperpixel = 1;
        uint8 singleData;
        uint16 blockBase = 0x0;
        int turn16;

        // this buf will be kept untill the object was detoryed.
        tdata_t buf;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &sampleperpixel);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);

        // padding edge to be 16x
        buf = _TIFFmalloc(TIFFScanlineSize(tif));

        RLEdata.clear();
		RLEVec3 RLEHeader;
		RLEHeader.x[0] = 0;
		RLEHeader.x[1] = width;
		RLEHeader.y = height;
		RLEHeader.z = 0;
		RLEdata.push_back(RLEHeader);

        while(TIFFReadDirectory(tif)) {
            
			// Read the data every row
            for (row = 0; row < height; row++) {
                TIFFReadScanline(tif, buf, row);
                // Read the element
                for (col = 0, turn16 = 16; col < width; col++) {
                    singleData = *((uint8*)buf + col);
                    if (singleData) {
                        RLEVec3 tmpStripe;
                        tmpStripe.z = dircount;
                        tmpStripe.y = row;
                        tmpStripe.x[0] = col;
                        while(singleData && col < width) {
                            col++;
                            singleData = *((uint8*)buf + col);
                        }
                        tmpStripe.x[1] = col;
                        RLEdata.push_back(tmpStripe);
                    }
                }
            }
            dircount++;
        }
		RLEdata[0].z = dircount;
        _TIFFfree(buf);
        TIFFClose(tif);

        return 0;
    } else {
        fprintf(stderr, "Failed to open tiff file.\n");
        return -1;
    }
}

int TiffIn::readTif(const char* filename)
{
    createBuffer(filename);
    return 0;
}

int TiffIn::toFile(const char* filename)
{

    // std::cout << "After RLE, length is: " << RLEdata.size() << std::endl;

    if (RLEdata.size() > 0) {
        std::ofstream out_file(filename, std::ios::out | std::ios::binary);
        for (const RLEVec3& info: RLEdata) {
            out_file.write(reinterpret_cast<const char*>(&info), sizeof(info));
        }
        out_file.close();

        return 0;
    }
    else
    {
        fprintf(stderr, "buffer was empty, failed to write file!\n");
        return -1;
    }
}

int TiffIn::rleFileIn(const char* filename)
{
    RLEVec3 info;
    std::ifstream in_file(filename, std::ios::in | std::ios::binary);
    RLEdata.clear();
    if (!in_file.eof() && !in_file.fail()) {
        while(in_file.read(reinterpret_cast<char*>(&info), sizeof(info))) {
            RLEdata.push_back(info);
        }
        // std::cout << "RLE block count: " << RLEdata.size() << std::endl;
    }
	else
	{
		std::cerr << "obj read failed!!!" << std::endl;
	}
    return 0;
}

int TiffIn::rleFileIn(const char* filename, RLEVec3Array& RLEArray)
{
	RLEVec3 info;
	std::ifstream in_file(filename, std::ios::in | std::ios::binary);
	if (!in_file.eof() && !in_file.fail()) {
		while (in_file.read(reinterpret_cast<char*>(&info), sizeof(info))) {
			RLEArray.push_back(info);
		}
		// std::cout << "RLE block count: " << RLEArray.size() << std::endl;
	}
	else
	{
		std::cerr << "obj read failed!!!" << std::endl;
	}
	return 0;
}

int TiffIn::clone(RLEVec3Array& RLEArray)
{
	RLEArray.clear();
	RLEArray.assign(RLEdata.begin(), RLEdata.end());
	return 0;
}