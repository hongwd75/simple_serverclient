using System;
using System.Collections;
using Logic.Portal;
using UnityEngine;
using Random = UnityEngine.Random;

namespace Characters
{
    public sealed class SpawningTeamPool : SpawningPool
    {
        [SerializeField] protected MoveObject.TeamColors color;
        [SerializeField] protected float spawningTime;

        protected bool spawning = true;
        void Start()
        {
            StartCoroutine(SpawnLoop());
        }

        private void OnDisable()
        {
            StopCoroutine(SpawnLoop());
        }
        
        public Vector2 RandomPointInAnnulus(Vector2 origin, float minRadius, float maxRadius) {
            Vector2 randomDirection = Random.insideUnitCircle.normalized; // There are more efficient ways, but well
            float minRadius2 = minRadius * minRadius;
            float maxRadius2 = maxRadius * maxRadius;
            float randomDistance = Mathf.Sqrt(Random.value * (maxRadius2 - minRadius2) + minRadius2);
            return origin + randomDirection * randomDistance;
        }
        
        IEnumerator SpawnLoop()
        {
            while (spawning)
            {
                yield return new WaitForSeconds(0.0001f);
                if (RougeLikeObjectPoolManager.PoolManager.ActiveObjectCount < 50000)
                {
                    Vector3 tpos = gameObject.transform.position;
                    Vector2 spos = new Vector2(tpos.x, tpos.z);
                    var pos = RandomPointInAnnulus(spos, 1.5f, 26.5f);

                    var moveObject = RougeLikeObjectPoolManager.PoolManager.Get();
                    if (moveObject != null)
                    {
                        moveObject.SetBase(pos.x, pos.y, color);
                    }
                }
            }

        }
    }
}