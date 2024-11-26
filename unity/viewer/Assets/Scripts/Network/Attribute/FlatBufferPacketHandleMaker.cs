/*
 *  패킷 한글러 생성기 ===================================================================
 *  Editor 폴더에 코드가 있는 경우 Assembly.GetExecutingAssembly에서 클래스를 뽑아 올 수 없다. 
 */

#if UNITY_EDITOR
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Network.Protocol;
using NetworkMessage;
using UnityEditor;
using UnityEngine;

namespace GameEditor.Utility
{
    public class FlatBufferPacketHandleMaker
    {
        [MenuItem("Tools/신규 패킷 핸들러 생성")]
        public static void GenerateFlatBufferPacketHandle()
        {
            string cspath = Application.dataPath + "/Scripts/Network/protocal/Messages/";

            // 패킷 리스트 획득
            List<ServerPackets> packetlist = Enum.GetValues(typeof(ServerPackets)).Cast<ServerPackets>()
                .Where(id => id.ToString() is var idString && idString.StartsWith("SC")).ToList();

            // 존재하는 패킷은 리스트에서 제거
            var assembly = Assembly.GetExecutingAssembly();
            foreach (var type in assembly.GetTypes())
            {
                if (type.IsClass == false) continue;

                if (type.GetInterface("Network.Protocol.IPacketMessage") == null) continue;

                var packetattribute =
                    (PacketMessageAttribute[])type.GetCustomAttributes(typeof(PacketMessageAttribute), true);
                if (packetattribute.Length > 0)
                {
                    packetlist.Remove(packetattribute[0].ID);
                }
            }


            // 신규 패킷 파일 생성
            if (packetlist.Count > 0)
            {
                EditorCoroutine.Start(GeneratePacketHandlersCoroutine(packetlist, cspath));
            }
            else
            {
                EditorUtility.DisplayDialog("패킷 핸들러 생성 ", "신규 패킷이 존재하지 않습니다.", "닫기");
            }
        }

        private static IEnumerator GeneratePacketHandlersCoroutine(List<ServerPackets> packetlist, string cspath)
        {
            for (int i = 0; i < packetlist.Count; i++)
            {
                var item = packetlist[i];
                EditorUtility.DisplayProgressBar($"패킷 핸들러 생성 중....({i}/{packetlist.Count})",
                    $"신규 패킷 {item}을 발견하였습니다.",
                    i / (float)packetlist.Count);

                CreatePacketCSFile(item, cspath);
                yield return null; // 프레임을 넘겨 UI 업데이트
            }

            EditorUtility.ClearProgressBar();
        }

        public static void CreatePacketCSFile(ServerPackets id, string path)
        {
            StringBuilder csfile = new StringBuilder();
            csfile.AppendLine($"// ** {id} 패킷 메시지");
            csfile.AppendLine("using System.Threading.Tasks;");
            csfile.AppendLine("using Google.FlatBuffers;");
            csfile.AppendLine("using NetworkMessage;");
            csfile.AppendLine("");
            csfile.AppendLine("namespace Network.Protocol");
            csfile.AppendLine("{");
            csfile.AppendLine($"\t[PacketMessageAttribute(ServerPackets.{id})]");
            csfile.AppendLine($"\tpublic class {id}_handler : IPacketMessage");
            csfile.AppendLine("\t{");
            csfile.AppendLine("\t\t#pragma warning disable CS1998");
            csfile.AppendLine("\t\tpublic async Task Packet(ByteBuffer buteBuffer)");
            csfile.AppendLine("\t\t#pragma warning restore CS1998");
            csfile.AppendLine("\t\t{");
            csfile.AppendLine("\t\t\t//Todo 코드 작업 필요");
            csfile.AppendLine("\t\t}");
            csfile.AppendLine("\t}");
            csfile.AppendLine("}");

            string savefile = $"{path}{id}_handler.cs";
            StreamWriter writer = new StreamWriter(savefile, false, new UTF8Encoding(true));
            writer.Write(csfile.ToString());
            writer.Flush();
            writer.Close();
        }
    }

    public static class EditorCoroutine
    {
        public static void Start(IEnumerator routine)
        {
            EditorApplication.update += () => UpdateRoutine(routine);
        }

        private static void UpdateRoutine(IEnumerator routine)
        {
            if (!routine.MoveNext())
            {
                EditorApplication.update -= () => UpdateRoutine(routine);
            }
        }
    }
}
#endif