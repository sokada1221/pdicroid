LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := pdjni
LOCAL_LDLIBS := \
	-llog \
	-landroid \

LOCAL_SRC_FILES := \
	K:/src/Android/Pdic/app/src/main/jni/pdjni.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/android.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/bifile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/biofile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1_utf8.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/bofile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/buffer.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/fexist.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/file.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/filestr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/fixchar.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/flexobj.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/newstr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/ofile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/rexpgen.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/strlib.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tfilebuf.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tifile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/timex_linux.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tiofile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tnarray.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tnassert.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tndefs.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstrbuf.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/tofile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/unix.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/utf.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/uustr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/vbuffer.cpp \
	K:/src/Android/Pdic/app/src/main/jni/tnlib2/cbxe/stdafx.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/BiPool.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/defs.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/defs_android.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/filestr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/jtype.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/jtype1.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/jtype3.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/md5.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/pdconfig.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/pdstrlib.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/pdtime.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/RangeCodec.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/TextResource.cpp \
	K:/src/Android/Pdic/app/src/main/jni/commonLib/wordcount.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Dic.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Dic3.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/DicConv.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dicdata.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Dicgrp.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Dicinx.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Dicixdt.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/diclist.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dicmix.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dicname.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dicobj.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dictype.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/dicUtil.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/faststr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/filebuf.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicdata.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicinx.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/japa.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/japa1.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/kstr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/MainDic.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicLib/multiwd.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicUtil/dictext.cpp \
	K:/src/Android/Pdic/app/src/main/jni/dicUtil/utydic.cpp \
	K:/src/Android/Pdic/app/src/main/jni/History/histfile.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/CCTable.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/IrregDic.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/ktable.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LangProc.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcBase.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcDef.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcMan.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcStd.cpp \
	K:/src/Android/Pdic/app/src/main/jni/LangProc/LPTable.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/msgbox.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/mstr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/pddefs.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/pdfilestr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/pfs.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/regs.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/SrchPatParser.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/txr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom/android/msgbox.cpp \
	K:/src/Android/Pdic/app/src/main/jni/pdcom2/textext.cpp \
	K:/src/Android/Pdic/app/src/main/jni/PdicMain/Dicproc.cpp \
	K:/src/Android/Pdic/app/src/main/jni/PdicMain/android/UIMain.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Popup/PSBookmark.cpp \
	K:/src/Android/Pdic/app/src/main/jni/profile/android/pdprof_android.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Search/srchcom.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Search/SrchMed.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Search/srchstat.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapCommon.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapture.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/Pool.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/PrimaryPool.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/android/SquFrm.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/SquInterface.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/SquItemView.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/android/SquView.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/SquWidthAttr.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/srchproc.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/wdicsqu.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/winsqu.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/winsqu_android.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/wsdisp.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/wsinc.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/wspaint.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/Wsqusrch.cpp \
	K:/src/Android/Pdic/app/src/main/jni/Square/Wsscroll.cpp \

LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\debug\jni
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\bookmark
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\commonLib
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\dicLib
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\dicUtil
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\History
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\LangProc
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\pdcom
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\pdcom2
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\pdic
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\pdicLib
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\PdicMain
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\PdicMain\android
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\Popup
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\profile
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\profile\android
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\tnlib2
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\tnlib2\cbxe
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\Search
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\Square
LOCAL_C_INCLUDES += K:\src\Android\Pdic\app\src\main\jni\Square\android

include $(BUILD_SHARED_LIBRARY)
