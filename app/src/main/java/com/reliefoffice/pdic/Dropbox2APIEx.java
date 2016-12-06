package com.reliefoffice.pdic;

import com.dropbox.core.DbxRequestConfig;
import com.dropbox.core.v2.DbxClientV2;

/**
 * Created by tnishi on 2016/09/08.
 */
// DbxClientV2はfinal宣言されているため派生できずorz
public class Dropbox2APIEx /*extends DbxClientV2*/ {
    public Dropbox2APIEx(DbxRequestConfig requestConfig, String accessToken){
        //super(requestConfig, accessToken);
    }
}
