LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := pdjni
LOCAL_LDLIBS := \
	-llog \
	-landroid \

LOCAL_SRC_FILES := \
	E:/src/Android/Pdic/app/src/main/jni/pdjni.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/android.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/bifile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/biofile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/bocu1_utf8.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/bofile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/buffer.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/fexist.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/file.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/filestr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/fixchar.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/flexobj.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/newstr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/ofile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/rexpgen.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/strlib.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tfilebuf.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tifile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/timex_linux.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tiofile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tnarray.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tnassert.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tndefs.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tnstrbuf.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/tofile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/unix.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/utf.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/uustr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/vbuffer.cpp \
	E:/src/Android/Pdic/app/src/main/jni/tnlib2/cbxe/stdafx.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/BiPool.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/defs.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/defs_android.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/fileio.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/filestr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/jtype.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/jtype1.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/jtype3.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/md5.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/pdconfig.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/pdstrlib.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/pdtime.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/RangeCodec.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/TextResource.cpp \
	E:/src/Android/Pdic/app/src/main/jni/commonLib/wordcount.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Dic.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Dic3.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/DicConv.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dicdata.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Dicgrp.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Dicinx.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Dicixdt.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/diclist.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dicmix.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dicname.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dicobj.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dictype.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/dicUtil.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/faststr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/filebuf.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicdata.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/Hdicinx.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/japa.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/japa1.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/kstr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/MainDic.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicLib/multiwd.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicUtil/dictext.cpp \
	E:/src/Android/Pdic/app/src/main/jni/dicUtil/utydic.cpp \
	E:/src/Android/Pdic/app/src/main/jni/History/histfile.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/CCTable.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/IrregDic.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/ktable.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LangProc.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcBase.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcDef.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcMan.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LangProcStd.cpp \
	E:/src/Android/Pdic/app/src/main/jni/LangProc/LPTable.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/msgbox.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/mstr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/pddefs.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/pdfilestr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/pfs.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/regs.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/SrchPatParser.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/txr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom/android/msgbox.cpp \
	E:/src/Android/Pdic/app/src/main/jni/pdcom2/textext.cpp \
	E:/src/Android/Pdic/app/src/main/jni/PdicMain/Dicproc.cpp \
	E:/src/Android/Pdic/app/src/main/jni/PdicMain/android/UIMain.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Popup/PSBookmark.cpp \
	E:/src/Android/Pdic/app/src/main/jni/profile/android/pdprof_android.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Search/srchcom.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Search/SrchMed.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Search/srchstat.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapCommon.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/android/MouseCapture.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/Pool.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/PrimaryPool.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/android/SquFrm.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/SquInterface.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/SquItemView.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/android/SquView.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/SquWidthAttr.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/srchproc.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/wdicsqu.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/winsqu.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/winsqu_android.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/wsdisp.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/wsinc.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/wspaint.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/Wsqusrch.cpp \
	E:/src/Android/Pdic/app/src/main/jni/Square/Wsscroll.cpp \

LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\debug\jni
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\bookmark
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\commonLib
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\dicLib
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\dicUtil
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\History
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\LangProc
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\pdcom
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\pdcom2
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\pdic
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\pdicLib
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\PdicMain
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\PdicMain\android
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\Popup
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\profile
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\profile\android
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\tnlib2
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\tnlib2\cbxe
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\Search
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\Square
LOCAL_C_INCLUDES += E:\src\Android\Pdic\app\src\main\jni\Square\android

include $(BUILD_SHARED_LIBRARY)
