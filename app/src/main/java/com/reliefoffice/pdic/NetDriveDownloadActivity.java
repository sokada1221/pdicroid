package com.reliefoffice.pdic;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.io.File;

// refer to http://qiita.com/mizofumi0411/items/fed5c43fc36c5a945a7b
public abstract class NetDriveDownloadActivity extends AppCompatActivity implements FileSelectionDialog.OnFileSelectListener {

    //static final boolean debugMode = true;

    //public static final Session.AccessType DROPBOX_ACCESS_TYPE = Session.AccessType.APP_FOLDER;
    static final String PFS_INITIALDIR = "DropboxInitialDir";

    static NetDriveDownloadActivity This;

    String m_strInitialDir = "/";

    SharedPreferences pref;
    DicPref dicPref;

    // NetDrive components //
    INetDriveFileManager ndvFM;
    INetDriveUtils ndvUtils;
    boolean fromNetDrive = false;

    abstract void createNetDriveComponents();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dropbox_download);

        This = this;

        createNetDriveComponents();

        pref = PreferenceManager.getDefaultSharedPreferences(this);
        dicPref = new DicPref(pref);
    }

    abstract void selectFile();

    @Override
    public void onFileSelect(FileInfo file) {
        if (fromNetDrive) {
            ndvUtils.setInitialDir(file.getParent());
            String toFileName = ndvUtils.convertToLocalName(file.getAbsolutePath());
            File toFile = new File(toFileName);
            ndvUtils.makeDir(this, toFile);
            executeDownloadFile(file.getAbsolutePath(), toFile);    // execute download file
        } else {
            File dicDir = DicUtils.getDictionaryPath(this);
            if (dicDir == null) return;
            File toFile = new File(dicDir, file.getName());
            downloadFile(file.getAbsolutePath(), toFile);
            Toast.makeText(this, getString(R.string.msg_dictionary_added) + " : " + file.getPath(), Toast.LENGTH_SHORT).show();

            m_strInitialDir = file.getParent();
            SharedPreferences.Editor edit = pref.edit();
            edit.putString(PFS_INITIALDIR, m_strInitialDir);    // execute download file
            edit.commit();
        }
    }

    void downloadFile() {
        if (ndvFM.startAuth(false)) {
            ndvUtils.appKeysConfirmed = true;
            selectFile();
        }
    }
    void executeDownloadFile(String from, File to) {
        downloadFile(from, to);
        createProgressDialog(false);
    }

    void exit(String filename, String downloadedRemoteName){
        Intent i = new Intent();
        i.putExtra("filename", filename);
        i.putExtra("remotename", downloadedRemoteName);
        setResult(Activity.RESULT_OK, i);
        finish();
    }

    // Download Progress Dialog //
    ProgressDialog progress;
    void createProgressDialog(boolean save) {
        progress = new ProgressDialog(this);
        if (save){
            progress.setTitle(getString(R.string.title_prog_file_saving));
            progress.setMessage(getString(R.string.msg_prog_file_saving));
        } else {
            progress.setTitle(getString(R.string.title_prog_file_loading));
            progress.setMessage(getString(R.string.msg_prog_file_loading));
        }
        progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progress.show();
    }
    void deleteProgressDialog() {
        if (progress == null) return;
        progress.dismiss();
        progress = null;
    }

    @Override
    protected void onResume() {
        super.onResume();

        m_strInitialDir = pref.getString(PFS_INITIALDIR, m_strInitialDir);

        if (ndvFM.authComplete(this)) {
            ndvUtils.appKeysConfirmed = true;
            selectFile();
        }
    }

    void downloadFile(String from, File to){
        ndvFM.executeDownload(from, to, new INetDriveFileManager.OnExecuteListener() {
            @Override
            public void onPostExecute(boolean downloaded, String from, File to, String revision) {
                deleteProgressDialog();
                if (!downloaded){
                    // failed to downloada
                    finish();
                } else {
                    // downloaded successfully
                    exit(to.getAbsolutePath(), from);
                }
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_dropbox_download, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
