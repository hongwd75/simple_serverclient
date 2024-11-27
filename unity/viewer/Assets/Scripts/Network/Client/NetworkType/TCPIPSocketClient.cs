using System;
using System.Buffers;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;
using Google.FlatBuffers;
using UnityEngine;

namespace Network.Client
{
    public class TCPIPSocketClient : NetworkClient
    {
        private const int BufferSize = 8192; // 버퍼 크기를 8KB로 설정
        private const int HeaderSize = 4;
        
        private TcpClient _tcpClient;
        private NetworkStream _networkStream;

        public override async Task<bool> ConnectAsync(string address, int port)
        {
            try
            {
                _tcpClient = new TcpClient();
                await _tcpClient.ConnectAsync(address, port);
                _networkStream = _tcpClient.GetStream();
                IsConnected = true;

                StartSendQueue();
                StartReceiveLoop();
                Debug.Log("TCP Client connected.");

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

                _networkStream?.Close();
                _tcpClient?.Close();
                _networkStream = null;
                _tcpClient = null;
                IsConnected = false;

                OnDisconnect();
                Debug.Log("TCP Client disconnected.");
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
                if (_networkStream == null || !_networkStream.CanWrite)
                    throw new Exception("NetworkStream is not writable.");

                var data = buffer.ToFullArray();
                await _networkStream.WriteAsync(data, 0, data.Length);
            }
            catch (Exception ex)
            {
                RaiseError(ex);
            }
        }

        private async void StartReceiveLoop()
        {
            byte[] buffer = ArrayPool<byte>.Shared.Rent(BufferSize);
            byte[] headerBuffer = ArrayPool<byte>.Shared.Rent(HeaderSize);
            int remainingBytes = 0;
            int packetSize = 0;

            try
            {
                while (_tcpClient?.Connected == true)
                {
                    int bytesRead = await _networkStream.ReadAsync(buffer, remainingBytes, buffer.Length - remainingBytes);
                    if (bytesRead == 0)
                    {
                        break; // 연결 종료
                    }

                    int processedBytes = 0;
                    int totalBytes = remainingBytes + bytesRead;

                    while (processedBytes < totalBytes)
                    {
                        if (packetSize == 0)
                        {
                            if (totalBytes - processedBytes >= HeaderSize)
                            {
                                Buffer.BlockCopy(buffer, processedBytes, headerBuffer, 0, HeaderSize);
                                packetSize = BitConverter.ToInt32(headerBuffer, 0);
                                processedBytes += HeaderSize;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (totalBytes - processedBytes >= packetSize)
                        {
                            ProcessPacket(new ReadOnlySpan<byte>(buffer, processedBytes, packetSize));
                            processedBytes += packetSize;
                            packetSize = 0;
                        }
                        else
                        {
                            break;
                        }
                    }

                    remainingBytes = totalBytes - processedBytes;
                    if (remainingBytes > 0)
                        Buffer.BlockCopy(buffer, processedBytes, buffer, 0, remainingBytes);
                }
            }
            catch (Exception ex)
            {
                RaiseError(ex);
            }
            finally
            {
                ArrayPool<byte>.Shared.Return(buffer);
                ArrayPool<byte>.Shared.Return(headerBuffer);
            }

            await DisconnectAsync();
        }

        private void ProcessPacket(ReadOnlySpan<byte> packetData)
        {
            var receivedBuffer = new ByteBuffer(packetData.ToArray());
            EnqueueReceivedMessage(receivedBuffer);
        }      
    }
}