package com.reliefoffice.pdic;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.PopupMenu;


public class FileHistoryActivity extends AppCompatActivity {

    ListView psbList;
    private PSBookmarkListAdapter psbListAdapter;   // �蔲��������psbokmark�?p
    int lastSel;
    boolean savedResult;
    FileHistoryManager fileHistory;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_history);

        // Create ListView for bookmark list
        psbListAdapter = new PSBookmarkListAdapter(this, R.layout.list_item_psbookmarklist);
        psbList = (ListView) findViewById(R.id.fileHistoryList);
        psbList.setAdapter(psbListAdapter);
        psbList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                exit(position);
            }
        });
        psbList.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> adapterView, View view, int position, long l) {
                lastSel = position;
                contextMenu(view);
                return true;
            }
        });

        fileHistory = new FileHistoryManager(this);
        load();
        if (fileHistory.size()==0){
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            //builder.setTitle("");
            builder.setMessage(getString(R.string.msg_no_history));
            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
            builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
                @Override
                public void onCancel(DialogInterface dialog) {
                    finish();
                }
            });
            //builder.setView( listview );
            builder.show();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_file_history, menu);
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

    void exit(int position){
        Intent i = new Intent();
        PSBookmarkItem item = psbListAdapter.getItem(position);
        if (Utility.isNotEmpty(item.filename)){
            INetDriveUtils ndvUtils = DropboxUtils.getInstance(this);
            if (ndvUtils.hasPrefix(item.filename)){
                i.putExtra("filename", ndvUtils.convertToLocalName(item.filename));
                i.putExtra("remotename", item.filename.substring(4));
            } else {
                i.putExtra("filename", item.filename);
            }
        } else {
            i.putExtra("start", item.position);
            i.putExtra("length", item.length);
        }
        setResult(Activity.RESULT_OK, i);
        savedResult = true;
        finish();
    }

    void load(){
        for (int i=0;i<fileHistory.size();i++){
            String filename = fileHistory.get(i);
            psbListAdapter.add(new PSBookmarkItem(filename));
        }
    }

    void contextMenu(final View view){
        // PopupMenu : refer to http://techbooster.org/android/ui/3056/
        PopupMenu popup = new PopupMenu(this, view);
        popup.getMenuInflater().inflate(R.menu.psb_context_menu, popup.getMenu());
        popup.show();
        popup.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.removeItem) {
                    removeItem(lastSel);
                }
                return true;
            }
        });
    }

    void removeItem(int index){
        // delete from file
    }
}
