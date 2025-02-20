
using Unity.Burst;
using Unity.Collections;
using Unity.Entities;
using UnityEngine;

namespace Characters.DOTS
{
    public partial class MoveObjectRendererSystem : SystemBase
    {
        private Material[] _teamMaterials;

        protected override void OnCreate()
        {
            // 팀별 머티리얼 배열 초기화 (관리형 객체 사용 가능)
            _teamMaterials = new Material[]
            {
                Resources.Load<Material>("Materials/MobMaterial"),
                Resources.Load<Material>("Materials/TeamRed"),
                Resources.Load<Material>("Materials/TeamBlue"),
                Resources.Load<Material>("Materials/TeamGreen")
            };
        }

        protected override void OnUpdate()
        {
            var entityManager = EntityManager;
            foreach (var (moveData, entity) in SystemAPI.Query<MoveObjectData>().WithEntityAccess())
            {
                if (entityManager.HasComponent<MeshRenderer>(entity))
                {
                    var meshRenderer = entityManager.GetComponentObject<MeshRenderer>(entity);
                    if (meshRenderer != null)
                    {
                        meshRenderer.sharedMaterial = _teamMaterials[moveData.TeamColorIndex];
                    }
                }
            }
        }
    }
}