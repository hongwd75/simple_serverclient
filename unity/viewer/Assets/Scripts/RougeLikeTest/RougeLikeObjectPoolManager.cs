using System;
using System.Collections.Concurrent;
using Unity.VisualScripting;
using UnityEngine;

namespace Characters
{
    public class RougeLikeObjectPoolManager : MonoBehaviour
    {
        [SerializeField]
        protected MoveObject spawnObject;
        protected ConcurrentQueue<MoveObject> objectPool = new ConcurrentQueue<MoveObject>();

        public static RougeLikeObjectPoolManager PoolManager;

        protected int spawnedObjectCount = 0;

        public int ActiveObjectCount => spawnedObjectCount;
        private void Awake()
        {
            PoolManager = this;
        }

        public void AddPool(MoveObject obj)
        {
            if (obj != null)
            {
                spawnedObjectCount--;
                objectPool.Enqueue(obj);
            }
        }

        public MoveObject Get()
        {
            MoveObject outObj = null;
            if (objectPool.Count > 0)
            {
                objectPool.TryDequeue(out outObj);
            }

            if (outObj == null)
            {
                outObj = Instantiate(spawnObject);
            }

            if (outObj != null)
            {
                outObj.Init();
                spawnedObjectCount++;
            }
            return outObj;
        }
    }
}