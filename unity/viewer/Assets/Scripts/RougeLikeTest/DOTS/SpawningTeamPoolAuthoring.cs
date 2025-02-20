using Logic.Portal;
using Unity.Entities;
using UnityEngine;

namespace Characters.DOTS
{
    public class SpawningTeamPoolAuthoring : SpawningPool
    {
        [SerializeField] protected MoveObject.TeamColors color;
        [SerializeField] protected float spawningTime;
 
        public class Baker : Baker<SpawningTeamPoolAuthoring>
        {
            public override void Bake(SpawningTeamPoolAuthoring authoring)
            {
                var entity = GetEntity(TransformUsageFlags.Dynamic); 
                var prefabEntity = GetEntity(authoring.SpawingObject, TransformUsageFlags.Dynamic);

                AddComponent(entity, new SpawningTeamPoolData
                {
                    TeamColor = (int)authoring.color,
                    MoveObjectPrefab = prefabEntity
                });
            }
        }        
    }
}