using System;
using System.Collections;
using System.Collections.Generic;
using Players;
using TMPro;
using UnityEngine;

public class Body : MonoBehaviour
{
    [SerializeField]
    private Animator anim;
    [SerializeField]
    private Renderer _renderer;
    [SerializeField]
    private TextMeshPro _name;

    // 랠름
    private int _realm = 0;
    
    // 이동 관련 
    private bool backStep = false;
    private Vector3 _destination;
    private float _moveSpeed = 0f;
    private float _currentSpeed = 0f;
    private readonly float acceleration = 2f;

    #region 스폰 로직 ===========================================================================
    public void Spawn(int realm, Vector3 pos,int heading,string name)
    {
        _realm = realm;
        _name.text = name;
        
        _name.color = realm switch
        {
            0 => Color.yellow,
            1 => Color.blue,
            2 => Color.red,
            _=> Color.black
        };
        
        _moveSpeed = 0;
        pos *= 0.1f;
        pos.x -= 15f;
        pos.z -= 15f;
        _destination = pos;
        transform.SetPositionAndRotation(pos,Quaternion.Euler(0, heading, 0));  
    }
    #endregion

    #region 이동 로직 ===========================================================================
    public void OnMove(Vector3 pos, int headingTo, int speed)
    {
        pos *= 0.1f;
        pos.x -= 15f;
        pos.z -= 15f;
        MoveTo(pos, headingTo, speed);
    }

    private void MoveTo(Vector3 newPos,int heading,int speed)
    {
        transform.rotation = Quaternion.Euler(0, heading, 0);
        _moveSpeed = (float)speed;
        _destination = newPos;
        
        anim.SetFloat("MoveSpeed",0.3f);
    }
    #endregion

    public void Update()
    {
        if (_currentSpeed > 0f || _moveSpeed > 0f)
        {
            if (_currentSpeed < _moveSpeed)
            {
                _currentSpeed += acceleration * Time.deltaTime;
                if (_currentSpeed > _moveSpeed) _currentSpeed = _moveSpeed;
            }
            else if(_currentSpeed > _moveSpeed)
            {
                _currentSpeed -= acceleration * Time.deltaTime;
                if (_currentSpeed < _moveSpeed) _currentSpeed = _moveSpeed;
            }
            
            transform.Translate((backStep ? Vector3.back : Vector3.forward) * (_currentSpeed * Time.deltaTime));
            if(_moveSpeed == 0f && _currentSpeed == 0f)
            {
                transform.position = _destination;
            }
        }
    }
}
