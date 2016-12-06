package com.reliefoffice.pdic;

import android.util.Log;

/**
 * Created by tnishi on 2015/08/30.
 */
public class DicFileManager implements INetDriveFileInfo.UpdateNotifier {
    DicPref dicPref;
    public DicFileManager(DicPref dicPref){
        this.dicPref = dicPref;
    }

    @Override
    public void downloaded(INetDriveFileInfo ndInfo){
        int index = dicPref.findDictionaryIndex(ndInfo.getLocalName());
        if (index==-1){
            Log.d("PDD", "findDictionaryIndex error: file="+ndInfo.getLocalFile());
            return;
        }
        dicPref.saveDicInfo(index, ndInfo.dicInfo);
    }
    @Override
    public void uploaded(INetDriveFileInfo ndInfo){

    }

}
