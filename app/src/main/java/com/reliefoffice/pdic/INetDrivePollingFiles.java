package com.reliefoffice.pdic;

/**
 * Created by nishikawat on 2016/05/25.
 */
public abstract class INetDrivePollingFiles {
    public abstract void add(String localName, String remoteName, String remoteRevision, DicInfo refInfo, INetDriveFileInfo.UpdateNotifier update);
}
