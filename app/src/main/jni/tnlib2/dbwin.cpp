#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include "bc.h"
#pragma	hdrstop
#include "tndefs.h"
#include "mutex.h"

//	DBWin用デバッグモジュール

#if !defined(WIN32)
void DBW( const char *format, ... )
{
	tchar buf[ 1024 ];
	wvsprintf( buf, format, ... );
#if 1	// DBWinバージョン
	OutputDebugString( buf );
#else

#endif
}
#else	// WIN32
#ifndef WINCE
static HWND hWin = NULL;
static const TCHAR *clsname = _T("TDbgMsgForm");
static const TCHAR *winname_wrong = _T("Debug Messanger");
static const TCHAR *winname = _T("Debug Messenger");
#define	clsnameW	clsname
#define	winnameW_wrong	winname_wrong
#define	winnameW	winname
#endif

static bool FindDBGMSG()
{
	hWin = FindWindow( clsname, winname );
	if ( !hWin ){
		hWin = FindWindow( clsname, winname_wrong );
		if ( !hWin ) return false;
	}
	return true;
}

#if 0
static int FindWin( )
{
	return hWin ? 1 : 0;
}
#endif
#ifdef WINCE
void DBW( const char *format, ... )
{}	// dummy
#else
// ANSI version
void DBW( const char *format, ... )
{
	if ( !hWin ) if (!FindDBGMSG()) return;
	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];
	wvsprintfA( buf, format, ap );
	COPYDATASTRUCT cds;
	cds.dwData = 1;	// Indicate String
	cds.cbData = strlen(buf);
	cds.lpData = buf;
	SendMessageA( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
	va_end( ap );
}
void dbwcls()
{
	if ( !hWin ) if (!FindDBGMSG()) return;

	COPYDATASTRUCT cds;
	cds.dwData = 0x12;	// Save&Clear
	cds.cbData = 0;
	cds.lpData = 0;
	SendMessageA( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
}
#endif
#ifdef _UNICODE
void DBW( const tchar *format, ... )
{
#ifndef WINCE
	if ( !hWin ) if (!FindDBGMSG()) return;
#endif
	va_list ap;
	va_start( ap, format );
	tchar buf[ 2048 ];
	wvsprintf( buf, format, ap );
#ifdef WINCE
	_tcscat( buf, _T("\n") );
	OutputDebugString( buf );
#else
	COPYDATASTRUCT cds;
	cds.dwData = 5;	// Indicate unicode String
	cds.cbData = lstrlen(buf)*2;
	cds.lpData = buf;
	SendMessage( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
#endif
	va_end( ap );
}
#endif
#ifndef WINCE
void D8( const void *data, int len )
{
	if ( !hWin ) if (!FindDBGMSG()) return;
	COPYDATASTRUCT cds;
	cds.dwData = 2;	// Indicate byte dump
	cds.cbData = len;
	cds.lpData = (void*)data;
	SendMessage( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
}
#endif	// !WINCE
#endif	// WIN32

static FILE *afile = NULL;

void alog_filename(const char *filename, char *pathbuf)
{
	GetTempPathA( MAX_PATH, pathbuf );
	strcat(pathbuf, filename);
}

void alog_init(const char *filename)
{
	if (afile) return;

	char pathbuf[ MAX_PATH ];
	alog_filename(filename, pathbuf);
	afile = fopen(pathbuf, "wt");
}

void alog_exit()
{
	if (!afile) return;
	fclose(afile);
	afile = NULL;
}

void alog( const char *format, ... )
{
	if (!afile) return;

	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];
	DWORD dw = GetTickCount();
	wsprintfA( buf, "%7d.%03d:", dw/1000, dw%1000);
	wvsprintfA( buf+strlen(buf), format, ap );
	fputs(buf, afile);
	va_end( ap );
}

void elog( const char *format, ... )
{
	va_list ap;
	va_start( ap, format );
	char buf[ 2048 ];

	if ( !hWin ) if (!FindDBGMSG()) return;

	bool formed = false;
	const int tmoffs = 12;		// "%7d.%03d:"
	char *pbuf = buf + tmoffs;

	if (hWin){
		wvsprintfA( pbuf, format, ap );
		formed = true;
		COPYDATASTRUCT cds;
		cds.dwData = 1;	// Indicate String
		cds.cbData = strlen(pbuf);
		cds.lpData = pbuf;
		SendMessageA( hWin, WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)&cds );
	}

	if (afile){
		DWORD dw = GetTickCount();
		wsprintfA( buf, "%7d.%03d", dw/1000, dw%1000);	// The format string length + 1 must match with tmoffs defined above.
		buf[tmoffs-1] = ':';
		if (!formed)
			wvsprintfA( pbuf, format, ap );
		fputs(buf, afile);
	}

	va_end( ap );
}

class TRingBuffer {
protected:
	char *buffer;
	int bufferSize;
	int top;
	int tail;
	int count;
public:
	TRingBuffer(int size)
	{
		buffer = new char[size];
		bufferSize = size;
		top = tail = 0;
		count = 0;
	}
	~TRingBuffer()
	{
		if (buffer) delete[] buffer;
	}
	int getAvailSize() const {
		return bufferSize - count;
	}
	void put(const char *data, int size)
	{
		while (1){
			int len = min(bufferSize-count, size);

			// buffer fullの場合は即exit!!
			if (len==0) break;

			if (len > bufferSize - top)
				len = bufferSize - top;

			memcpy(buffer + top, data, len);
			top += len;
			data += len;
			size -= len;
			count += len;
			if (top == bufferSize) top = 0;
		}
	}
	int get(char *recvbuf, int size)
	{
		int copylen = 0;

		while (1){
			int len = min(count, size);

			if (len==0) break;	// no more data

			if (len > bufferSize - tail)
				len = bufferSize - tail;

			memcpy(recvbuf, buffer + tail, len);
			tail += len;
			recvbuf += len;
			size -= len;
			count -= len;
			if (tail == bufferSize) tail = 0;
		}

		return copylen;
	}
};


class TLogThread {
protected:
	TRingBuffer ringBuffer;
	bool selfDelete;
	bool endReq;
	HANDLE hSem;
	HANDLE hFile;
	TMutex mutex;
	DWORD startTime;
	char buf[1024+16];
public:
	TLogThread(const char *filename, int bufferSize, bool append)
		:ringBuffer(bufferSize)
	{
		selfDelete = false;
		endReq = false;
		hSem = NULL;

		hFile = CreateFileA(filename, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile==INVALID_HANDLE_VALUE){
			return;
		}

		hSem = CreateSemaphore(NULL, 0, 0x10000, NULL);

		// start thread
		DWORD dwThreadId;
		HANDLE h = CreateThread(NULL, 0, threadProc, this, 0, &dwThreadId);
		CloseHandle(h);
	}
	~TLogThread()
	{
		if (hSem) CloseHandle(hSem);
		if (hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile);
	}
	bool failed()
	{
		return hFile == INVALID_HANDLE_VALUE;
	}
	bool put(const char *data, int len)
	{
		{
			TAutoLock lock(mutex);
			int avail = ringBuffer.getAvailSize();
			if (avail < len + 2) return false;
			short l = (short)len;
			ringBuffer.put((char*)&l, sizeof(l));
			ringBuffer.put(data, len);
		}
		wake();
		return true;
	}
	void flush()
	{
		FlushFileBuffers(hFile);
	}
	void terminate(bool selfDelete)
	{
		selfDelete = true;
		endReq = true;
		wake();
	}
protected:
	static DWORD WINAPI threadProc(LPVOID param)
	{
		((TLogThread*)param)->threadEntry();
		return 0;
	}
	void wake()
	{
		ReleaseSemaphore(hSem, 1, NULL);
	}
	void threadEntry()
	{
		startTime = timeGetTime();
		for (;;){
			WaitForSingleObject(hSem, INFINITE);
			if (endReq) break;
			get();
		}
		delete this;
	}
	void get()
	{
		DWORD dw = timeGetTime() - startTime;
		wsprintfA( buf, "%7d.%03d:", dw/1000, dw%1000);
		DWORD written;
		WriteFile(hFile, buf, strlen(buf), &written, NULL);
		short size = 0;
		{
			TAutoLock lock(mutex);
			ringBuffer.get((char*)&size, sizeof(size));
			// assert(size<=sizeof(buf))
			ringBuffer.get(buf, size);
		}
		WriteFile(hFile, buf, size, &written, NULL);
		WriteFile(hFile, "\r\n", 2, &written, NULL);
	}
};

// ------------------------------------------------------------
// trace log
// ------------------------------------------------------------

#define	DefaultQueSize	(0x2000)
TLogThread *tlogThread = NULL;

void tlog_filename(const char *filename, char *pathbuf)
{
	GetTempPathA( MAX_PATH, pathbuf );
	strcat(pathbuf, filename);
}

// これを呼び出さないとtlog()は動作しない
bool tlog_init(const char *filename, int queSize, bool append)
{
	if (tlogThread) return true;	// already running

	char pathbuf[ MAX_PATH ];
	tlog_filename(filename, pathbuf);
	tlogThread = new TLogThread(pathbuf, queSize ? queSize : DefaultQueSize, append);

	if (tlogThread->failed()){
		delete tlogThread;
		tlogThread = NULL;
		return false;
	}
	return true;
}

void tlog_exit()
{
	if (!tlogThread) return;
	tlogThread->terminate(true);
	tlogThread = NULL;
}

void tlog(const char *format, ...)
{
	if (!tlogThread) return;

	va_list ap;
	va_start( ap, format );
	char buf[ 1024+1 ];
	int len = wvsprintfA( buf, format, ap );
	tlogThread->put(buf, len);
	va_end( ap );
}

// 文字列のみのlog
void tlogs(const char *s, int len)
{
	if (!tlogThread) return;

	tlogThread->put(s, len>0 ? len : strlen(s));
}

void tlog_flush()
{
	if (!tlogThread) return;
	tlogThread->flush();
}

