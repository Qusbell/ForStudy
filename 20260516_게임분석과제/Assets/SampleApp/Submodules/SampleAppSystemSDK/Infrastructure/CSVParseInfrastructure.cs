using SampleAppSystemSDK.Domain;
using System;
using System.Collections.Generic;
using System.Reflection;

namespace SampleAppSystemSDK.Application
{
    public class CSVParseInfrastructure : IParseDomain
    {
        public IReadOnlyList<T> ParsingProcess<T>( string rawData ) where T : new()
        {
            var resultList = new List<T>();

            if( string.IsNullOrWhiteSpace( rawData ) )
            {
                return resultList;
            }

            string[] lines = rawData.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

            FieldInfo[] fields = typeof(T).GetFields(BindingFlags.Public | BindingFlags.Instance);

            foreach( var line in lines )
            {
                string[] values = line.Split(',');
                T dataInstance = new T();
                for( int i = 0; i < fields.Length && i < values.Length; i++ )
                {
                    if( string.IsNullOrWhiteSpace( values[i] ) ) continue;

                    try
                    {
                        object convertedValue = Convert.ChangeType(values[i].Trim(), fields[i].FieldType);
                        fields[i].SetValue( dataInstance, convertedValue );
                    }
                    catch( Exception ex )
                    {
                        UnityEngine.Debug.LogError( $"파싱 에러: {fields[i].Name} 필드에 '{values[i]}' 값을 넣을 수 없어. \n{ex.Message}" );
                    }
                }

                resultList.Add( dataInstance );
            }

            return resultList;
        }
    }
}
