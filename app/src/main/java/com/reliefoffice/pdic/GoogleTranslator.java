package com.reliefoffice.pdic;

import android.os.AsyncTask;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class GoogleTranslator extends AsyncTask<String,String,String> {
    String text;
    String apiKey;
    String debugText;
    public GoogleTranslator(String sourceText, String apiKey)
    {
        text = sourceText;
        this.apiKey = apiKey;
    }

    @Override
    protected String doInBackground(String... strings) {
        String baseUrl = "https://translation.googleapis.com/language/translate/v2/?";
        String apikey = "key=" + this.apiKey;
        String srcLang = "&source=en";
        String targetLang = "&target=ja";
        String transChar = "&q=" + text;

        String url = baseUrl + apikey + srcLang + targetLang + transChar;

        String strData = "";

        // OkHttpのインスタンス生成
        OkHttpClient client = new OkHttpClient();

        // urlを含めてリクエストをビルド
        Request request = new Request.Builder()
                .url(url)
                .get()
                .build();

        try {
            debugText = url;
            // リクエスト実行
            Response response = client.newCall(request).execute();
            // レスポンスのbodyからデータ取得
            strData = response.body().string();
            debugText += strData;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return strData;
    }

    @Override
    protected void onPostExecute(String str) {
//        Log.d("PDD", str);

        try{
            JSONObject json = new JSONObject(str);
            final JSONArray translations = json.getJSONObject("data").getJSONArray("translations");
            final JSONObject item0 = translations.getJSONObject(0);
            final String translatedText = item0.getString("translatedText");
            onTranslated(translatedText, debugText);
        }catch(Exception e){
            Log.e("PDD Json Exception",e.getMessage());
        }
    }

    @Override
    protected void onCancelled() {
        super.onCancelled();
        onTranslated(null, null);
    }

    protected void onTranslated(String translatedText, String debugText){
        // Do override
    }
}
