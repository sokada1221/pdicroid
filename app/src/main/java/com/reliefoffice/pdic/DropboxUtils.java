package com.reliefoffice.pdic;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;
import android.widget.Toast;

import com.dropbox.client2.android.AndroidAuthSession;
import com.dropbox.client2.session.AppKeyPair;
import com.reliefoffice.pdic.text.config;

import java.io.File;

/**
 * Created by tnishi on 2015/08/09.
 */
public class DropboxUtils extends INetDriveUtils {
    private static final String PFS_PROXY_FOR_DROPBOX = "ProxyForDropbox";
    private static final String PFS_DBX_APP_KEY = "DbxAppKey";
    private static final String PFS_DBX_APP_SECRET = "DbxAppSecret";
    private static final String PFS_TOKEN = "token";
    private static final String PREF_NAME = "dropbox";

    private static final String PFS_DBX_INITIAL_DIR = "DropboxInitialDir";

    protected static DropboxUtils This;

    // public boolean appKeysConfirmed = false; // moved to super

    protected Context context;
        //TODO: この値がActivityのresume/pauseに正しく対応させていないため潜在的な問題が存在する。onPause(またはonStop)でreleaseInstance()でcontextを無効にする必要があるだろう
        // →それより、getApplicationContext()のほうがいいかもしれない。memory leakの問題もあるし

    static final String PFS_LAST_CURSOR = "LastCursor";
    SharedPreferences pref;

    protected DropboxUtils(Context context) {
        super("dbx:");
        this.context = context;
        pref = context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
    }

/*    public static DropboxUtils createInstance(Context context){
        This = new DropboxUtils(context);
        return  This;
    }*/

    public static DropboxUtils getInstance(Context context){
        return Dropbox2Utils.getInstance(context);
    }

    public AppKeys getAppKeys(){
        AppKeys appKeys = new AppKeys(pref.getString(PFS_DBX_APP_KEY, ""), pref.getString(PFS_DBX_APP_SECRET, ""));
//        if (appKeys.key.equals("tkhiro")){
//
//
//        }
        return appKeys;
    }
    @Override
    public void setAppKeys(AppKeys keys){
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_DBX_APP_KEY, keys.key);
        editor.putString(PFS_DBX_APP_SECRET, keys.secret);
        editor.commit();
    }

    public void storeOauth2AccessToken(String token){
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_TOKEN, token);
        editor.commit();
    }

    public String getAccessToken(){
        return pref.getString(PFS_TOKEN, null);
    }

    public void clearToken(){
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_TOKEN, null);
        editor.commit();
    }

    class ProxyAndroidAuthSession extends AndroidAuthSession {
        ProxyAndroidAuthSession(AppKeyPair appKeys){
            super(appKeys);
        }
        public ProxyAndroidAuthSession(AppKeyPair appKeyPair, String oauth2AccessToken) {
            super(appKeyPair, oauth2AccessToken);
        }
        @Override
        public synchronized ProxyInfo getProxyInfo(){
            if (pref.getBoolean(PFS_PROXY_FOR_DROPBOX, false)) {
                //TODO: proxy直打ち
                return new ProxyInfo("proxy.atg.sony.co.jp", 10080);
            } else {
                return null;
            }
        }
    }

    public AndroidAuthSession loadAndroidAuthSession() {
        String token = pref.getString(PFS_TOKEN, null);
        if (token != null) {
            AppKeys appKeys = getAppKeys();
            AppKeyPair appKeyPair = new AppKeyPair(appKeys.key, appKeys.secret);
            return new ProxyAndroidAuthSession(appKeyPair,token);
        } else {
            return null;
        }
    }

    public AndroidAuthSession createAndroidAuthSession(){
        AppKeys appKeys = getAppKeys();
        AppKeyPair appKeyPair = new AppKeyPair(appKeys.key, appKeys.secret);
        return new ProxyAndroidAuthSession(appKeyPair);
    }

    public boolean hasLoadAndroidAuthSession() {
        return loadAndroidAuthSession() != null;
    }

    public String getCursor(){
        return pref.getString(PFS_LAST_CURSOR, null);
    }
    public void setCursor(String cursor){
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_LAST_CURSOR, cursor);
        editor.commit();
    }

    @Override
    public String getInitialDir(){
        return pref.getString(PFS_DBX_INITIAL_DIR, "/");
    }
    @Override
    public void setInitialDir(String dir){
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(PFS_DBX_INITIAL_DIR, dir);
        editor.commit();
    }

    final static boolean canMkdir = true;  // getExternalFilesDir()の下にmkdirできる場合true
    // return /sdcard/.../dbx  (without last slash)
    public String getLocalDirName(){
        //File dir = DicUtils.getDictionaryPath(context); //TODO: これでいい？
        File dir = context.getExternalFilesDir(null);
        //File dir = Environment.getExternalStorageDirectory();
        //File dir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        return dir.getAbsolutePath() + "/dbx";
    }
    // has /.../dbx/ ?
    @Override
    public boolean isLocalFileForRemote(String name){
        String dir = getLocalDirName() + "/";
        return name.startsWith(dir);
    }
    // [dbx:]/dropbox/... => /sdcard/.../dbx/dropbox/...
    @Override
    public String convertToLocalName(String remoteName){
        if (hasPrefix(remoteName)){
            remoteName = remoteName.substring(4);
        }
        return getLocalDirName() + remoteName;
    }
    // /sdcard/.../dbx/dropbox/... => /dropbox/...
    @Override
    public String convertToRemoteName(String localName){
        String dir = getLocalDirName();
        return localName.substring(dir.length());
    }

    @Override
    public boolean makeDir(Context context, File file){
        File parent = file.getParentFile();
        if (!parent.exists()) {
            if (!parent.mkdirs()) {
                Toast.makeText(context, context.getString(R.string.msg_mkdir_failed) + parent.getAbsolutePath(), Toast.LENGTH_SHORT).show();
                Log.w("PDD", "Failed to mkdirs: " + parent.getAbsolutePath());
                return false;
            }
        }
        return true;
    }
}
