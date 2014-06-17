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

#if !defined(AFX_SOCKETEXCEPTION_H__E9C654DF_B6C5_4521_AEFC_92A8A72016E5__INCLUDED_)
#define AFX_SOCKETEXCEPTION_H__E9C654DF_B6C5_4521_AEFC_92A8A72016E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <string>
#include <exception>


namespace net
{
	class SocketException : public std::exception
    {
        private:
            int m_errorCode;
            const char* m_functionName;
            const char* getWSErrorString (int code)  const;
			std::string m_errorString;


        public:
            SocketException(const char* functionName, int errorCode)
                : m_functionName(functionName),
                  m_errorCode(errorCode)
            {
				m_errorString = std::string(functionName) + ": " + getErrorString();
            }

            int getErrorCode() const
            {
                return m_errorCode;
            }

            const char* getFunctionName () const
            {
                return m_functionName;    
            }

            const char* getErrorString () const
            {
                return getWSErrorString(m_errorCode);
            }

			virtual const char* what() const throw()
			{
				return m_errorString.c_str();
			}
    };
}
#endif 
