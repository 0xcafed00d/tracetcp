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

#include "CommandOptionParser.h"
#include "StringUtils.h"

namespace neo
{
    CommandOption::CommandOption ()
                  :m_optionChar ('-'),
                   m_minParams (0),
                   m_maxParams (0),
                   m_helpText (""),
                   m_isPresent (false)

    {
    }


    CommandOption::CommandOption (char optionChar, size_t minParams, size_t maxParams, std::string helpText)
                  :m_optionChar (optionChar),
                   m_minParams (minParams),
                   m_maxParams (maxParams),
                   m_helpText (helpText),
                   m_isPresent (false)
    {
    }


    void CommandOption::checkIndex (size_t index)
    {
        if (index > m_params.size())
            throw CommandOptionException (std::string("Internal Error: index out of range for option: ") + m_optionChar);
    }      
     
    std::string CommandOption::getParam (size_t index)
    {
        checkIndex (index);
        return m_params[index];
    }

    std::string CommandOption::getParam (size_t index, std::string defaultValue)
    {
        // if this option was not present on the command line then return the default value.
        if (!isPresent())
            return defaultValue;

        return getParam (index);
    }

    int CommandOption::getParamAsInt (size_t index)
    {
        checkIndex (index);
        
        std::string param = m_params[index];

        try
        {   
            return parseInt (param);
        }
        catch (ParseException&)
        {
            throw CommandOptionException (std::string("Invalid numeric value: \"") + param + "\" on option -" + m_optionChar);
        }
    }

    int CommandOption::getParamAsInt (size_t index, int minValue, int maxValue, int defaultValue)
    {
        // if this option was not present on the command line then return the default value.
        if (!isPresent())
            return defaultValue;

        int value = getParamAsInt (index);
        if ((value < minValue) || (value > maxValue))
            throw CommandOptionException (std::string("Value \"") + toString(value) + "\" out of range: [" +
                                          toString(minValue) + ".." + toString(maxValue) + "] on option -" + m_optionChar);


        return value;
    }

    void CommandOption::validate() const
    {
        if (!isPresent())
            return;

        if (m_params.size() > m_maxParams)
            throw CommandOptionException(std::string ("option -") + m_optionChar + " has too many parameters specified.");    

        if (m_params.size() < m_minParams)
            throw CommandOptionException(std::string ("option -") + m_optionChar + " has too few parameters specified.");    
    }
}