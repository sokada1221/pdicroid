package com.reliefoffice.pdic;

import android.util.Log;

import com.dropbox.client2.DropboxAPI;
import com.dropbox.client2.RESTUtility;
import com.dropbox.client2.exception.DropboxException;
import com.dropbox.client2.exception.DropboxParseException;
import com.dropbox.client2.jsonextract.JsonExtractionException;
import com.dropbox.client2.jsonextract.JsonThing;
import com.dropbox.client2.session.Session;

import org.json.simple.JSONArray;

import java.util.Map;

/**
 * Created by nishikawat on 2015/08/19.
 */
// extended class for DropboxAPI
class DropboxAPIEx<SESS_T extends Session> extends DropboxAPI<SESS_T> {
    public DropboxAPIEx(SESS_T session){
        super(session);
    }

    // path parameter added from super.delta()
    public DropboxAPI.DeltaPage<DropboxAPI.Entry> delta(String cursor, String path) throws DropboxException {
        String[] params = new String[]{"cursor", cursor, "path_prefix", path, "locale", this.session.getLocale().toString()};
        Object json = RESTUtility.request(RESTUtility.RequestMethod.POST, this.session.getAPIServer(), "/delta", 1, params, this.session);

        try {
            return DropboxAPI.DeltaPage.extractFromJson(new JsonThing(json), DropboxAPI.Entry.JsonExtractor);
        } catch (JsonExtractionException var5) {
            throw new DropboxParseException("Error parsing /delta results: " + var5.getMessage());
        }
    }

    public String latest_cursor(String path) throws DropboxException {
        String[] params = new String[]{"path_prefix", path, "locale", this.session.getLocale().toString()};
        Object json = RESTUtility.request(RESTUtility.RequestMethod.POST, this.session.getAPIServer(), "/delta/latest_cursor", 1, params, this.session);

        //try {
            //DropboxAPI.DeltaPage.extractFromJson(new JsonThing(json), DropboxAPI.Entry.JsonExtractor);
            //return DropboxAPI.DeltaPage.extractFromJson(new JsonThing(json), DropboxAPI.Entry.JsonExtractor);
            Map<String, Object> map = (Map)json;
            return (String)map.get("cursor");
        //} catch (JsonExtractionException var5) {
        //    throw new DropboxParseException("Error parsing /delta results: " + var5.getMessage());
        //}
    }
}
