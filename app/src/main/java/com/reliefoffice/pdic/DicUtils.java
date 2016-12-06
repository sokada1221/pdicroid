package com.reliefoffice.pdic;

import android.content.Context;
import android.widget.Toast;

import java.io.File;

/**
 * Created by nishikawat on 2015/08/12.
 */
public class DicUtils {
    public static final File getDictionaryPath(Context context){
        File dicDir = context.getExternalFilesDir(null);
        if (!dicDir.exists()){
            if (!dicDir.mkdir()){
                Toast ts = Toast.makeText(context, context.getString(R.string.failed_to_make_dir)+dicDir, Toast.LENGTH_LONG);
                ts.show();
                return null;
            }
        }
        return dicDir;
    }
}
