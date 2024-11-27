using System;
using UnityEngine;

namespace Utility
{
    public class FollowCamera : MonoBehaviour
    {
        public Transform target; // 따라갈 대상
        public Vector3 offset = new Vector3(0, 66, -66); // 카메라와 대상 사이의 거리
        public float smoothSpeed = 0.125f; // 카메라 이동 속도

        void LateUpdate()
        {
            if (target == true)
            {
                Vector3 desiredPosition = target.position + offset;
                Vector3 smoothedPosition = Vector3.Lerp(transform.position, desiredPosition, smoothSpeed);
                transform.position = smoothedPosition;

                transform.LookAt(target);
            }
        }        
    }
}