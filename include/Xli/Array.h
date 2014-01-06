#ifndef __XLI_ARRAY_H__
#define __XLI_ARRAY_H__

#include <Xli/Config.h>
#include <Xli/Object.h>
#include <Xli/Exception.h>
#include "Sort.h"

namespace Xli
{
    /**
        \ingroup XliCoreContainers
    */
    template <typename T, int TBufSize> class Array: public Object
    {
        T* data;
        T buf[TBufSize];
        int used;
        int capacity;

    public:
        Array()
        {
            used = 0;
            capacity = TBufSize;
            data = buf;
        }

        Array(int size)
        {
            used = 0;
            capacity = TBufSize;
            data = buf;
            Resize(size);
        }

        Array(int count, const T* initItems)
        {
            used = 0;
            capacity = TBufSize;
            data = buf;
            Resize(count);

            for (int i = 0; i < count; i++) 
                data[i] = initItems[i];
        }

        Array(const Array& a)
        {
            used = 0;
            capacity = TBufSize;
            data = buf;
            Resize(a.used);

            for (int i = 0; i < used; i++)
                data[i] = a.data[i];
        }

        Array& operator = (Array& a)
        {
            Resize(a.used);

            for (int i = 0; i < used; i++)
                data[i] = a.data[i];

            return *this;
        }

        ~Array()
        {
            if (data != buf) 
                delete [] data;
        }

        void Reserve(int newCapacity)
        {
            if (newCapacity > capacity)
            {
                T* newData = new T[newCapacity];
                
                for (int i = 0; i < used; i++) 
                    newData[i] = data[i];
                
                if (data != buf) 
                    delete [] data;
                
                data = newData;
                capacity = newCapacity;
            }
        }

        void Trim()
        {
            if (data != buf)
            {
                if (used <= (int)TBufSize)
                {
                    for (int i = 0; i < used; i++)
                        buf[i] = data[i];

                    delete [] data;
                    data = buf;
                    capacity = TBufSize;
                }
                else if (used < capacity)
                {
                    T* newData = new T[used];

                    for (int i = 0; i < used; i++)
                        newData[i] = data[i];

                    delete [] data;
                    data = newData;
                    capacity = used;
                }
            }
        }

        void Resize(int newSize)
        {
            Reserve(newSize);
            used = newSize;
        }

        void Expand()
        {
            if (used > capacity / 2) 
                Reserve(capacity * 2);
        }

        int Length() const
        {
            return used;
        }

        int FirstIndex() const
        {
            return 0;
        }

        int LastIndex() const
        {
            return used - 1;
        }

        int Add()
        {
            if (capacity <= used) 
                Reserve(capacity * 2);
            
            return used++;
        }

        int Add(const T& item)
        {
            if (capacity <= used) 
                Reserve(capacity * 2);

            data[used] = item;
            return used++;
        }

        int Add(const T* items, int count)
        {
            int res = used;
            
            for (int i = 0; i < count; i++) 
                Add(items[i]);
            
            return res;
        }

        int Add(const Array<T>& values)
        {
            return Add(values.data, values.used);
        }

        int Insert(int index, const T& item)
        {
#ifdef XLI_RANGE_CHECK
            if (index > used || index < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif

            Add(item);

            for (int i = used - 1; i > index; i--) 
                data[i] = data[i - 1];

            data[index] = item;
            return index;
        }

        int IndexOf(const T& value) const
        {
            for (int i = 0; i < used; i++) 
                if (data[i] == value) 
                    return i;

            return -1;
        }

        int LastIndexOf(const T& elm) const
        {
            for (int i = used-1; i >= 0; i--) 
                if (data[i] == elm) 
                    return i;

            return -1;
        }

        bool Contains(const T& value) const
        {
            int i = IndexOf(value);
            return i != -1;
        }

        void RemoveAt(int index)
        {
#ifdef XLI_RANGE_CHECK
            if (index >= used || index < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif

            for (int i = index; i < used - 1; i++) 
                data[i] = data[i + 1];

            used--;
        }

        bool Remove(const T& item)
        {
            int i = IndexOf(item);

            if (i != -1)
            {
                RemoveAt(i);
                return true;
            }

            return false;
        }

        void RemoveAll(const T& item)
        {
            while (Remove(item));
        }

        void SwapRemoveAt(int i)
        {
            Swap(Get(i), Last());
            used--;
        }

        void RemoveRange(int start, int count)
        {
#ifdef XLI_RANGE_CHECK
            if (start >= used || start < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;

            if (start+count > used || count < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif

            for (int i = start; i < used - count; i++) 
                data[i] = data[i + count];

            used -= count;
        }

        T RemoveFirst()
        {
            T temp = First();
            RemoveAt(0);
            return temp;
        }

        T RemoveLast()
        {
            T temp = Last();
            RemoveAt(used-1);
            return temp;
        }

        T& Get(int index)
        {
#ifdef XLI_RANGE_CHECK
            if (index >= used || index < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif

            return data[index];
        }

        const T& Get(int index) const
        {
#ifdef XLI_RANGE_CHECK
            if (index >= used || index < 0)
                XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif

            return data[index];
        }

        void Clear()
        {
            used = 0;
        }

        T& operator [] (int index)
        {
            return Get(index);
        }

        const T& operator [] (int index) const
        {
            return Get(index);
        }

        T* Data()
        {
            return data;
        }

        const T* Data() const
        {
            return data;
        }

        T& First()
        {
            return Get(0);
        }

        const T& First() const
        {
            return Get(0);
        }

        T& Last()
        {
            return Get(used - 1);
        }

        const T& Last() const
        {
            return Get(used - 1);
        }

        void Reverse()
        {
            for (int a = 0, b = used; a < --b; a++)
                Swap(data[a], data[b]);
        }

        template <typename TComparator> void Sort()
        {
            Xli::Sort<T, TComparator>(data, 0, used);
        }

        void Sort()
        {
            Sort<ComparatorLessThan<T> >();
        }

        void SortReversed()
        {
            Sort<ComparatorGreaterThan<T> >();
        }

        void SortPointers()
        {
            Sort<ComparatorPointerLessThan<T> >();
        }

        void SortPointersReversed()
        {
            Sort<ComparatorPointerGreaterThan<T> >();
        }
    };
}


#endif
