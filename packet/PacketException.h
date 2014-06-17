//
// Copyright (c) 2005 L.M.Witek
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

#include <string>
#include <exception>

namespace packet
{
	class PacketException : public std::exception
	{
		private:
			std::string m_message;

		public:
			PacketException (const char* message) : m_message (message)
			{
			}

			const std::string& getMessage ()
			{
				return m_message;
			}

			virtual const char* what() const throw()
			{
				return m_message.c_str();
			}
	};
}