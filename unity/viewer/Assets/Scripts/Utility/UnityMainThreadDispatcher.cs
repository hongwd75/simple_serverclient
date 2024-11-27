using System;
using System.Collections.Concurrent;
using UnityEngine;

namespace Utility
{
    public class UnityMainThreadDispatcher : MonoBehaviour
    {
        private static UnityMainThreadDispatcher _instance = null;
        private readonly ConcurrentQueue<Action> _executionQueue = new ConcurrentQueue<Action>();

        public static UnityMainThreadDispatcher Instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = FindObjectOfType<UnityMainThreadDispatcher>();
                    if (_instance == null)
                    {
                        GameObject obj = new GameObject("UnityMainThreadDispatcher");
                        _instance = obj.AddComponent<UnityMainThreadDispatcher>();
                    }
                }
                return _instance;
            }
        }

        public void Enqueue(Action action)
        {
            _executionQueue.Enqueue(action);
        }

        private void Update()
        {
            while (_executionQueue.TryDequeue(out Action action))
            {
                action.Invoke();
            }
        }
    }
}