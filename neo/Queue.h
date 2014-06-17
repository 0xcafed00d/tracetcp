#ifndef __NEO__Queue_h__
#define __NEO__Queue_h__

#include <queue>
#include "..\thread\Thread.h"

namespace neo
{
    template <typename T> class Queue
    {
        private:
            std::queue<T> q;
            thread::CriticalSection cs;

        public:
            class EmptyQueueRead {};
            
            Queue() {}

            void push (T item)
            {
                thread::Synchronize<thread::CriticalSection> sync(cs);
                q.push(item);
            }

            T pop ()
            {
                thread::Synchronize<thread::CriticalSection> sync(cs);
                if (q.empty())
                    throw EmptyQueueRead ();
        
                T item = q.front();
                q.pop();
                return item;
            }

            bool isEmpty ()
            {
                thread::Synchronize<thread::CriticalSection> sync(cs);
                return q.empty();
            }

            private:
                // private copy ctor and assignment op to prevent copies. Memory blocks 
                // cannot be copied and must be passed by ref or pointer. 
                Queue (const Queue&);
                Queue& operator= (const Queue& that);
    };

}

#endif