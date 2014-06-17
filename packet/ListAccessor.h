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

#include <vector>

namespace packet
{
    // allows easier access to a linksed list, where the next node is pointed to by T* T->next
    template <typename T> class ListAccessor
    {
        private:
            std::vector<T*> m_items;

        public:
            ListAccessor(T* list)
            {
                setList (list);
            }

            void setList (T* list)
            {
                m_items.clear();
                if (NULL != list)
                {
                    m_items.push_back(list);
                    while (list->next)
                    {
                        list = list->next;
                        m_items.push_back(list);
                    }
                }
            }

            size_t getListLength ()
            {
                return m_items.size();
            }

            T* at (size_t index)
            {
                return m_items.at(index);
            }
    };
}
