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


#ifndef __RefPtr_h__
#define __RefPtr_h__


namespace neo
{                
    // reference counted templated pointer 
    template <typename T> class RefPtr
    {
        private:
            class RefCount
            {
                private:
                    int m_RefCount;
                    T*  m_ObjPtr;
            
                public:
                    RefCount (T* obj) : m_RefCount (1), m_ObjPtr(obj) 
                    {
                    }

                    ~RefCount () 
                    {
                        delete m_ObjPtr;
                    }

                    void attach ()
                    {
                        if (this == 0) return;
                        m_RefCount++;
                    }

                    void detach ()
                    {
                        if (this == 0) return;
                        m_RefCount--;
                        if (m_RefCount == 0) delete this;
                    }

                    T* getObject () 
                    {
                        if (this == 0) return 0;
                        return m_ObjPtr;
                    }
            };

            RefCount* m_RefCounter;

        public:
            RefPtr (T* ptr = 0) : m_RefCounter(0)
            {
                if (ptr)
                {
                    m_RefCounter = new RefCount (ptr);
                }
            }

            RefPtr (const RefPtr<T>& other) : m_RefCounter(0)
            {
                // detach from current ref count obj
                m_RefCounter->detach(); 

                // attach to ref count from other object
                m_RefCounter = other.m_RefCounter;
                m_RefCounter->attach();
            }

            ~RefPtr ()
            {
                // detach from current ref count obj
                m_RefCounter->detach(); 
            }

            RefPtr& operator= (const RefPtr& other)
            {
                // detach from current ref count obj
                m_RefCounter->detach(); 

                // attach to ref count from other object
                m_RefCounter = other.m_RefCounter;
                m_RefCounter->attach();

                return *this;
            }
            
            RefPtr& operator= (T* ptr)
            {
                // detach from current ref count obj
                m_RefCounter->detach(); 

                // create a new ref counter object 
                if (ptr)
                    m_RefCounter = new RefCount (ptr);
                else
                    m_RefCounter = 0;

                return *this;
            }

            T* operator->() { return m_RefCounter->getObject(); }
            operator T*()   { return m_RefCounter->getObject(); }
            T& operator*()  { return  *m_RefCounter->getObject(); }

            int isNULL ()   { return m_RefCounter->getObject() == 0;}
    };
}

#endif
