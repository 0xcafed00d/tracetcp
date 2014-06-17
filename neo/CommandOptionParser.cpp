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

namespace neo
{
    CommandOptionParser::CommandOptionParser ()
    {
        addOption(CommandOption(CommandOption::UNNAMED, 0, 0, "Unnamed Options"));
    }


    void CommandOptionParser::parse (int argc, char** argv)
    {
        char currentOption = CommandOption::UNNAMED;
        getOption(currentOption).setPresent();

        // start at 1 as this is the program name.
        for (int n = 1; n < argc; n++)
        {
            std::string argStr (argv[n]);
            // is this an option? 
            if ((argStr.size() >= 2) && (argStr[0] == '-'))
            {
                for (size_t argNum = 1; argNum < argStr.size(); argNum++)
                {
                    currentOption = argStr[argNum];

                    // is this a valid option ?
                    std::map<char, CommandOption>::iterator opt = m_options.find(currentOption);
                    if (m_options.end() == opt)
                        throw CommandOptionException(std::string ("-") + currentOption + " is not a valid command option.");
                    else
                        opt->second.setPresent();
                }
            }
            else
            {
                // add param to current option
                CommandOption& opt = getOption(currentOption);
                opt.addParam(argStr);
            }
        }
        
        // validate the options. the validate method on each option will throw if the options are not valid.
        std::map<char, CommandOption>::const_iterator opt;
        for (opt = m_options.begin(); opt != m_options.end(); ++opt)
        {
            opt->second.validate();
        }
    }

    void CommandOptionParser::addOption (CommandOption& option)
    {
        m_options[option.getOptionChar()] = option;
    }

    CommandOption& CommandOptionParser::getOption (char optionChar)
    {
        std::map<char, CommandOption>::iterator opt = m_options.find(optionChar);

        if (m_options.end() == opt)
        {
            throw CommandOptionException(std::string ("CommandOptionParser::getOption invalid option lookup: ") + optionChar);
        }

        return opt->second;
    }

    void CommandOptionParser::displayOptionsHelp (std::ostream& out)
    {
        for (std::map<char, CommandOption>::iterator i = m_options.begin(); i != m_options.end(); ++i)
        {
            if (i->first != CommandOption::UNNAMED)
                out << "    -" << i->first << " " << i->second.getHelpText() << std::endl;
        }
    }

}