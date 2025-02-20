using System;
using UnityEngine;

namespace Logic.Portal
{
    public class SpawningPool : MonoBehaviour
    {
        [SerializeField] private GameObject spawingObject;
        public GameObject SpawingObject => spawingObject;
        
        private void Start()
        {

        }
    }
}