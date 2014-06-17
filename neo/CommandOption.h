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

#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <map>

namespace neo
{
	class CommandOptionException : public std::exception
    {
        private:
            std::string m_message;

        public:
            CommandOptionException (std::string message) : m_message(message) {}
            std::string getMessage () const
            {
                return m_message;
            }

			const char* what ()
			{
				return m_message.c_str();
			}

    };


    class CommandOption
    {

        private:
            char   m_optionChar;
            size_t m_minParams;
            size_t m_maxParams; 
            std::string m_helpText;
            
            bool m_isPresent;

            std::vector<std::string> m_params;

            void checkIndex (size_t index);

        public:

            static const char UNNAMED = -1;

            CommandOption ();
            CommandOption (char optionChar, size_t minParams, size_t maxParams, std::string helpText);

            char getOptionChar() const 
            {
                return m_optionChar;
            }

            std::string getHelpText() const
            {
                return m_helpText;
            }

            void addParam (std::string p)
            {
                m_params.push_back(p);
            }

            void validate() const;

            bool isPresent () const
            {
                return m_isPresent;
            }

            void setPresent ()
            {
                m_isPresent = true;
            }

            size_t getNumParams ()
            {
                return m_params.size();
            }

            std::string getParam (size_t index);
            std::string getParam (size_t index, std::string defaultValue);

            int getParamAsInt (size_t index);
            int getParamAsInt (size_t index, int minValue, int maxValue, int defaultValue);
         
    };
}
