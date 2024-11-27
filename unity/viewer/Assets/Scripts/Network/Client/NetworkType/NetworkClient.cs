using System;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;
using Google.FlatBuffers;
using NetworkMessage;

namespace Network.Client
{
    public abstract class NetworkClient
    {
        protected readonly ConcurrentQueue<ByteBuffer> _sendQueue = new();
        protected readonly ConcurrentQueue<ByteBuffer> _receiveQueue = new();

        private Task _sendTask;
        private CancellationTokenSource _sendCancellationTokenSource;

        public bool IsConnected { get; protected set; } = false;

        public event Action<ByteBuffer> OnMessageReceived;
        public event Action<Exception> OnError;

        public void SetCallbackFunctions(Action<ByteBuffer> received, Action<Exception> errorCallback)
        {
            OnMessageReceived = received;
            OnError = errorCallback;
        }
        
        public virtual async Task<bool> ConnectAsync(string address, int port)
        {
            IsConnected = true;
            StartSendQueue(); // 큐 시작
            OnConnect(); // 연결 후 호출
            return true;
        }

        public virtual async Task DisconnectAsync()
        {
            if (!IsConnected) return;

            StopSendQueue();

            // 큐 초기화
            _sendQueue.Clear();
            _receiveQueue.Clear();

            IsConnected = false;
            OnDisconnect();
        }

        public void ForceDisconnect()
        {
            DisconnectAsync().Wait();
        }

        // 자식 클래스에서 재정의 가능한 OnConnect
        protected virtual void OnConnect()
        {
        }

        // 자식 클래스에서 재정의 가능한 OnDisconnect
        protected virtual void OnDisconnect()
        {
        }

        public void EnqueueSend(ByteBuffer buffer)
        {
            _sendQueue.Enqueue(buffer);
        }

        public void StartSendQueue()
        {
            if (_sendTask == null || _sendTask.IsCompleted)
            {
                _sendCancellationTokenSource = new CancellationTokenSource();
                _sendTask = Task.Run(ProcessSendQueue, _sendCancellationTokenSource.Token);
            }
        }

        private async Task ProcessSendQueue()
        {
            try
            {
                while (!_sendCancellationTokenSource.Token.IsCancellationRequested)
                {
                    if (_sendQueue.TryDequeue(out var buffer))
                    {
                        await SendInternalAsync(buffer);
                    }
                    else
                    {
                        await Task.Yield();
                    }
                }
            }
            catch (Exception ex)
            {
                RaiseError(ex);
            }
        }

        protected abstract Task SendInternalAsync(ByteBuffer buffer);

        public void StopSendQueue()
        {
            _sendCancellationTokenSource?.Cancel();
            _sendTask?.Wait();
            _sendTask = null;
        }

        protected void EnqueueReceivedMessage(ByteBuffer buffer)
        {
            _receiveQueue.Enqueue(buffer);
        }

        protected void RaiseError(Exception ex)
        {
            OnError?.Invoke(ex);
        }

        public void ProcessReceivedMessages()
        {
            while (_receiveQueue.TryDequeue(out var message))
            {
                OnMessageReceived?.Invoke(message);
            }
        }
    }
}