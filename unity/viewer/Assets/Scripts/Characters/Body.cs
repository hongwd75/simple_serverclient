using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Body : MonoBehaviour
{
    [SerializeField]
    private Animator anim;
    [SerializeField]
    private Renderer _renderer;
    


    public void MoveTo(Vector3 newPos,int heading)
    {
        transform.SetPositionAndRotation(newPos,Quaternion.Euler(0, heading, 0));   
    }
}
