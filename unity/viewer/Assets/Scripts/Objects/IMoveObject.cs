﻿using UnityEngine;

namespace Players
{
    public interface IMoveObject
    {
        void OnMove(Vector3Int pos, int headingTo, int speed);
    }
}