#include <jni.h>
#include <wchar.h>
#include <stdio.h>

JNIEXPORT jint JNICALL Java_com_example_tnishi_myjniapp3_MainActivity_jnitest(JNIEnv* env, jint param1)
{
	int ok = 0;
	FILE *fp = fopen("/data/data/com.example.tnishi.myjniapp3/files/Sample.dic", "r");
	if (fp){
		ok = 1;
	}
	fclose(fp);
	return ok;
}

void testfunc()
{
}

