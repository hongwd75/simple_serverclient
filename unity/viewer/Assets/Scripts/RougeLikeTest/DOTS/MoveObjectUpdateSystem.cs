using Unity.Burst;
using Unity.Entities;
using Unity.Transforms;

namespace Characters.DOTS
{
    [BurstCompile]
    public partial struct MoveObjectUpdateSystem : ISystem
    {
        public void OnUpdate(ref SystemState state)
        {
            new UpdateMoveObjectJob().ScheduleParallel();
        }

        [BurstCompile]
        private partial struct UpdateMoveObjectJob : IJobEntity
        {
            public void Execute(ref LocalTransform transform, in MoveObjectData moveData)
            {
                if (moveData.IsActive)
                {
                    transform.Position = moveData.Position; // 기존 OnSpawn() 위치 업데이트 대체
                }
            }
        }
    }
}