#ifdef USE_FORMAT_DISK_COLORS
const ULONG format_disk_colors[96] =
{
	0xaaaaaaaa,0xaaaaaaaa,0xaaaaaaaa,
	0x7f7f7f7f,0x89898989,0xc2c2c2c2,
	0x24242424,0x42424242,0x66666666,
	0xadadadad,0xadadadad,0xf0f0f0f0,
	0xaaaaaaaa,0xaaaaaaaa,0xaaaaaaaa,
	0xe3e3e3e3,0xe3e3e3e3,0xe3e3e3e3,
	0xc6c6c6c6,0xc6c6c6c6,0xc6c6c6c6,
	0x6a6a6a6a,0x6a6a6a6a,0x6a6a6a6a,
	0xdbdbdbdb,0x75757575,0x41414141,
	0xdfdfdfdf,0xbabababa,0x45454545,
	0x52525252,0x66666666,0x94949494,
	0xffffffff,0xffffffff,0xffffffff,
	0xefefefef,0xe7e7e7e7,0x14141414,
	0xdfdfdfdf,0x35353535,0x35353535,
	0x00000000,0x00000000,0x00000000,
	0x8a8a8a8a,0x8a8a8a8a,0x8a8a8a8a,
	0x19191919,0x5e5e5e5e,0x32323232,
	0x38383838,0x88888888,0x46464646,
	0x56565656,0xb1b1b1b1,0x5a5a5a5a,
	0x75757575,0xdbdbdbdb,0x6e6e6e6e,
	0x29292929,0x29292929,0x29292929,
	0x49494949,0x49494949,0x49494949,
	0x82828282,0x00000000,0x00000000,
	0xc2c2c2c2,0x99999999,0xc2c2c2c2,
	0xaaaaaaaa,0x68686868,0xa2a2a2a2,
	0x91919191,0x36363636,0x82828282,
	0xadadadad,0x6d6d6d6d,0xb5b5b5b5,
	0xc7c7c7c7,0x2b2b2b2b,0xd9d9d9d9,
	0x6d6d6d6d,0x2b2b2b2b,0x5b5b5b5b,
	0x91919191,0x00000000,0x6a6a6a6a,
	0xb5b5b5b5,0x48484848,0x6f6f6f6f,
	0xd9d9d9d9,0x00000000,0x4c4c4c4c,
};
#endif

#define FORMAT_DISK_WIDTH        21
#define FORMAT_DISK_HEIGHT       21
#define FORMAT_DISK_DEPTH         5
#define FORMAT_DISK_COMPRESSION   0
#define FORMAT_DISK_MASKING       2

#ifdef USE_FORMAT_DISK_HEADER
const struct BitMapHeader format_disk_header =
{ 21,21,0,0,5,2,0,0,0,44,44,320,256 };
#endif

#ifdef USE_FORMAT_DISK_BODY
const UBYTE format_disk_body[420] = {
0x00,0x00,0x01,0xf0,0x00,0x00,0x01,0xf0,0x00,0x00,0x01,0xf0,0x00,0x00,0x05,
0xf0,0x00,0x00,0x07,0x58,0x00,0x00,0x04,0x50,0x00,0x00,0x04,0xa8,0x00,0x00,
0x05,0x00,0x00,0x00,0x00,0x90,0x00,0x00,0x00,0xa0,0x0a,0xff,0x70,0x00,0x04,
0x00,0x10,0x00,0x01,0xff,0x10,0x00,0x00,0x00,0x0f,0xdf,0x00,0x00,0x00,0x7f,
0x10,0xe2,0xa8,0x00,0x0e,0x1d,0x60,0x00,0x01,0xf3,0x20,0x00,0x0e,0x00,0x5f,
0x9f,0x00,0x00,0x07,0xff,0x10,0xc2,0xd7,0xc0,0x0e,0x3c,0x40,0x00,0x01,0xf3,
0x48,0x00,0x0e,0x0c,0x3f,0xc0,0x00,0x00,0x00,0x00,0x10,0x82,0xa8,0x00,0x0e,
0x7c,0x88,0x00,0x01,0xf3,0x90,0x00,0x0e,0x0c,0x78,0x00,0x00,0x00,0x07,0x20,
0x10,0x03,0x50,0x00,0x0e,0xfd,0x10,0x00,0x01,0xf3,0x28,0x00,0x0e,0x0c,0xf8,
0x00,0x00,0x00,0x07,0xa0,0x10,0x02,0xaf,0xbf,0x0e,0xee,0x20,0x00,0x01,0xf2,
0x50,0x00,0x0e,0x01,0xf8,0x00,0x00,0x00,0x07,0x20,0x10,0x05,0x50,0x00,0x0f,
0xfc,0x40,0x00,0x00,0x04,0xa0,0x00,0x0f,0xfb,0xf8,0x00,0x00,0x00,0x07,0xc0,
0x17,0xfa,0xa0,0x00,0x0c,0x08,0x88,0x00,0x03,0xf9,0x48,0x00,0x08,0x07,0xf8,
0x00,0x00,0x00,0x00,0x00,0x17,0xf5,0x4f,0xff,0x0f,0xf1,0x18,0x00,0x00,0x12,
0x98,0x00,0x0f,0xef,0xf8,0x00,0x00,0x00,0x07,0x80,0x17,0xca,0x88,0x42,0x0f,
0xc2,0x38,0x00,0x00,0x05,0x28,0x00,0x0f,0xdf,0xe8,0x00,0x00,0x20,0x07,0x80,
0x17,0xd5,0x0c,0x00,0x0f,0x84,0x78,0x00,0x00,0x0a,0x48,0x00,0x0f,0x9f,0xe8,
0x00,0x00,0x60,0x00,0x00,0x17,0x4a,0x4f,0xfc,0x0f,0x08,0xb8,0x00,0x00,0x14,
0xc8,0x00,0x0f,0x1f,0xa8,0x00,0x00,0xe0,0x00,0x00,0x17,0x44,0xc8,0x0a,0x0e,
0x01,0x38,0x00,0x01,0x09,0xc8,0x00,0x0f,0x0f,0x28,0x00,0x00,0xf0,0x07,0x00,
0x16,0x21,0xc8,0x00,0x0c,0x70,0x38,0x00,0x02,0x11,0xc8,0x00,0x0f,0x10,0x28,
0x00,0x00,0xee,0x07,0x80,0x14,0x13,0xcf,0x00,0x08,0xa4,0x38,0x00,0x04,0x87,
0xc8,0x00,0x0f,0x84,0x28,0x00,0x00,0x78,0x07,0x00,0x10,0x87,0xc8,0x03,0x00,
0x47,0xdc,0x00,0x08,0xc7,0xe8,0x00,0x07,0xc0,0x08,0x00,0x08,0x3f,0xe0,0x00,
0x11,0xcf,0xd8,0x00,0x18,0x30,0x28,0x01,0x11,0xf0,0x18,0x00,0x17,0xcf,0xc8,
0x00,0x00,0x3f,0xd0,0x00,0x0f,0x8f,0xf7,0xfe,0x08,0x61,0xf0,0x00,0x0f,0xff,
0xf0,0x00,0x0f,0x80,0x70,0x00,0x00,0x7e,0x07,0x80,0x02,0x1f,0xf1,0xf0,0x01,
0xcf,0xf7,0x00,0x03,0xff,0xf0,0x00,0x02,0x03,0xf0,0x00,0x01,0xf0,0x07,0x80,
 };
#endif