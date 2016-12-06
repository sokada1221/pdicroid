#ifndef __BASE64_H
#define	__BASE64_H

int base64enc_size(int inbytes);
int base64dec_size(int inbytes);
int base64enc( const unsigned char *in, int inbytes, char *out );
int base64dec( const char *in, int inbytes, unsigned char *out );

#endif	// __BASE64_H

