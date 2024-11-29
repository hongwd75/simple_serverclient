using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using Players.Structs;
using UnityEngine;

namespace Players
{
    public class ObjectManager
    {
        private ConcurrentDictionary<int, BaseObject> objects = new();
        private readonly object objectLock = new();

        #region ### 객체 추가 삭제 ###
        public void AddObject(int index,BaseObject obj)
        {
            if (objects.TryAdd(index, obj) == false)
            {
                Debug.LogError($"[오류] 객체({index}) 추가 오류 발생");
            }
        }

        public void RemoveObject(int index)
        {
            objects.TryRemove(index, out var removed);
            removed?.OnRemoved();
        }
        #endregion

        #region ### 객체 획득 ###
        public BaseObject GetObject(int index)
        {
            if (objects.TryGetValue(index, out var obj) == true)
            {
                return obj;
            }
            return null;
        }
        
        public void ModifyObject(int index, Action<BaseObject> action)
        {
            lock (objectLock)
            {
                if (objects.TryGetValue(index, out var obj) == true)
                {
                    action?.Invoke(obj);
                }
            }
        }
        #endregion
    }
}