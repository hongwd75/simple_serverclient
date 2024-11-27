using System.Threading;
using UnityEngine;

namespace Utility
{
    public abstract class MonoSingleton<T> : MonoBehaviour where T : MonoBehaviour
    {
        private static T _instance = null;
        private static readonly object _lock = new object();
        private static bool _isInitialized = false;

        #region 클래스 설정 부분 ################################################################
        // 파괴되지 않는 기능 설정
        protected virtual bool dontDistroyFlag
        {
            get => false;
        }

        // 초기화
        protected abstract void InitClass();
        #endregion
        
        public static T Instance
        {
            get
            {
                if (_isInitialized == true) return _instance;
                
                lock (_lock)
                {
                    if (_instance == null)
                    {
                        if (Thread.CurrentThread.ManagedThreadId == 1)
                        {
                            // 유니티 메인 스레드임
                            CreateInstance();
                        }
                        else
                        {
                            // 메인 스레드가 아닌 경우 처리
                            UnityMainThreadDispatcher.Instance.Enqueue(()=>CreateInstance());
                            while (_instance == null)
                            {
                                Thread.Sleep(1);
                            }
                        }
                    }
                    return _instance;
                }
            }
        }
        
        private static void CreateInstance()
        {
            _instance = FindObjectOfType<T>();
            if (_instance == null)
            {
                GameObject obj = new GameObject(typeof(T).Name);
                _instance = obj.AddComponent<T>();
            }
            _isInitialized = true;
        }


        protected void Awake()
        {
            if (dontDistroyFlag == true)
            {
                DontDestroyOnLoad(gameObject);
            }

            InitClass();
        }        
        
        // 싱글톤 인스턴스 해제
        public static void Release()
        {
            if (_instance != null)
            {
                Destroy(_instance.gameObject);
                _instance = null;
                _isInitialized = false;
            }
        }

        protected virtual void OnDestroy()
        {
            if (_instance == this)
            {
                _instance = null;
                _isInitialized = false;
            }
        }        
    }
}