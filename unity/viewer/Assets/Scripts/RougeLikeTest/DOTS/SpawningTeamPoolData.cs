using Unity.Entities;

namespace Characters.DOTS
{
    public struct SpawningTeamPoolData : IComponentData
    {
        public int TeamColor;
        public Entity MoveObjectPrefab;
    }
}