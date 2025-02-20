using Unity.Entities;
using UnityEngine;

namespace Characters.DOTS
{
    public class MoveObjectAuthoring : MonoBehaviour
    {
        public MeshRenderer meshRenderer;

        public class Baker : Baker<MoveObjectAuthoring>
        {
            public override void Bake(MoveObjectAuthoring authoring)
            {
                var entity = GetEntity(TransformUsageFlags.Dynamic);
                AddComponent(entity, new MoveObjectData { });
                AddComponentObject(entity, authoring.meshRenderer); 
            }
        }
    }
}