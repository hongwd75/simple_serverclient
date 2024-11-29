using System;
using Players.Structs;
using UnityEditor.SceneTemplate;
using UnityEngine;
using Object = System.Object;

namespace Players.Players
{
    public class GamePlayer : BaseObject
    {
        [NonSerialized]
        protected Body objectControl = null;
        
        public override void Create(int _index,string _name, baseObjectData _data,Vector3 pos, int heading,int _realm)
        {
            index = _index;
            name = _name;
            realm = _realm;
            
            // 지금은 단일 object만 로드되게 된다.
            var loadedojbect = Resources.Load<GameObject>("Players/Player");
            if (loadedojbect != null)
            {
                modelObject = GameObject.Instantiate(loadedojbect);
                objectControl = modelObject?.GetComponent<Body>();
                if (objectControl != null)
                {
                    objectControl.Spawn(realm,pos,heading,name);
                }
            }
        }

        public override void OnRemoved()
        {
            GameObject.Destroy(modelObject);
            modelObject = null;
            objectControl = null;
        }

        public override void OnMoveTo(Vector3 pos, int speed, int heading)
        {
            objectControl?.OnMove(pos,heading,speed);
        }

        public override void OnAction(Action<BaseObject> action)
        {
        }
    }
}