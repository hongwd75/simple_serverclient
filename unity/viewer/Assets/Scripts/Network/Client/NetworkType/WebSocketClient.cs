using System;
using System.Collections.Concurrent;
using System.IO;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;
using Google.FlatBuffers;
using UnityEngine;

namespace Network.Client
{
    public class WebSocketClient : NetworkClient
    {
    private ClientWebSocket _webSocket;
    private CancellationTokenSource _cancellationTokenSource;

    public override async Task<bool> ConnectAsync(string address, int port)
    {
        try
        {
            _webSocket = new ClientWebSocket();
            _cancellationTokenSource = new CancellationTokenSource();

            var uri = new Uri($"ws://{address}:{port}");
            await _webSocket.ConnectAsync(uri, _cancellationTokenSource.Token);
            IsConnected = true;

            StartSendQueue();
            StartReceiveLoop();
            Debug.Log("WebSocket connected.");

            OnConnect(); // 연결 후 호출
            return true;
        }
        catch (Exception ex)
        {
            RaiseError(ex);
            return false;
        }
    }

    public override async Task DisconnectAsync()
    {
        if (!IsConnected) return;

        try
        {
            StopSendQueue();
            _sendQueue.Clear();
            _receiveQueue.Clear();

            if (_webSocket != null && _webSocket.State == WebSocketState.Open)
            {
                await _webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Disconnect", CancellationToken.None);
            }

            _webSocket?.Dispose();
            _webSocket = null;
            _cancellationTokenSource?.Cancel();
            _cancellationTokenSource?.Dispose();
            _cancellationTokenSource = null;
            IsConnected = false;

            OnDisconnect();
            Debug.Log("WebSocket disconnected.");
        }
        catch (Exception ex)
        {
            RaiseError(ex);
        }
    }

    protected override async Task SendInternalAsync(ByteBuffer buffer)
    {
        try
        {
            if (_webSocket == null || _webSocket.State != WebSocketState.Open)
                throw new Exception("WebSocket is not connected.");

            var data = buffer.ToArraySegment(0, buffer.Length);
            await _webSocket.SendAsync(data, WebSocketMessageType.Binary, true, _cancellationTokenSource.Token);
        }
        catch (Exception ex)
        {
            RaiseError(ex);
        }
    }

    private async void StartReceiveLoop()
    {
        var buffer = new byte[1024];
        var receivedData = new MemoryStream(); // 조립용 메모리 스트림
        while (_webSocket?.State == WebSocketState.Open)
        {
            try
            {
                var result = await _webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), _cancellationTokenSource.Token);
                // 수신 데이터 추가
                receivedData.Write(buffer, 0, result.Count);
                
                if (result.MessageType == WebSocketMessageType.Close)
                {
                    Debug.Log("WebSocket closed by server.");
                    break;
                }
                else
                {
                    if (result.EndOfMessage == true)
                    {
                        var receivedBuffer = new ByteBuffer(receivedData.ToArray());
                        EnqueueReceivedMessage(receivedBuffer);
                    }
                }
            }
            catch (Exception ex)
            {
                RaiseError(ex);
                break;
            }
        }

        await DisconnectAsync();
    }
    }
}