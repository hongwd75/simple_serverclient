using System;
using Property;
using UnityEngine;

namespace Characters
{
    public class MoveObject : MonoBehaviour
    {
        public enum TeamColors
        {
            Mob = 0,
            Red,
            Blue,
            Green,
        }
        
        [SerializeField]
        [EnumNamedArray( typeof(TeamColors) )]
        protected Material[] TeamColor = new Material[(int)sizeof(TeamColors)];

        [SerializeField]
        protected MeshRenderer meshRenderer;

        protected bool activeSpawnObject = false;

        public void SetTeam(TeamColors color)
        {
            meshRenderer.sharedMaterial = TeamColor[(int)color];
        }

        #region Spawn / Despawn
        public void Init()
        {
            
        }

        public virtual void SetBase(float x, float y, TeamColors color)
        {
            OnSpawn(new Vector3(x, 0.4f, y), color);
        }
        
        protected virtual void OnSpawn(Vector3 pos, TeamColors color)
        {
            transform.position = pos;
            SetTeam(color);
            activeSpawnObject = true;
        }

        protected virtual void OnDespawn()
        {
            activeSpawnObject = false;
            if (meshRenderer.sharedMaterial != null)
            {
                meshRenderer.sharedMaterial = null;
            }
            if (gameObject.activeSelf == true)
            {
                gameObject.SetActive(false);
            }
        }
        #endregion
        
        protected void OnDestroy()
        {
            if (activeSpawnObject == true)
            {
                OnDespawn();
            }
        }
    }
}