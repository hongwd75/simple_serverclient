using System;
using Network.Client;
using NetworkMessage;
using UnityEngine;

namespace Logic.SceneMain
{
    public class MainNetwork : MonoBehaviour
    {
        private async void Start()
        {
            await NetworkManager.Instance.ConnectWithRetryAsync("localhost", 1000, OnConnected, OnConnectFail);
        }

        private void OnConnected()
        {
            CS_LoginReq_FBS req = new CS_LoginReq_FBS();
            req.Id = "observer";
            req.Pwd = "1002222";
            NetworkManager.Instance.SendPacket(ClientPackets.CS_LoginReq,req);
        }
        
        private void OnConnectFail(int trycount)
        {
            Debug.Log($"=========== 연결 실패 {trycount} =============");
        }
    }
}