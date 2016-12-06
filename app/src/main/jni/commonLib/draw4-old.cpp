#if 0	// old
int ParseText(TNFONT &tnfont, TDispLinesInfo &linesinfo, const tchar *str, RECT *rc, TDrawSetting &setting)
{
	HDC hdc = tnfont;

	int cy = tnfont.cy;

	const UINT maxcx = rc->right - rc->left;	// １行の最大ドット数
	int cx = setting.InitLeft;	// １行のｘドット数
	int l = 0;	// 行数
	const tchar *p = str;	// 現在の処理文字列
	const tchar *leftp = p;	// 行の先頭文字列
	int top = rc->top;	// 現在行の頭の座標
	const tchar *breakp = leftp;
	const tchar *newbreakp = breakp;
	UINT bcx = 0;	// ワードブレーク用保存ｘドット数
	// 禁則処理用
	const tchar *kinsokup = leftp;	// 禁則文字用のブレークポインタ
	UINT fw = 0;

#ifndef _UNICODE
	bool fUseAsc = false;
	bool fCurAsc = false;
	if ( tnfont.hAscFont ){
		fUseAsc = true;
	}
#endif
	bool illchar = false;	// 制御コードなどがあったフラグ
	bool tabchar = false;	// TABがあった(TabbedTextOutが遅いためなるべく使わないようにする）

	const int *width = tnfont.width;
	const int tabwidth = tnfont.width[ '\t' ] = width[ ' ' ] * setting.TabCol;
	INT tabstop[1];
	tabstop[0] = tabwidth;
#ifndef _UNICODE
	bool fSingleByte = tnfont.fSingleByte;
#endif

	//Note:
	// 文字コード解析、文字幅算出処理が変更になるときは、
	// HitTestXも変更すること！

	ushort c = 0;
	bool cr = false;
	while ( *p || STR_DIFF( p, leftp ) || cr ){
		const tchar *nextp;
		UINT w;
		cr = false;
		if ( *p ){
#ifndef _UNICODE
			if ( fSingleByte )
				nextp = p+1;
			else
#endif
				nextp = CharNext( p );
#ifdef _UNICODE
			c = *p;
			if ((c>>8)==0)
#else
			if ( FP_DIFF( nextp, p ) == 1 )
#endif
			{
				// single byte //
jmp_eng:
#ifndef _UNICODE
				c = (uchar)*p;
#endif
				w = width[ c ];
				// テーブルにする！！！！
				if ( isalnum( c )
					|| ( (uchar)c >= 0x80 ) ){	// 0x80以降はどれがワードブレークになるかわからないためすべて??
					bcx += w;	// ワードブレーク分のドット幅
				} else if ( c < ' ' ){
					// 制御コード
					if ( c == '\r' ){
						cr = true;
						// 改行
						if (*nextp=='\n'){
							nextp++;
							newbreakp = nextp;
							goto j0;
						}
					} else
					if ( c == '\n' ){
						cr = true;
						// 改行
						// w = 0;
					} else if ( c == '\t' ){
						// タブ
						w = GetTabWidth(cx, tabwidth);
						tabchar = true;
					} else {
						illchar = true;
						// 上記以外は . で代用(1999.5.3)
#if ILLCHAR_AS_PERIOD
						w = width[ILLCHAR];
#else
						// w = 0;
						goto jmp111;
#endif
					}
					newbreakp = nextp;
				} else {
					newbreakp = nextp;
				}
j0:;
#ifndef _UNICODE
				if ( fUseAsc && !fCurAsc ){
					tnfont.SelectAsc();
					fCurAsc = true;
				}
#endif
			} else
			{
#ifndef _UNICODE
				if ( fUseAsc && fCurAsc ){
					tnfont.Select();
					fCurAsc = false;
				}
				c = (((ushort)(*(uchar*)p))<<8) | (ushort)(*(uchar*)(p+1));
#endif
#ifdef WINCE
				SIZE sz;
				if ( GetTextExtentPoint32( hdc, &c, 1, &sz ) )
#else
				if ( !GetCharWidth32( hdc, c, c, (LPINT)&w ) &&
					!GetCharWidth( hdc, c, c, (LPINT)&w ) )
#endif
				{	// MS明朝４ポイントでここでエラーが発生するときがある？？
#ifdef _UNICODE
						w = width[ILLCHAR];
#else
						// エラーの場合、英字フォントとして扱う
						nextp = p + 1;
#ifdef _UNICODE
						c = *p;
#endif
						goto jmp_eng;
#endif
				}
#ifdef WINCE
				w = sz.cx;
#endif
				newbreakp = nextp;
			}
	jmp111:;
		} else {
			// 最終行の場合の処理
			c = 0;
			nextp = p;
			newbreakp = nextp;	// ワードブレークはしないように
			w = 0;
		}
#if 0	//TODO: いらないはずだが、GetTextExtentPoint32()の計算が気になる。。
		if ( etw.CheckHit() ){
			// 横幅の補正
			SIZE sz;
			if ( GetTextExtentPoint32( hdc, leftp, STR_DIFF(p, leftp), &sz ) ){
				cx = sz.cx + nInitLeft;
			}
			if ( flags & DF_CALCRECT ){
			} else {
				// 強調表示範囲のチェック
				etw.SetPoints(STR_DIFF(p,str), cx);
			}
		}
#endif
		//DBW("cx=%d %d",cx,w);
		if ( cx + w > maxcx || !*p || cr ){
			// cxは本当に正しい横幅か調べる(for arabic)
			//TODO: tabがあると正しく動作しない
			SIZE sz;
			if ( GetTextExtentPoint32( hdc, leftp, STR_DIFF(p, leftp), &sz ) )
			{
				// OK
				if (cx-setting.InitLeft!=sz.cx){
					// 計算値が異なる
					// cxの補正
					cx = sz.cx+setting.InitLeft;
					if (cx+w<=maxcx && (*p&&!cr)){
						// 継続
						goto jmp222;
					}
				}
			}

			int ll;	// 表示対象文字数
			if ( newbreakp == nextp ){
				// 通常
#ifdef _UNICODE
				if ( ( IsKinsokuA( c ) ) || ( IsKinsokuJ( c ) ) )
#else
				if ( ( fSingleByte && IsKinsokuA( c ) ) || ( !fSingleByte && IsKinsokuJ( c ) ) )
#endif
				{
					if ( breakp >= kinsokup ){
						// ワードブレークを優先するため、ワードブレークの方が禁則より後ろの場合のみ禁則処理
						newbreakp = kinsokup;
						bcx = fw + w;
					} else {
						newbreakp = breakp;	// 強制的にワードブレーク(禁則とワードブレークのダブリ)
					}
					goto wordbreak;
				} else {
					ll = STR_DIFF( p, leftp );
					bcx = 0;
				}
			} else {
wordbreak:;
				// ワードブレーク！
				if ( leftp == breakp ){
					// １単語が１行のときはそのまま
					ll = STR_DIFF( p, leftp );
					newbreakp = nextp;
					bcx = 0;
				} else {
					ll = STR_DIFF( breakp, leftp );
				}
			}

			int right = cx;	// 実際に表示される文字列の右端(+1ドット)座標(ただし、rc->left分は含んでいない）
			if ( bcx ){
				right += w - bcx;
			}

			if ( linesinfo.MaxRight < right )
				linesinfo.MaxRight = right;

			TDispLineInfo info(
				STR_DIFF(leftp,str),
				ll,
				setting.InitLeft,
				top,
				right,	//TODO: rightで正しい？→もしcxならこの値を参照しているところすべて修正する必要有り
				cr,		//TODO: crでよい？
				illchar,
				tabchar,
				newbreakp != nextp	// wordbroken
				);
			linesinfo.Lines.push_back(info);

			illchar = false;
			tabchar = false;
			setting.InitLeft = 0;
			l++;	// １行プラス
			if ( !*p ){
				break;	// 最終行の場合は終了
			}
			if ( newbreakp != nextp ){
				// ワードブレークした場合
				leftp = breakp;
				p = nextp;
				cx = bcx;	// ワード分(bcxには現ポインターのwも含まれている）
			} else {
				if ( cr ){
					p = nextp;
					leftp = nextp;
				} else
				{
					leftp = p;
					p = nextp;
				}
				cx = w;
				bcx = 0;
				breakp = leftp;
			}
			top += cy;	// 現在行トップ座標更新
			kinsokup = leftp;
			fw = 0;
		} else {
			// 禁則処理用
jmp222:;
			fw = w;
			kinsokup = p;

			if ( breakp != newbreakp ){
				breakp = newbreakp;
				bcx = 0;
			}
			cx += w;
			p = nextp;
		}
	}

	if ( linesinfo.MaxRight < cx )
		linesinfo.MaxRight = cx;
	linesinfo.Height = l*cy;
	linesinfo.LastRight = cx;
	linesinfo.NumLines = l;
	linesinfo.LastTop = top;

	return l*cy;
}

#endif	// old

