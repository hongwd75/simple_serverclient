using Players;
using Players.Structs;

namespace Logic.SceneMain
{
    public class EntityManager
    {
        public PlayerInfo myPlayerInfo;
        public ObjectManager zoneObjectManager = new ObjectManager();

        public static EntityManager Instance = new EntityManager();
    }
}