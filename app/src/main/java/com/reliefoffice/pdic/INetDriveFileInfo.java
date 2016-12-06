package com.reliefoffice.pdic;

import java.io.File;

/**
 * Created by tnishi on 2016/04/09.
 */
abstract public class INetDriveFileInfo {
    public String remoteRevision;
    public DicInfo dicInfo;
    public abstract File getLocalFile();
    public abstract String getLocalName();
    // Dropboxでdownload/uploadされたときに呼ばれる
    public interface UpdateNotifier {
        void downloaded(INetDriveFileInfo ndvInfo);
        void uploaded(INetDriveFileInfo ndvInfo);
    }
}
