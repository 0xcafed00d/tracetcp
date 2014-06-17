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

#include <assert.h>
#include <strstream>
#include <stdlib.h>

#include "StringUtils.h"

namespace neo
{
    template<class valueType> 
    inline valueType parse (std::string str)
    {
        str = trimBoth(str);
        valueType value;
        std::istrstream stream (str.c_str());
        
        stream >> value;
        // if we failed extract an valid value or we didnt use up all the chars then throw an error
        if (stream.fail() || !stream.eof())
        {
            throw ParseException();
        }

        return value;
    }

    int parseInt (std::string str)
    {
        return parse<int>(str);
    }
    
    unsigned int parseUnsignedInt (std::string str)
    {
        return parse<unsigned int>(str);
    }
    
    short parseShort (std::string str)
    {
        return parse<short>(str);
    }

    unsigned short parseUnsignedShort (std::string str)
    {
        return parse<unsigned short>(str);
    }

    double parseDouble (std::string str)
    {
         return parse<double>(str);
    }

    std::string toString (int value)
    {
        char buff[32];
        _itoa(value, buff, 10);
        return std::string(buff);
    }



    void StringUtils_testharness ()
    {
        assert (trimLeft(" xx ") == "xx ");
        assert (trimRight(" xx ") == " xx");
        assert (trimBoth("  xx  ") == "xx");
        assert (trimLeft(trimRight("xx")) == "xx");
        assert (trimLeft(trimRight("")) == "");

    }
}

