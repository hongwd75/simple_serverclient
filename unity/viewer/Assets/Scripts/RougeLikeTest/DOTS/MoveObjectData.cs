using Unity.Entities;
using Unity.Mathematics;

namespace Characters.DOTS
{
    public struct MoveObjectData : IComponentData
    {
        public float3 Position;
        public int TeamColorIndex;
        public bool IsActive;
        public Entity MeshRendererEntity;
    }
}