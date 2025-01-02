using System;
using UnityEditor;
/*
 *  사용법
 *  using(RefreshMessageBox.RefreshNow(true))
 *  {
 *    에디터 박스 사용
 *  }
 * 
 */
namespace Utility.Editor.Refresh
{
    public struct RefreshMessageBox : IDisposable
    {
        // 설정 값 저장
        private bool savedSetting;
        private bool refreshAsset;

        public static RefreshMessageBox RefrshNow(bool forceRefresh)
        {
            return new RefreshMessageBox(forceRefresh);
        }
        
        private RefreshMessageBox(bool forceRefresh)
        {
            savedSetting = EditorUserSettings.AutomaticAdd;
            refreshAsset = forceRefresh;
            EditorUserSettings.AutomaticAdd = false;
            AssetDatabase.StartAssetEditing();
            EditorApplication.LockReloadAssemblies();
        }
        
        public void Dispose()
        {
            EditorApplication.UnlockReloadAssemblies();
            AssetDatabase.StopAssetEditing();

            if (refreshAsset == true)
            {
                AssetDatabase.Refresh(ImportAssetOptions.ForceUpdate);
            }
            EditorUserSettings.AutomaticAdd = savedSetting;
        }
    }
}