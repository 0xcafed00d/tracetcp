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

#include <stddef.h>
#include "RefPtr.h"

namespace neo
{
    // encapsuates a block of memory, unlike a malloced block of memory 
    // the size of the block is available.
    class MemoryBlock
    {
        public:
            typedef RefPtr<MemoryBlock> ref;  // smart ref to an object of this type
            typedef char element_t;

            // constructs a block og memory of the given size.
            MemoryBlock(size_t size) : m_size(size)
            {
                m_block = new element_t[size];
            }

            // destroys the block of memory
            ~MemoryBlock(void)
            {
                delete[] m_block;
            }

            // returns the size of this block.
            size_t getSize() const 
            {
                return m_size; 
            }

            // returns a pointer to the block
            element_t* getBlock()
            {
                return m_block;
            }

            // returns a constant pointer to the block
            const element_t* getBlock() const 
            {
                return m_block;
            }

            void resize (size_t newSize)
            {
                delete[] m_block;
                m_block = new element_t[newSize];
                m_size = newSize;
            }

        private:
            // private copy ctor and assignment op to prevent copies. Memory blocks 
            // cannot be copied and must be passed by ref or pointer. 
            MemoryBlock (const MemoryBlock&);
            MemoryBlock& operator= (const MemoryBlock& that);

            size_t m_size;           // the number of elements in this block of memory.
            element_t* m_block;      // pointer to the block of memory.

    };
}