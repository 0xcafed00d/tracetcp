//
// Copyright (c) 2003 L.M.Witek
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef __StringUtils_NEO_h__
#define __StringUtils_NEO_h__

#include <string>
#include <stdexcept>

namespace neo
{
    class ParseException
    {
    };

    inline std::string trimLeft (std::string str, const char* wschars = " \t")
    {
        str.erase(0,str.find_first_not_of(wschars));
        return str;
    }

    inline std::string trimRight (std::string str, const char* wschars = " \t")
    {
        str.erase(str.find_last_not_of(wschars)+1);
        return str;
    }

    inline std::string trimBoth (std::string str, const char* wschars = " \t")
    {
        return trimLeft(trimRight(str, wschars), wschars);
    }


    // convert strings to numeric types..... all throw ParseException if not valid strings
    int parseInt (std::string str);
    unsigned int parseUnsignedInt (std::string str);

    short parseShort (std::string str);
    unsigned short parseUnsignedShort (std::string str);

    double parseDouble (std::string str);

    // convert numeric types to strings
    std::string toString (int value);

    void StringUtils_testharness ();
}


#endif