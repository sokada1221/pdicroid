#include "base64.h"

static char basecode[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64enc_size(int inbytes)
{
	return ((inbytes+3)*4)/3;
}
int base64dec_size(int inbytes)
{
	return (inbytes*3+3)/4;
}

int base64enc( const unsigned char *in, int inbytes, char *out )
{
	const unsigned char *in_e = in+inbytes;
	char *_out = out;
	int phase = 0;
	unsigned short c = 0;
	for(;in<in_e;){
		c = (c<<8) | *in++;

		switch (phase++) {
		case 0:
			*out++ = basecode[c>>2];
			c&=3;
			break;
		case 1:
			*out++ = basecode[c>>4];
			c&=0xF;
			break;
		case 2:
			*out++ = basecode[c>>6]; 
			*out++ = basecode[c&0x3F];
			c = 0; phase = 0;
		}
	}
	switch (phase) {
		case 0:
			break; /* nothing in progress */
		case 2:
			*out++ = basecode[c<<2];
			*out++ = '=';
			break;
		case 1:
			*out++ = basecode[c<<4];
			*out++ = '=';
			*out++ = '=';
			break;
	}
	*out = '\0';
	return (int)(out-_out);
}
int base64dec( const char *in, int inbytes, unsigned char *out )
{
	const char*in_e = in+inbytes;
	unsigned char *_out = out;
	int phase = 0;
	unsigned short temp = 0;
	for (;in<in_e;){
		char c = *in++;
		if      (c=='/')           temp = (temp<<6) | 63;
		else if (c=='+')           temp = (temp<<6) | 62;
		else if (c>='A' && c<='Z') temp = (temp<<6) | (c-'A');
		else if (c>='a' && c<='z') temp = (temp<<6) | (c-'a'+26);
		else if (c>='0' && c<='9') temp = (temp<<6) | (c-'0'+52);
		else if (c == '=') { }
		else continue;

		switch (phase++){
			case 0:
				break;
			case 1:
				*out++ = (unsigned char)(temp>>4);
				temp&=0xF;
				break;
			case 2:
				*out++ = (unsigned char)(temp>>2);
				temp&=0x3;
				break; 
			case 3:
				*out++ = (unsigned char)temp;
				phase = 0;
				temp = 0;
				break;
		}
		if (c=='=')
			break;
	}
	return (int)(out-_out);
}
