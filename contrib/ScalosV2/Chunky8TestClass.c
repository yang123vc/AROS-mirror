// :ts=4 (Tabsize)
// derived from area class

/*
** Amiga Workbench� Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, J�rg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <intuition/classusr.h>
#include <intuition/screens.h>
#include <guigfx/guigfx.h>

#include "Scalos.h"
#include "Chunky8TestClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"
#include "Debug.h"

#include "scalos_protos.h"

// ARTPRO palette :: depth: 8bit
//                   type: pure
//                   width: long
// we have to define this as a struct and not like the chunky data array because "sizeof expr" won`t work otherwise
struct palette
{
  ULONG entry;
} chunky_8bit_palette[] =
{
                                                                                        0x00000000,0x00001010,0x00001818,0x00002118,0x00002121,0x00002942,0x00003129,0x00003939,
                                                                                        0x00080808,0x00101000,0x00101810,0x00102129,0x00182929,0x00183142,0x00183939,0x00183952,
                                                                                        0x00212108,0x00212121,0x00212910,0x00212918,0x00213118,0x00213131,0x00213939,0x00213952,
                                                                                        0x0021425A,0x00291800,0x00292908,0x00292918,0x00292929,0x00293110,0x00293942,0x00294A63,
                                                                                        0x00313131,0x00313921,0x00313939,0x00314221,0x00314242,0x00314A4A,0x00394229,0x00395231,
                                                                                        0x00395252,0x0039525A,0x00395A6B,0x00395A73,0x00422908,0x00423110,0x00423921,0x00424A21,
                                                                                        0x00424A29,0x00424A31,0x00424A4A,0x00426363,0x00426373,0x0042637B,0x00426B7B,0x004A3108,
                                                                                        0x004A4208,0x004A4A31,0x004A5239,0x004A5252,0x004A6B7B,0x004A6B84,0x00523908,0x00523910,
                                                                                        0x00524218,0x00524221,0x00524A29,0x00525239,0x00525252,0x00525A73,0x00526394,0x00527373,
                                                                                        0x00527B84,0x005A4208,0x005A4210,0x005A5210,0x005A5A5A,0x005A6373,0x005A6384,0x005A6B6B,
                                                                                        0x005A7B7B,0x005A7B84,0x00634208,0x00636B9C,0x00637331,0x00637373,0x0063848C,0x00638C94,
                                                                                        0x006B4A10,0x006B5208,0x006B5229,0x006B5A10,0x006B5A39,0x006B6339,0x006B634A,0x006B6B6B,
                                                                                        0x006B7384,0x006B7394,0x006B73A5,0x006B9494,0x006B9C9C,0x006B9CA5,0x00734A10,0x00735218,
                                                                                        0x00735221,0x00735A31,0x00736342,0x0073736B,0x00737B7B,0x00737B84,0x00737B9C,0x00738484,
                                                                                        0x00738C8C,0x00739CA5,0x0073A5A5,0x0073A5AD,0x007B5210,0x007B5A18,0x007B5A29,0x007B6339,
                                                                                        0x007B7308,0x007B7352,0x007B7B29,0x007B8473,0x007B84A5,0x007B84AD,0x007B8CB5,0x007B8CC6,
                                                                                        0x007BADAD,0x007BB5B5,0x00845A10,0x00846300,0x00846321,0x00846B31,0x00846B42,0x00847352,
                                                                                        0x00847B29,0x00848473,0x00848C8C,0x00849494,0x0084B5B5,0x008C5A08,0x008C7331,0x008C7339,
                                                                                        0x008C734A,0x008C7B5A,0x008C8CA5,0x008C947B,0x008C948C,0x008C94BD,0x008C9C9C,0x008CA594,
                                                                                        0x008CADAD,0x008CADB5,0x00946310,0x00946B21,0x00947331,0x00947339,0x00948463,0x00949473,
                                                                                        0x00949494,0x00949C8C,0x00949CC6,0x0094A5CE,0x0094BDBD,0x009C7321,0x009C7329,0x009C7331,
                                                                                        0x009C7B4A,0x009C8408,0x009C9C5A,0x009C9C8C,0x009C9C94,0x009C9C9C,0x009CA5BD,0x009CADAD,
                                                                                        0x009CADD6,0x009CC6C6,0x00A56B18,0x00A57318,0x00A57B31,0x00A5844A,0x00A5845A,0x00A58C63,
                                                                                        0x00A59408,0x00A5946B,0x00A59473,0x00A5A5A5,0x00A5ADBD,0x00A5BDC6,0x00AD7318,0x00AD7B29,
                                                                                        0x00AD8439,0x00AD844A,0x00ADADA5,0x00ADADAD,0x00ADB5AD,0x00ADBDBD,0x00ADC6C6,0x00ADCECE,
                                                                                        0x00B58C52,0x00B58C5A,0x00B59463,0x00B5B5B5,0x00BD8418,0x00BD8429,0x00BD8C39,0x00BD9C63,
                                                                                        0x00BD9C73,0x00BDA518,0x00BDA57B,0x00BDB5A5,0x00BDBDBD,0x00BDCECE,0x00BDD6D6,0x00C68C31,
                                                                                        0x00C69442,0x00C69C52,0x00C6A56B,0x00C6BD42,0x00C6C6C6,0x00C6DEDE,0x00CE8C21,0x00CE9431,
                                                                                        0x00CEA55A,0x00CEAD7B,0x00CEB58C,0x00CECEC6,0x00CECECE,0x00CEE7E7,0x00D69C39,0x00D69C42,
                                                                                        0x00D6A552,0x00D6AD6B,0x00D6B57B,0x00D6BD8C,0x00D6D6D6,0x00D6E7E7,0x00DEA542,0x00DEAD63,
                                                                                        0x00DEB56B,0x00DEB573,0x00DEC608,0x00DEC68C,0x00DEDEDE,0x00E7BD84,0x00E7C694,0x00E7DEDE,
                                                                                        0x00E7E7E7,0x00EFCE94,0x00EFEFEF,0x00F7D69C,0x00F7F7F7,0x00FFE7A5,0x00FFE7B5,0x00FFFFFF,
};

// ARTPRO chunky:: type: byte
//                 byte type: right
//                 width: words

UWORD chunky_8bit_data[] =
{
        0x0812,0x1014,0x1426,0x2621,0x2639,0x3012,0x1212,0x1212,
        0x1212,0x1212,0x1212,0x1210,0x2114,0x232F,0x231B,0x1B23,
        0x1212,0x120A,0x1212,0x121D,0x1212,0x1212,0x1212,0x0A12,
        0x1212,0x0A12,0x1212,0x120A,0x1212,0x1212,0x120A,0x1012,
        0x235F,0x5EAB,0x6D32,0xAC44,0x5E31,0x120A,0x1414,0x1412,
        0x1414,0x234C,0x6BB9,0xDAF1,0xF0DA,0xDAB6,0x5D23,0x120A,
        0x1412,0x1214,0x1212,0x1212,0x141D,0x1212,0x1212,0x1214,
        0x1212,0x1012,0x1210,0x0A10,0x1212,0x0A12,0x3A6B,0x44CB,
        0x4C20,0x946B,0xCBC3,0x6B26,0x1214,0x1212,0x0A0A,0x5EE2,
        0xE1C9,0xA890,0xA8A8,0xC9EF,0xDAA8,0x5427,0x1414,0x1412,
        0x0A12,0x1412,0x1212,0x120A,0x1008,0x0110,0x0A12,0x0A0A,
        0x0A0A,0x100A,0x1412,0x1214,0x2244,0x246F,0xAC5F,0x4C24,
        0x94FA,0xD443,0x0808,0x0108,0x0121,0xEAE2,0x3908,0x0208,
        0x0208,0x0121,0x86F1,0xEAC9,0x2F23,0x1414,0x1412,0x1414,
        0x1412,0x1414,0x1412,0x0000,0x1008,0x1212,0x1212,0x0A12,
        0x1414,0x1415,0x2043,0x2220,0xA1BB,0x6C44,0x6B7B,0x940A,
        0x0808,0x0101,0x1BDA,0x9F13,0x0A1E,0x0C15,0x0C0A,0x0A08,
        0x0141,0xCFF1,0xEF77,0x3023,0x1423,0x435F,0x5531,0x1212,
        0x1212,0x1212,0x0A10,0x0A08,0x1008,0x1212,0x1414,0x1331,
        0x241C,0x201C,0x1144,0xC3D4,0x6D24,0x1C08,0x010A,0x0A0A,
        0x87FD,0x110E,0x250C,0x150C,0x1115,0x0C02,0x0808,0x0A42,
        0xDAF0,0xCF43,0x265F,0xDCEC,0xDC97,0x4C3A,0x2314,0x1414,
        0x1414,0x1214,0x0A08,0x0808,0x1426,0x2E3A,0x1211,0x201C,
        0x2444,0x6BAD,0xAC32,0x1C08,0x0A0A,0x0019,0xCF86,0x0A11,
        0x0C0A,0x0C16,0x0A0A,0x0A0A,0x0A0A,0x0808,0x2186,0xF1EF,
        0x906B,0x94A1,0xA1BB,0x9494,0x5527,0x2314,0x2314,0x1414,
        0x0A10,0x1214,0x2379,0x6B32,0x4CD4,0x944C,0x201C,0x3220,
        0x946F,0x1300,0x0808,0x0186,0xC809,0x0A08,0x080A,0x0A0A,
        0x161E,0x1711,0x010A,0x020A,0x0808,0x5AF1,0xE142,0x6BBB,
        0xE4EC,0xECD4,0xAC5F,0x2323,0x2312,0x2312,0x1212,0x0A10,
        0x1289,0x6C6B,0xADDC,0xC26D,0x5F13,0x2422,0x6D44,0x0808,
        0x0801,0x21F1,0x2E01,0x0802,0x0A08,0x080C,0x150C,0x1608,
        0x0A0C,0x0A0A,0x0A01,0x0141,0xE7C0,0x434C,0x4C4C,0xA0E4,
        0xF4D4,0xA07B,0x5E26,0x1412,0x0A14,0x120A,0x0A3A,0x6B5F,
        0x1C0A,0x1C6B,0x2224,0x4408,0x3243,0x0801,0x0800,0xB6B6,
        0x0808,0x0808,0x080C,0x0C08,0x0A08,0x0C25,0x0C15,0x0C0C,
        0x0B0A,0x0A08,0x109A,0xCC77,0x110A,0x1C22,0x4CBB,0xE4D4,
        0x9789,0x2714,0x1414,0x1414,0x0A1C,0x5F4C,0x201C,0x0A13,
        0x2044,0x6D20,0x6B3A,0x0A08,0x002D,0xD82E,0x0008,0x0808,
        0x080A,0x0A0A,0x1E0C,0x1115,0x0C16,0x1616,0x0C0C,0x0A0A,
        0x022D,0xCCB2,0x1C44,0x4C20,0x224C,0x6FAD,0x6C93,0x5F31,
        0x2314,0x2314,0x0808,0x444C,0x4432,0x1C1C,0x2008,0x6FF7,
        0xC21C,0x0800,0x08B2,0x4A00,0x0802,0x0808,0x0108,0x0A0C,
        0x0A0A,0x0808,0x1516,0x110A,0x0A0A,0x0B0A,0x0B0A,0x67CC,
        0x435F,0xBB6F,0x2424,0x896B,0x4C6C,0x9344,0x2314,0x1423,
        0x1000,0x396C,0x7B4C,0x4C20,0x130A,0x226B,0x3219,0x0A00,
        0x66DE,0x1A01,0x080A,0x0808,0x0808,0x0808,0x0808,0x0A0C,
        0x160A,0x0A0B,0x0A16,0x0C0A,0x0A08,0x0AA5,0xB244,0xA17B,
        0x2032,0x4C4C,0x446C,0x7B4C,0x2323,0x2323,0x2312,0x1B9F,
        0xE3CB,0xAD32,0x2239,0x6819,0x3F67,0x588D,0x8D19,0x0108,
        0x0808,0x0108,0x0808,0x0808,0x0808,0x080C,0x0C0A,0x0C11,
        0x0D1C,0x0C0C,0x0408,0x0813,0xCC75,0x0813,0x1C22,0x3222,
        0x111C,0x9493,0x2723,0x2323,0x1414,0x2330,0x89C2,0xAD43,
        0x42A6,0xB275,0xB23E,0x3752,0x0802,0x0808,0x0A08,0x0108,
        0x080A,0x0208,0x0802,0x0808,0x1608,0x0A02,0x1116,0x0A11,
        0x0A0A,0x0408,0x58B2,0x1B20,0x201C,0x1C11,0x201C,0x94C4,
        0x6B3A,0x2323,0x1214,0x1214,0x1223,0x210A,0x0810,0x2113,
        0x0800,0x0100,0x0111,0x080A,0x110A,0x201C,0x2022,0x2000,
        0x0108,0x1125,0x3B0C,0x010A,0x080A,0x0A0B,0x0A0A,0x0808,
        0x0A9A,0x694C,0x2022,0x5F6B,0x4C32,0x4424,0xBB6B,0x2721,
        0x1214,0x140A,0x1414,0x120A,0x0A0A,0x2126,0x0A08,0x0808,
        0x110A,0x0A13,0x131C,0x435E,0x4339,0x1C08,0x0808,0x0C2A,
        0x600A,0x080A,0x0A08,0x0A0A,0x0B0A,0x0808,0x084A,0xBF20,
        0x1324,0x7B94,0x4C44,0x4C44,0x205F,0x4327,0x2314,0x1414,
        0x1212,0x141B,0x1B14,0x2326,0x1308,0x0808,0x1311,0x4311,
        0x0A09,0x1143,0xE1EA,0x9187,0x2E00,0x0A11,0x0A08,0x0808,
        0x0408,0x0A0A,0x0C08,0x0808,0x0808,0x9A40,0x111C,0x224C,
        0x4432,0x6D6C,0x24A1,0x4F3A,0x1414,0x2314,0x1414,0x1423,
        0x2323,0x2323,0x1308,0x0A0A,0x0811,0x5E43,0x431B,0x2E91,
        0x919E,0x5C87,0xCA43,0x0A01,0x080A,0x0B0A,0x0A08,0x080B,
        0x080A,0x0802,0x0808,0x4A9A,0x0A0A,0x0A13,0x111C,0x4332,
        0x3222,0x4C43,0x2323,0x2323,0x2323,0x2323,0x2323,0x2323,
        0x210A,0x080A,0x2612,0x111B,0xB9B9,0xBA6A,0x1208,0x39A8,
        0xB7B6,0x0808,0x080A,0x1E0C,0x0A0A,0x080A,0x0A0A,0x0808,
        0x0801,0x099A,0x1B0A,0x0A0A,0x1113,0x326B,0x5F24,0x7927,
        0x2323,0x2323,0x2323,0x2323,0x2323,0x2323,0x1B0A,0x0A11,
        0x111B,0x0808,0x919E,0xD02E,0x1311,0x87B7,0xB6CA,0x2108,
        0x0215,0x0C28,0x150A,0x010A,0x0A08,0x0801,0x0808,0x0066,
        0x9B20,0x2022,0x0A0A,0x110A,0x1C24,0x1121,0x2323,0x2323,
        0x2323,0x2323,0x2323,0x2323,0x0A08,0x1C11,0x0819,0x0A08,
        0x43EB,0x91BA,0x5E0A,0x136A,0x5C6A,0x5C08,0x0A11,0x0C70,
        0x2500,0x0108,0x0808,0x0808,0x0808,0x0119,0xBE5A,0x4424,
        0x110A,0x0A11,0x1B08,0x2E26,0x2323,0x2323,0x2323,0x231B,
        0x2323,0x2313,0x080A,0x0A19,0x0A08,0x1B08,0x9EFD,0xD0D2,
        0x8708,0x001B,0x2E1B,0x6A08,0x0A0A,0x0A33,0x240A,0x0808,
        0x0208,0x0108,0x0800,0x080A,0x7566,0x1C42,0x1C11,0x1C13,
        0x2E41,0x3026,0x2323,0x2323,0x1B1B,0x1412,0x2323,0x231B,
        0x0A08,0x0813,0x111C,0x5C1C,0xF3F6,0xEBF9,0xE291,0x4391,
        0x2E1B,0xC943,0x0A0A,0x0A0C,0x0C28,0x150A,0x0A0A,0x0808,
        0x0801,0x0801,0x1974,0x090A,0x2D2D,0x1B1B,0x4239,0x3030,
        0x3023,0x231B,0x2312,0x1414,0x1B23,0x2323,0x0A08,0x211C,
        0x215E,0x2E39,0x9EBA,0xF6FB,0xE1F6,0xF6E1,0x8786,0xC9A8,
        0x130A,0x080A,0x151C,0x0C0A,0x0A0A,0x0801,0x0A08,0x0808,
        0x012D,0x4A00,0x0A12,0x2E39,0x3931,0x3131,0x2323,0x2323,
        0x2323,0x2323,0x2F23,0x2321,0x0A0A,0x1B43,0x5C39,0x0843,
        0xE1E2,0xEBF6,0xF6F9,0xF6EA,0xD0CF,0xC96A,0x0A11,0x0C0C,
        0x0A00,0x0808,0x0A02,0x0800,0x0000,0x0008,0x0800,0x1912,
        0x2126,0x3139,0x3130,0x3131,0x3023,0x2323,0x1B23,0x2123,
        0x2323,0x2312,0x0A08,0x0A5E,0x8713,0x12BA,0xFBF9,0xD2BA,
        0xD2FB,0xF6F5,0xDAC9,0xC986,0x080A,0x2570,0x1600,0x0913,
        0x0A0A,0x080A,0x0A0A,0x0A13,0x1B1B,0x1B21,0x2621,0x2630,
        0x3031,0x3130,0x2326,0x2F23,0x2F21,0x2321,0x2323,0x230A,
        0x0812,0x1B12,0x0A0A,0x5EFD,0xFBEB,0xB943,0xBAFB,0xEAE1,
        0xD0CA,0xB55C,0x0A11,0x1528,0x0800,0x1323,0x2121,0x2123,
        0x2E2F,0x3030,0x3030,0x3123,0x2130,0x3130,0x3031,0x2730,
        0x3026,0x3026,0x2323,0x2323,0x4023,0x231B,0x0020,0x870A,
        0x0A08,0x5CE2,0xBA5E,0x5E43,0xBAF6,0xEBE1,0xDACA,0xB55C,
        0x080A,0x0A0A,0x0C11,0x0923,0x4021,0x4023,0x2330,0x3030,
        0x3131,0x3930,0x2630,0x3931,0x3923,0x1B26,0x3023,0x2F30,
        0x2E23,0x4021,0x2123,0x231B,0x0A0A,0x3912,0x120A,0x080A,
        0x2E0A,0x1B1B,0x43EB,0xF3EA,0xD086,0xA85C,0x0808,0x001E,
        0x704C,0x1221,0x2323,0x232E,0x2F30,0x3139,0x3939,0x3A3A,
        0x3A39,0x3A31,0x3A21,0x1311,0x3030,0x3023,0x2321,0x2323,
        0x2123,0x2321,0x0A08,0x1B13,0x0808,0x0A1B,0x0A39,0x91BA,
        0xD291,0xBAEA,0xCF77,0x9086,0x0A0A,0x0A11,0x9633,0x0923,
        0x2121,0x2323,0x2F26,0x3939,0x393A,0x393A,0x313A,0x3A39,
        0x3131,0x1331,0x2123,0x262E,0x302E,0x2F23,0x2E23,0x4021,
        0x0A0A,0x080A,0x0A08,0x2187,0x21B9,0xFDFD,0xD2E1,0xDA91,
        0x8739,0x9087,0x1C11,0x0208,0x2525,0x0010,0x211D,0x1B23,
        0x212F,0x2326,0x3131,0x3139,0x3131,0x3A21,0x3A31,0x1331,
        0x211B,0x2123,0x2E2F,0x2323,0x2321,0x231B,0x0A08,0x0808,
        0x1212,0x1111,0x2E43,0x5EBA,0x39B9,0xF539,0x5A5A,0x5C42,
        0x0808,0x0808,0x0A11,0x0108,0x1212,0x2123,0x2F23,0x1B23,
        0x2631,0x3931,0x3A3A,0x3127,0x3A3A,0x1B26,0x0A08,0x0908,
        0x1B21,0x1B40,0x2123,0x2613,0x0808,0x0A08,0x131B,0x1B39,
        0x8791,0x6A39,0x081B,0xD0D0,0x5C1C,0x2E26,0x0808,0x080A,
        0x1625,0x8B32,0x000A,0x211B,0x2126,0x262F,0x2731,0x3139,
        0x313A,0x3A3A,0x4326,0x0A1B,0x1212,0x0810,0x1B21,0x2121,
        0x1021,0x160A,0x0808,0x0808,0x0A0A,0x1143,0x9EBA,0xE1EB,
        0xCA43,0x5CDA,0xA82E,0x2E1C,0x0801,0x0816,0x56A4,0xD66F,
        0x0809,0x1B1B,0x1B23,0x2726,0x2327,0x3131,0x3A3A,0x3A3A,
        0x4326,0x1131,0x1B21,0x1B12,0x1423,0x2110,0x0A09,0x080A,
        0x0208,0x0808,0x000A,0x1111,0x2E5C,0xB9EA,0xE1DA,0xA8B6,
        0xB511,0x0813,0x1101,0x080A,0x294D,0x3325,0x0800,0x0823,
        0x2326,0x2313,0x1321,0x2621,0x2126,0x3A26,0x2122,0x213A,
        0x1B21,0x1B23,0x2123,0x2321,0x0900,0x080A,0x0808,0x0808,
        0x0808,0x91E1,0xE1EA,0xE1D0,0xCFC9,0x9090,0x9021,0x1B1C,
        0x1508,0x0808,0x0A02,0x080A,0x0A08,0x0014,0x2626,0x2126,
        0x2720,0x141C,0x2120,0x2131,0x2026,0x2621,0x2F1B,0x1B21,
        0x1021,0x2113,0x0000,0x080A,0x0208,0x0208,0x0808,0x43D0,
        0xD0CA,0xB7C9,0xB6B5,0x5C2E,0x211C,0x1115,0x0A0A,0x0802,
        0x110A,0x0C0C,0x0C01,0x0021,0x2726,0x2726,0x1B13,0x090A,
        0x1B21,0x0A0A,0x2121,0x110A,0x1427,0x230A,0x121B,0x2110,
        0x0008,0x020A,0x0808,0x0A0A,0x0108,0x0821,0x2E21,0x2E42,
        0x5C42,0x211C,0x2024,0x0802,0x0A0A,0x080A,0x110C,0x0A0A,
        0x0A0A,0x0826,0x2231,0x2631,0x2031,0x110A,0x2120,0x1B20,
        0x2111,0x130A,0x211B,0x231B,0x2623,0x230A,0x0008,0x080A,
        0x0808,0x0808,0x0A0A,0x0808,0x0808,0x0A08,0x1211,0x2032,
        0x4C08,0x0808,0x0A0C,0x150C,0x0B0C,0x1515,0x080A,0x133A,
        0x263A,0x3131,0x3A3A,0x313A,0x2631,0x3121,0x2726,0x2121,
};

UWORD  mask_data[] =
{
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
                0x0000,0x0000,0x07FF,0xC000,0x0000,0x0000,0x7FFF,0xFC00,
                0x0000,0x0003,0xFFFF,0xFF80,0x0000,0x000F,0xFFFF,0xFFE0,
                0x0000,0x003F,0xFFFF,0xFFF0,0x0000,0x00FF,0xFFFF,0xFFF0,
                0x0000,0x01FF,0xFFFF,0xFFF0,0x0000,0x07FF,0xFFFF,0xFFF0,
                0x0000,0x0FFF,0xFFFF,0xFFF0,0x0000,0x1FFF,0xFFFF,0xFFF0,
                0x0000,0x3FFF,0xFFFF,0xFFF0,0x0000,0x7FFF,0xFFFF,0xFFF0,
                0x0000,0xFFFF,0xFFFF,0xFFF0,0x0001,0xFFFF,0xFFFF,0xFFF0,
                0x0003,0xFFFF,0xFFFF,0xFFF0,0x0007,0xFFFF,0xFFFF,0xFFF0,
};


Object *chunky_8_makebitmap(void)
{
        return(SC_NewObject(NULL,SCC_BITMAP_NAME,
                                           SCCA_Bitmap_RawData, chunky_8bit_data,
                                           SCCA_Bitmap_RawType, PIXFMT_CHUNKY_CLUT,
                                           SCCA_Graphic_Height, 40,
                                           SCCA_Graphic_Width, 60,
                                           SCCA_Bitmap_Palette, chunky_8bit_palette,
                                           SCCA_Bitmap_NumColors, (sizeof chunky_8bit_palette)/4,
                                           SCCA_Bitmap_PaletteFormat, PALFMT_RGB8,
                                           //SCCA_Bitmap_BltMask, mask_data,
                                           TAG_DONE));
}

/*
// empty init function, may be omitted
static ULONG Chunky8Test_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct Chunky8TestInst *inst)
{
        ULONG ret = FALSE;

        DEBUG("Got Init\n");

        if (SC_DoSuperMethodA(cl, obj, (Msg) msg))
        {
                ret = TRUE;
        }

        DEBUG1("Init return value: %ld\n",ret);

        return(ret);
}
*/

// when screen is known ...
static ULONG Chunky8Test_Setup(struct SC_Class *cl, Object *obj, Msg msg, struct Chunky8TestInst *inst)
{
        DEBUG("Got Setup\n");

        if (SC_DoSuperMethodA(cl, obj, msg))
        {

                /*
                 * We want to display some 8bit chunky raw data with the bitmap class
                 */

                if(!(inst->bmt_BitmapObj = SC_NewObject(NULL,SCC_BITMAP_NAME,
                                                                           SCCA_Bitmap_RawData, chunky_8bit_data,
                                                                           SCCA_Bitmap_RawType, PIXFMT_CHUNKY_CLUT,
                                                                           SCCA_Graphic_Height, 40,
                                                                           SCCA_Graphic_Width, 60,
                                                                           SCCA_Bitmap_Palette, chunky_8bit_palette,
                                                                           SCCA_Bitmap_NumColors, (sizeof chunky_8bit_palette)/4,
                                                                           SCCA_Bitmap_PaletteFormat, PALFMT_RGB8,
                                                                           //SCCA_Bitmap_BltMask, mask_data,
                                                                           TAG_DONE)))
                {
                        SC_DoClassMethod(cl, NULL,(ULONG) obj, SCCM_Area_Cleanup);
                
                        DEBUG("Setup return value: 0\n");
                        return FALSE;
                }
                
                // ... we can then call SCCM_Graphic_PreThink so that the time consuming stuff that has to be done
                // before we can blit with maximum speed is done
                // and then call SCCM_Graphic_Draw to actually draw the bitmap to the given position in the rastport
                
        }

        if(!(SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PreThinkScreen, scRenderInfo(obj)->screenobj)))
        {
                DEBUG("SCCM_Graphic_PreThinkScreen returned failure. Out of memory!\n");
        }

        DEBUG("Setup return value: 1\n");
        return(TRUE);
}

// when the window data changed: open, resize
static void Chunky8Test_Show(struct SC_Class *cl, Object *obj, struct SCCP_Area_Show *msg, struct Chunky8TestInst *inst)
{
        DEBUG1("Got Show with action: %ld\n",msg->action);

        SC_DoSuperMethodA(cl, obj, (Msg) msg);

        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n",(LONG) _scleft(obj),(LONG) _sctop(obj),(LONG) _scwidth(obj),(LONG) _scheight(obj));

        // set scale dimensions
        SC_SetAttrs(inst->bmt_BitmapObj, SCCA_Graphic_Width, _scwidth(obj), SCCA_Graphic_Height, _scheight(obj), TAG_DONE);
                
        if(!(SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PreThinkWindow, scRenderInfo(obj)->rport, msg->action)))
        {
                DEBUG("SCCM_Graphic_PreThinkWindow returns failure. Out of memory.\n");
        }

        DEBUG("Show finished\n");
}

static void Chunky8Test_Draw(struct SC_Class *cl, Object *obj, Msg msg, struct Chunky8TestInst *inst)
{
        DEBUG("Got Draw\n");

        SC_DoSuperMethodA(cl, obj, msg); // SCCM_Draw from AreaClass has no return value

        DEBUG4("BoxSize: %ld,%ld / %ld,%ld\n",(LONG) _scleft(obj),(LONG) _sctop(obj),(LONG) _scwidth(obj),(LONG) _scheight(obj));

        // blit the planar bitmap at position (_scleft(obj),_sctop(obj)) of the window rastport

        SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_Draw, _scleft(obj), _sctop(obj),SCCV_Graphic_DrawFlags_AbsPos);

        DEBUG("Draw finished\n");
}

static void Chunky8Test_Hide(struct SC_Class *cl, Object *obj, struct SCCP_Area_Hide *msg, struct Chunky8TestInst *inst)
{
        DEBUG1("Got Hide with action: %ld\n",msg->action);

        SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PostThinkWindow, msg->action);

        SC_DoSuperMethodA(cl, obj, (Msg) msg); // at the end

        DEBUG("Hide finished\n");
}

static void Chunky8Test_Cleanup(struct SC_Class *cl, Object *obj, Msg msg, struct Chunky8TestInst *inst)
{
        DEBUG("Got Cleanup\n");
        
        if(inst->bmt_BitmapObj)
        {
                SC_DoMethod(inst->bmt_BitmapObj, SCCM_Graphic_PostThinkScreen);
                SC_DisposeObject(inst->bmt_BitmapObj);
        }

        SC_DoSuperMethodA(cl, obj, msg); // at the end

        DEBUG("Cleanup finished\n");
}

static void Chunky8Test_AskMinMax(struct SC_Class *cl, Object *obj, struct SCCP_Area_AskMinMax *msg, struct Chunky8TestInst *inst)
{
        DEBUG("Got AskMinMax\n");

        SC_DoSuperMethodA(cl,obj, (Msg) msg);
        msg->sizes->minwidth += 60;
        msg->sizes->minheight += 40;
        msg->sizes->maxwidth = SCCV_Area_MaxSize;
        msg->sizes->maxheight = SCCV_Area_MaxSize;
        msg->sizes->defwidth += 60;
        msg->sizes->defheight += 40;
        
        DEBUG("AskMinMax finished\n");
}


/*
static void Chunky8Test_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct Chunky8TestInst *inst)
{
        DEBUG("Got Exit\n");

        SC_DoSuperMethodA(cl, obj, msg); // at the end

        DEBUG("Exit finished\n");
}
*/

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData Chunky8TestMethods[] =
{
        { SCCM_Area_AskMinMax,(ULONG)   Chunky8Test_AskMinMax, 0, 0, NULL },
        { SCCM_Area_Setup,(ULONG)       Chunky8Test_Setup, 0, 0, NULL },
        { SCCM_Area_Show,(ULONG)        Chunky8Test_Show, 0, 0, NULL },
        { SCCM_Area_Draw,(ULONG)        Chunky8Test_Draw, 0, 0, NULL },
        { SCCM_Area_Hide,(ULONG)        Chunky8Test_Hide, 0, 0, NULL },
        { SCCM_Area_Cleanup,(ULONG)     Chunky8Test_Cleanup, 0, 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

