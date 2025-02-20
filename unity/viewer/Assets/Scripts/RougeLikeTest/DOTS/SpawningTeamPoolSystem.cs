using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;

namespace Characters.DOTS
{
    [UpdateInGroup(typeof(SimulationSystemGroup))]
    public partial struct SpawningTeamPoolSystem : ISystem
    {
        private Unity.Mathematics.Random _random;

        public void OnCreate(ref SystemState state)
        {
            _random = new Unity.Mathematics.Random((uint)System.DateTime.Now.Millisecond);
        }

        public void OnUpdate(ref SystemState state)
        {
            var entityManager = state.EntityManager;
            int activeObjects = SystemAPI.QueryBuilder().WithAll<MoveObjectData>().Build().CalculateEntityCount();

            if (activeObjects < 50000)
            {
                foreach (var (spawner, localTransform) in SystemAPI.Query<SpawningTeamPoolData, LocalTransform>())
                {
                    for (int i = 0; i < 1000; i++) // 각 스포너에서 1000개씩 스폰
                    {
                        var entity = entityManager.Instantiate(spawner.MoveObjectPrefab);
                        float2 spawnPos = RandomPointInAnnulus(new float2(localTransform.Position.x, localTransform.Position.z), 1.5f, 26.5f);

                        entityManager.SetComponentData(entity, new MoveObjectData
                        {
                            Position = new float3(spawnPos.x, 0.4f, spawnPos.y),
                            TeamColorIndex = spawner.TeamColor,
                            IsActive = true
                        });

                        entityManager.SetComponentData(entity, new LocalTransform
                        {
                            Position = new float3(spawnPos.x, 0.4f, spawnPos.y),
                            Rotation = quaternion.identity,
                            Scale = 1f
                        });
                    }
                }
            }
        }

        private float2 RandomPointInAnnulus(float2 origin, float minRadius, float maxRadius)
        {
            float angle = _random.NextFloat(0, math.PI * 2);
            float radius = math.sqrt(_random.NextFloat(minRadius * minRadius, maxRadius * maxRadius));
            return origin + new float2(math.cos(angle), math.sin(angle)) * radius;
        }
    }
}