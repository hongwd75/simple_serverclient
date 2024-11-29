using System;
using System.Threading;
using Players.Structs;
using UnityEngine;

namespace Players
{
    public abstract class BaseObject
    {
        protected int index;      // 객체 ID
        protected int realm;
        protected string name;
        protected baseObjectData ObjectData;

        protected GameObject modelObject;

        //-------------------------------------------------------------------------
        // 객체 생성        
        public abstract void Create(int _index,string _name, baseObjectData _data,Vector3 pos, int heading,int realm);
        
        //-------------------------------------------------------------------------
        // 객체 제거시 호출됨. 
        public abstract void  OnRemoved();
        
        //-------------------------------------------------------------------------
        // 
        public abstract void OnMoveTo(Vector3 pos, int speed,int heading);
        public abstract void OnAction(Action<BaseObject> action);
    }
}