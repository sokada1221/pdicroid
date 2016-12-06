LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := pdjni
LOCAL_LDLIBS := \
	-llog \
	-landroid \

LOCAL_SRC_FILES := \
	J:/src/Android/Pdic/app/src/main/jni/pdjni.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/android.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/bifile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/biofile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1_utf8.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/bofile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/buffer.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/fexist.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/file.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/filestr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/fixchar.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/flexobj.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/newstr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/ofile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/rexpgen.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/strlib.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tfilebuf.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tifile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/timex_linux.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tiofile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tnarray.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tnassert.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tndefs.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstrbuf.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/tofile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/unix.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/utf.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/uustr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/vbuffer.cpp \
	J:/src/Android/Pdic/app/src/main/jni/tnlib2/cbxe/stdafx.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/BiPool.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/defs.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/defs_android.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/filestr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/jtype.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/jtype1.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/jtype3.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/md5.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/pdconfig.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/pdstrlib.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/pdtime.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/RangeCodec.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/TextResource.cpp \
	J:/src/Android/Pdic/app/src/main/jni/commonLib/wordcount.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Dic.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Dic3.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/DicConv.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dicdata.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Dicgrp.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Dicinx.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Dicixdt.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/diclist.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dicmix.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dicname.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dicobj.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dictype.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/dicUtil.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/faststr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/filebuf.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicdata.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicinx.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/japa.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/japa1.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/kstr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/MainDic.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicLib/multiwd.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicUtil/dictext.cpp \
	J:/src/Android/Pdic/app/src/main/jni/dicUtil/utydic.cpp \
	J:/src/Android/Pdic/app/src/main/jni/History/histfile.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/CCTable.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/IrregDic.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/ktable.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LangProc.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcBase.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcDef.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcMan.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcStd.cpp \
	J:/src/Android/Pdic/app/src/main/jni/LangProc/LPTable.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/msgbox.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/mstr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/pddefs.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/pdfilestr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/pfs.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/regs.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/SrchPatParser.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/txr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom/android/msgbox.cpp \
	J:/src/Android/Pdic/app/src/main/jni/pdcom2/textext.cpp \
	J:/src/Android/Pdic/app/src/main/jni/PdicMain/Dicproc.cpp \
	J:/src/Android/Pdic/app/src/main/jni/PdicMain/android/UIMain.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Popup/PSBookmark.cpp \
	J:/src/Android/Pdic/app/src/main/jni/profile/android/pdprof_android.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Search/srchcom.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Search/SrchMed.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Search/srchstat.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapCommon.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapture.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/Pool.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/PrimaryPool.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/android/SquFrm.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/SquInterface.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/SquItemView.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/android/SquView.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/SquWidthAttr.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/srchproc.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/wdicsqu.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/winsqu.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/winsqu_android.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/wsdisp.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/wsinc.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/wspaint.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/Wsqusrch.cpp \
	J:/src/Android/Pdic/app/src/main/jni/Square/Wsscroll.cpp \

LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\debug\jni
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\bookmark
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\commonLib
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\dicLib
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\dicUtil
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\History
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\LangProc
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\pdcom
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\pdcom2
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\pdic
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\pdicLib
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\PdicMain
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\PdicMain\android
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\Popup
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\profile
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\profile\android
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\tnlib2
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\tnlib2\cbxe
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\Search
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\Square
LOCAL_C_INCLUDES += J:\src\Android\Pdic\app\src\main\jni\Square\android

include $(BUILD_SHARED_LIBRARY)
