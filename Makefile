
#  - armeabi, mips‚Í‚¢‚¸‚êíœ

include env.mk

APP_BUILD_SCRIPT_PATH=.\Android.mk

build:
	$(NDK_PATH)ndk-build.cmd NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=$(APP_BUILD_SCRIPT_PATH) APP_PLATFORM=android-21 NDK_OUT=.\app\build\intermediates\ndk\debug\obj NDK_LIBS_OUT=.\app\build\intermediates\ndk\debug\lib APP_STL=gnustl_shared APP_ABI=armeabi,mips,armeabi-v7a,arm64-v8a,x86,x86_64
	$(CP) app/build/intermediates/ndk/debug/lib/armeabi//*.so app/src/main/jniLibs/armeabi/
	$(CP) app/build/intermediates/ndk/debug/lib/armeabi-v7a/*.so app/src/main/jniLibs/armeabi-v7a/
	$(CP) app/build/intermediates/ndk/debug/lib/arm64-v8a/*.so app/src/main/jniLibs/arm64-v8a/
	$(CP) app/build/intermediates/ndk/debug/lib/mips/*.so app/src/main/jniLibs/mips/
	$(CP) app/build/intermediates/ndk/debug/lib/x86/*.so app/src/main/jniLibs/x86/
	$(CP) app/build/intermediates/ndk/debug/lib/x86_64/*.so app/src/main/jniLibs/x86_64/

# for xperia
buildx:
	$(NDK_PATH)ndk-build.cmd NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=$(APP_BUILD_SCRIPT_PATH) APP_PLATFORM=android-21 NDK_OUT=.\app\build\intermediates\ndk\debug\obj NDK_LIBS_OUT=.\app\build\intermediates\ndk\debug\lib APP_STL=gnustl_shared APP_ABI=arm64-v8a
	copy app\build\intermediates\ndk\debug\lib\arm64-v8a\*.so app\src\main\jniLibs\arm64-v8a\\

build8664:
	$(NDK_PATH)ndk-build.cmd NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=$(APP_BUILD_SCRIPT_PATH) APP_PLATFORM=android-21 NDK_OUT=.\app\build\intermediates\ndk\debug\obj NDK_LIBS_OUT=.\app\build\intermediates\ndk\debug\lib APP_STL=gnustl_shared APP_ABI=x86_64
	copy app\build\intermediates\ndk\debug\lib\x86_64\*.so app\src\main\jniLibs\x86_64\\

buildunzip:
	$(NDK_PATH)ndk-build.cmd NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=.\litezip\LiteUnzip\Android.mk APP_PLATFORM=android-21 NDK_OUT=.\litezip\build\obj NDK_LIBS_OUT=.\litezip\build\lib APP_STL=gnustl_shared APP_ABI=armeabi,mips,armeabi-v7a,arm64-v8a,x86,x86_64

clean:
	$(NDK_PATH)ndk-build.cmd NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=$(APP_BUILD_SCRIPT_PATH) APP_PLATFORM=android-21 NDK_OUT=.\app\build\intermediates\ndk\debug\obj NDK_LIBS_OUT=.\app\build\intermediates\ndk\debug\lib APP_STL=gnustl_shared APP_ABI=armeabi,mips,armeabi-v7a,arm64-v8a,x86,x86_64 clean

javah:
	javah -classpath "bin/classes;E:\src\Android\sdk\platforms\android-21\data\layoutlib.jar;E:\src\Android\MyApplication\app\build\intermediates\classes\release\com\example\tnishi\myapplication" com.reliefoffice.pdic.MainActivity

zip:
	zip -r src . -i *.java *.c *.cpp *.h *.xml *.png Makefile *.mk *.gradle *.properties *.bat

deploy:
	copy app\release\app-release.apk pdicroid.apk
	perl -S deploy.pl pdicroid.apk -vapp/build.gradle -hs:\web\sakura-pdic\android\index.html -hkey:Apk @sakura
	perl -S deploy.pl pdicroid.apk -vapp/build.gradle -rename -copy release
#	pause zip‚µ‚Ü‚·
#	make zip
#	zr.bat
#	del zr.bat

deploydbg:
	copy app\release\app-release.apk S:\web\sakura-pdic\android\pdicroid-0.8.27.apk
#	echo upftp > S:\www\cgi-bin\cmdsvr\cmd.txt
