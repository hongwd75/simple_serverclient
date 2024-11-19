using UnityEngine;

namespace Players
{
    public interface IMoveObject
    {
        void OnMove(Vector3Int pos, int headingTo);
        void OnSpawn(Vector3Int pos, int headingTo);
    }
}