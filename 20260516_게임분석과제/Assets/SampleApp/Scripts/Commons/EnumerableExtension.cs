using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace SampleApp.Linq
{
    public static class EnumerableExtension
    {
        public static IEnumerable<T> Rotate<T>( this IEnumerable<T> thisEnum, int count )
        {
            return thisEnum.Skip( count ).Concat( thisEnum.Take( count ) );
        }

        public static int IndexOf<T>( this IEnumerable<T> thisEnum, System.Func<T, bool> pred )
        {
            using var etor = thisEnum.GetEnumerator();
            int i = 0;
            while(etor.MoveNext())
            {
                if(pred( etor.Current ))
                {
                    return i;
                }
                ++i;
            }
            return -1;
        }

        public static int IndexOfMin<T>( this IEnumerable<T> thisEnum )
        {
            return thisEnum.IndexOfMin( Comparer<T>.Default.Compare );
        }

        public static int IndexOfMin<T>( this IEnumerable<T> thisEnum, System.Func<T, T, int> Comparer )
        {
            using var etor = thisEnum.GetEnumerator();
            if(!etor.MoveNext())
            {
                return -1;
            }
            var min = etor.Current;
            var minIndex = 0;
            int i = 0;
            while(etor.MoveNext())
            {
                ++i;
                if(Comparer( min, etor.Current ) > 0)
                {
                    min = etor.Current;
                    minIndex = i;
                }
            }
            return minIndex;
        }

        public static int IndexOfMax<T>( this IEnumerable<T> thisEnum )
        {
            return thisEnum.IndexOfMax( Comparer<T>.Default.Compare );
        }

        public static int IndexOfMax<T>( this IEnumerable<T> thisEnum, System.Func<T, T, int> Comparer )
        {
            return thisEnum.IndexOfMin( ( l, r ) => Comparer( r, l ) );
        }

        public static IEnumerable<TSource> UnionBy<TSource, TKey>(
            IEnumerable<TSource> first, IEnumerable<TSource> second, Func<TSource, TKey> keySelector )
        {
            var set = new HashSet<TKey>();

            foreach(TSource element in first)
            {
                if(set.Add( keySelector( element ) ))
                {
                    yield return element;
                }
            }

            foreach(TSource element in second)
            {
                if(set.Add( keySelector( element ) ))
                {
                    yield return element;
                }
            }
        }

        public static IEnumerable<TSource> IntersectBy<TSource, TKey>(
            this IEnumerable<TSource> first, IEnumerable<TKey> second, Func<TSource, TKey> keySelector )
        {
            var set = new HashSet<TKey>(second);

            foreach(TSource element in first)
            {
                if(set.Remove( keySelector( element ) ))
                {
                    yield return element;
                }
            }
        }

        public static IEnumerable<TSource> ExceptBy<TSource, TKey>(
            this IEnumerable<TSource> first, IEnumerable<TKey> second, Func<TSource, TKey> keySelector )
        {
            var set = new HashSet<TKey>(second);

            foreach(TSource element in first)
            {
                if(set.Add( keySelector( element ) ))
                {
                    yield return element;
                }
            }
        }
    }
}
