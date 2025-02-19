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

        private void Awake()
        {
            PoolManager = this;
        }

        public void AddPool(MoveObject obj)
        {
            objectPool.Enqueue(obj);
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
            }
            return outObj;
        }
    }
}