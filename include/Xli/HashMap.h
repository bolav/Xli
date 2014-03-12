#ifndef __XLI_HASH_MAP_H__
#define __XLI_HASH_MAP_H__

#include <Xli/Array.h>
#include <Xli/Hash.h>

namespace Xli
{
    /**
        \ingroup XliCoreContainers

        Dictionary template.
        Maps keys to values using a very efficient hash map. For user-defined TKey types, methods
        UInt32 Hash() and operator == must be defined. All built-in C++ types can be used directly
        as TKey. If the template argument TTraits can be specified to override the Hash() and Equals()
        functions used. This is useful i.e. when using pointers to objects as keys and you want to
        compare the value of the objects and not the pointers.
    */
    template <typename TKey, typename TValue, typename TTraits = HashTraits<TKey> > class HashMap
    {
        static const int BufSize = 4;

        enum BucketState
        {
            BucketStateEmpty = 0,
            BucketStateUsed = 1,
            BucketStateDummy = 2
        };

        struct Bucket
        {
            TKey Key;
            TValue Value;
            BucketState State;
        };
        
        Bucket internalBuckets[BufSize];
        Bucket* buckets;

        int bucketCount;
        int count;

        void rehash(int newSize)
        {
            Bucket* oldBuckets = buckets;

            buckets = new Bucket[newSize];
            int oldSize = bucketCount;
            bucketCount = newSize;
            count = 0;

            for (int i = 0; i < bucketCount; i++) 
                buckets[i].State = BucketStateEmpty;

            for (int i = 0; i < oldSize; i++)
                if (oldBuckets[i].State == BucketStateUsed) 
                    (*this)[oldBuckets[i].Key] = oldBuckets[i].Value;

            if (oldBuckets != internalBuckets)
                delete [] oldBuckets;
        }

        void expand()
        {
            rehash(bucketCount * 2);
        }

        HashMap(const HashMap& copy);
        HashMap& operator = (const HashMap& copy);

    public:
        HashMap(int initialSizeLog2 = 0)
        {
            if (initialSizeLog2 == 0)
            {
                bucketCount = BufSize;
                buckets = internalBuckets;
            }
            else
            {
                bucketCount = 1 << initialSizeLog2;
                buckets = new Bucket[bucketCount];
            }

            for (int i = 0; i < bucketCount; i++) 
                buckets[i].State = BucketStateEmpty;

            count = 0;
        }

        ~HashMap()
        {
            if (buckets != internalBuckets)
                delete [] buckets;
        }

        /**
            Gives an integer iterator to the beginning of the map.
        */
        int Begin() const
        {
            return Next(End());
        }

        /**
            Returns the iterator value that indicates the end of the map.
        */
        int End() const
        {
            return -1;
        }

        /**
            Advances an iterator to the next element.
            @see Begin
            @param iterator The current iterator
            @return An iterator pointing to the succeeding element, or End().
        */
        int Next(int iterator) const
        {
            for (int i = iterator + 1; i < bucketCount; i++)
                if (buckets[i].State == BucketStateUsed) 
                    return i;

            return End();
        }

        /**
            Returns the value at the given iterator
            Use Begin(), Next() and End() to iterate through the map.
            @param iterator The iterator at which to look up the value
            @return The value at the given iterator.
        */
        const TValue& GetValue(int iterator) const
        {
#ifdef XLI_RANGE_CHECK
            if (buckets[iterator].State != BucketStateUsed) 
                XLI_THROW("Invalid iterator");
#endif
            
            return buckets[iterator].Value;
        }

        /**
            Returns the value at the given iterator
            Use Begin(), Next() and End() to iterate through the map.
            @param iterator The iterator at which to look up the value
            @return The value at the given iterator.
        */
        TValue& GetValue(int iterator)
        {
#ifdef XLI_RANGE_CHECK
            if (buckets[iterator].State != BucketStateUsed) 
                XLI_THROW("Invalid iterator");
#endif
            
            return buckets[iterator].Value;
        }

        /**
            Sets a new value at the given iterator
            Use Begin(), Next() and End() to iterate through the map.
            @param iterator The iterator at which to look up the value
            @param value The new value to be set at the given iterator.
        */
        void SetValue(int iterator, const TValue& value)
        {
            if (buckets[iterator].State != BucketStateUsed) 
                XLI_THROW("Invalid iterator");
            
            buckets[iterator].Value = value;
        }

        /**
            Returns the key at the given iterator
            Use Begin(), Next() and End() to iterate through the map.
            @param iterator The iterator at which to look up the key
            @return The key at the given iterator.
        */
        const TKey& GetKey(int iterator) const
        {
#ifdef XLI_RANGE_CHECK
            if (buckets[iterator].State != BucketStateUsed) 
                XLI_THROW("Invalid iterator");
#endif
            
            return buckets[iterator].Key;
        }

        /**
            Returns a key with the given value.
            If more than one key-value-pair contains the same value, which of the keys this funciton returns is undefined.
            To get all keys with a given value, use GetKeysFromValue().
            This is a very slow lookup, as it requires a linear iteration through the hash map.
            If you want to perform this operation frequently, consider using a BiHashMap.
            @param value The value to look up
            @return The key at the given value.
        */
        bool TryGetKeyFromValue(const TValue& value, TKey& result) const
        {
            for (int i = Begin(); i != End(); i = Next(i))
            {
                if (GetValue(i) == value)
                {
                    result = GetKey(i);
                    return true;
                }
            }
            
            return false;
        }

        bool ContainsValue(const TValue& value) const
        {
            for (int i = Begin(); i != End(); i = Next(i))
                if (GetValue(i) == value) 
                    return true;

            return false;
        }

        /**
            Enumerates all keys with the given value.
            This is a very slow lookup, as it requires a linear iteration through the hash map.
            If you want to perform this operation frequently, consider using a BiHashMap.
            @param value The value to look up
            @param keys An array to fill with the found keys.
        */
        void GetKeysFromValue(const TValue& value, Array<TKey>& keys) const
        {
            for (int i = Begin(); i != End(); i = Next(i))
                if (GetValue(i) == value) 
                    keys.Add(GetKey(i));
        }

        void Clear()
        {
            for (int i = 0; i < bucketCount; i++) 
                buckets[i].State = BucketStateEmpty;

            count = 0;
        }

        TValue& operator [] (const TKey& key)
        {
            if (count > (bucketCount / 8) * 5) 
                expand();
            
            int x = TTraits::Hash(key) & (bucketCount - 1);
            int firstX = x;

            while (true)
            {
                if (buckets[x].State == BucketStateUsed)
                {
                    if (TTraits::Equals(buckets[x].Key, key)) 
                        return buckets[x].Value;
                }
                else if (buckets[x].State == BucketStateEmpty)
                {
                    buckets[x].State = BucketStateUsed;
                    buckets[x].Key = key;
                    count++;
                    return buckets[x].Value;
                }

                x++;

                if (x >= bucketCount) 
                    x -= bucketCount;

                if (x == firstX)
                {
                    rehash(bucketCount);
                    x = firstX;
                    continue;
                }
            }
        }

        void Add(const TKey& key, const TValue& value)
        {
            if (count > (bucketCount / 8) * 5) 
                expand();

            int x = TTraits::Hash(key) & (bucketCount - 1);
            int firstX = x;

            while (true)
            {
                if (buckets[x].State == BucketStateUsed)
                {
                    if (TTraits::Equals(buckets[x].Key, key)) 
                        XLI_THROW("Map already contains the given key");
                }
                else if (buckets[x].State == BucketStateEmpty)
                {
                    buckets[x].State = BucketStateUsed;
                    buckets[x].Key = key;
                    buckets[x].Value = value;
                    count++;
                    return;
                }

                x++;

                if (x >= bucketCount) 
                    x -= bucketCount;

                if (x == firstX)
                {
                    rehash(bucketCount);
                    x = firstX;
                    continue;
                }
            }
        }

        void AddRange(const HashMap& map)
        {
            for (int it = map.Begin(); it != map.End(); it = map.Next(it))
                Add(map.GetKey(it), map.GetValue(it));
        }

        bool Remove(const TKey& key)
        {
            int x = TTraits::Hash(key) & (bucketCount - 1);

            while (true)
            {
                if (buckets[x].State == BucketStateUsed)
                {
                    if (TTraits::Equals(buckets[x].Key, key))
                    {
                        buckets[x].State = BucketStateDummy;
                        count--;
                        return true;
                    }
                }
                else if (buckets[x].State == BucketStateEmpty)
                {
                    return false;
                }

                x++;

                if (x >= bucketCount) 
                    x -= bucketCount;
            }
        }

        bool ContainsKey(const TKey& key) const
        {
            int x = TTraits::Hash(key) & (bucketCount - 1);
            int firstX = x;

            while (true)
            {
                if (buckets[x].State == BucketStateUsed)
                {
                    if (TTraits::Equals(buckets[x].Key, key)) 
                        return true;
                }
                else if (buckets[x].State == BucketStateEmpty)
                {
                    return false;
                }

                x++;

                if (x >= bucketCount) 
                    x -= bucketCount;

                if (x == firstX) 
                    return false;
            }
        }

        bool TryGetValue(const TKey& key, TValue& value) const
        {
            int x = TTraits::Hash(key) & (bucketCount - 1);
            int firstX = x;

            while (true)
            {
                if (buckets[x].State == BucketStateUsed)
                {
                    if (TTraits::Equals(buckets[x].Key, key))
                    {
                        value = buckets[x].Value;
                        return true;
                    }
                }
                else if (buckets[x].State == BucketStateEmpty)
                {
                    return false;
                }

                x++;

                if (x >= bucketCount) 
                    x -= bucketCount;
                
                if (x == firstX)
                    return false;
            }
        }

        int Count() const
        {
            return count;
        }
    };
}

#endif
