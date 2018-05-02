//
// Created by lccur on 2017/12/23.
//

#pragma once

template<int Size>
class BitMask {
public:
    BitMask() = default;
    ~BitMask() = default;

    bool isChild(int iLoc) {
        return childIs[iLoc];
    };

    void setChild(int iLoc) {
        childIs[iLoc] = true;
    }

    void offChild(int iLoc) {
        childIs[iLoc] = false;
    }

private:
    static const int sSize = Size;
    bool childIs[sSize] = { false };
};
