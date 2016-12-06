package com.reliefoffice.pdic;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Created by nishikawat on 2016/10/12.
 */

public class FileListAdapter extends ArrayAdapter<FileInfo> {
    private List<FileInfo> m_listFileInfo;

    LayoutInflater layoutInflater;
    int resource;

    FileListAdapter(Context context, int resource, List<FileInfo> objects) {
        super(context, resource);

        layoutInflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        this.resource = resource;
        m_listFileInfo = objects;
    }

    @Override
    public int getCount() { return m_listFileInfo.size(); }

    @Override
    public FileInfo getItem( int position )
    {
        return m_listFileInfo.get( position );
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent){
        final FileItemLayout view;
        if (convertView==null){
            view = (FileItemLayout)layoutInflater.inflate(resource, null);
        } else {
            view = (FileItemLayout)convertView;
        }
        view.bindView(getItem(position));
        return view;
    }

    public void sortByName(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                return lhs.getName().compareTo(rhs.getName());
            }
        });
    }
    public void sortByNameR(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                return rhs.getName().compareTo(lhs.getName());
            }
        });
    }
    public void sortByDate(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                return lhs.getModDate().compareTo(rhs.getModDate());
            }
        });
    }
    public void sortByDateR(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                if (lhs.getModDate()==null) return 1;
                if (rhs.getModDate()==null) return -1;
                return rhs.getModDate().compareTo(lhs.getModDate());
            }
        });
    }
    public void sortByRead(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                if (lhs.getReadDate()==null) return 1;
                if (rhs.getReadDate()==null) return -1;
                return lhs.getReadDate().compareTo(rhs.getReadDate());
            }
        });
    }
    public void sortByReadR(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                if (lhs.getReadDate()==null) return 1;
                if (rhs.getReadDate()==null) return -1;
                return rhs.getReadDate().compareTo(lhs.getReadDate());
            }
        });
    }
    public void sortBySize(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                long r = lhs.getFileSize() - rhs.getFileSize();
                if (r==0){
                    return 0;
                }
                return r>0 ? 1 : -1;
            }
        });
    }
    public void sortBySizeR(){
        Collections.sort(m_listFileInfo, new Comparator<FileInfo>() {
            @Override
            public int compare(FileInfo lhs, FileInfo rhs) {
                if (lhs.getName().equals("..")) return -1;
                if (rhs.getName().equals("..")) return 1;
                long r = rhs.getFileSize() - lhs.getFileSize();
                if (r==0){
                    return 0;
                }
                return r>0 ? 1 : -1;
            }
        });
    }
}
