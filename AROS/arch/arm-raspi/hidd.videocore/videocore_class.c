/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Class for VideoCore.
    Lang: English.
*/

#define DEBUG 1
#include <aros/debug.h>

#define __OOP_NOATTRBASES__

#include <proto/exec.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <aros/symbolsets.h>
#include <devices/inputevent.h>
#include <exec/alerts.h>
#include <exec/memory.h>
#include <graphics/displayinfo.h>
#include <graphics/view.h>
#include <hardware/custom.h>
#include <hidd/hidd.h>
#include <hidd/graphics.h>
#include <oop/oop.h>
#include <clib/alib_protos.h>
#include <string.h>
#include <stdio.h>

#include "videocore_class.h"
#include "videocore_hardware.h"

#include LC_LIBDEFS_FILE

static OOP_AttrBase HiddBitMapAttrBase;  
static OOP_AttrBase HiddPixFmtAttrBase;
static OOP_AttrBase HiddGfxAttrBase;
static OOP_AttrBase HiddSyncAttrBase;
static OOP_AttrBase HiddVideoCoreAttrBase;
static OOP_AttrBase HiddVideoCoreBitMapAttrBase;

static struct OOP_ABDescr attrbases[] =
{
    {IID_Hidd_BitMap,           &HiddBitMapAttrBase             },
    {IID_Hidd_VideoCoreBitMap,  &HiddVideoCoreBitMapAttrBase    },
    {IID_Hidd_VideoCore,        &HiddVideoCoreAttrBase          },
    {IID_Hidd_PixFmt,           &HiddPixFmtAttrBase             },
    {IID_Hidd_Sync,             &HiddSyncAttrBase               },
    {IID_Hidd_Gfx,              &HiddGfxAttrBase                },
    {NULL,                      NULL                            }
};

STATIC ULONG mask_to_shift(ULONG mask)
{
    ULONG i;

    for (i = 32; mask; i --) {
        mask >>= 1;
    }

    if (mask == 32) {
        i = 0;
    }

    return i;
}

#define SYNCTAGS_SIZE (11 * sizeof(struct TagItem))

APTR VideoCore_GenModeArray(struct List *modelist, struct TagItem *fmts)
{
    APTR                modearray = NULL;
    struct TagItem      *ma_syncs = NULL, *ma_synctags = NULL;
    int                 i, fmtcount = 0, modecount = 0;
    struct DisplayMode  *modecurrent;

    /* quickly count fmts and modes */
    while (fmts[fmtcount].ti_Tag == aHidd_Gfx_PixFmtTags)
        fmtcount++;

    ForeachNode(modelist, modecurrent)
    {
        modecount++;
    }

    D(bug("[VideoCore] VideoCore_GenModeArray: %d PixFmts, %d SyncModes\n", fmtcount, modecount));

    /* build our table .. */
    if (fmtcount && modecount)
    {
        if ((modearray = AllocVec((fmtcount * sizeof(struct TagItem)) + (modecount + 1 * sizeof(struct TagItem)) + (modecount * SYNCTAGS_SIZE), MEMF_PUBLIC)) != NULL)
        {
            D(bug("[VideoCore] VideoCore_GenModeArray: PixFmt's @ 0x%p\n", modearray));

            struct TagItem *ma_fmts = (struct TagItem  *)modearray;

            for (i = 0; i < fmtcount; i ++)
            {
                ma_fmts[i].ti_Tag = aHidd_Gfx_PixFmtTags;
                ma_fmts[i].ti_Data = fmts[i].ti_Data;
            }
            ma_syncs = (struct TagItem *)&ma_fmts[fmtcount];
            D(bug("[VideoCore] VideoCore_GenModeArray: ModeSync's @ 0x%p\n", ma_syncs));
            ma_synctags = (struct TagItem  *)&ma_syncs[modecount + 1];
            i = 0;
            ForeachNode(modelist, modecurrent)
            {
                D(bug("[VideoCore] VideoCore_GenModeArray: SyncTag #%d @ 0x%p\n", i, ma_synctags));

                ma_syncs[i].ti_Tag = aHidd_Gfx_SyncTags;
                ma_syncs[i].ti_Data = ma_synctags;

                ma_synctags[0].ti_Tag = aHidd_Sync_PixelClock;
                ma_synctags[0].ti_Data = modecurrent->dm_clock * 1000;
                ma_synctags[1].ti_Tag = aHidd_Sync_HDisp;
                ma_synctags[1].ti_Data = modecurrent->dm_hdisp;
                ma_synctags[2].ti_Tag = aHidd_Sync_HSyncStart;
                ma_synctags[2].ti_Data = modecurrent->dm_hstart;
                ma_synctags[3].ti_Tag = aHidd_Sync_HSyncEnd;
                ma_synctags[3].ti_Data = modecurrent->dm_hend;
                ma_synctags[4].ti_Tag = aHidd_Sync_HTotal;
                ma_synctags[4].ti_Data = modecurrent->dm_htotal;
                ma_synctags[5].ti_Tag = aHidd_Sync_VDisp;
                ma_synctags[5].ti_Data = modecurrent->dm_vdisp;
                ma_synctags[6].ti_Tag = aHidd_Sync_VSyncStart;
                ma_synctags[6].ti_Data = modecurrent->dm_vstart;
                ma_synctags[7].ti_Tag = aHidd_Sync_VSyncEnd;
                ma_synctags[7].ti_Data = modecurrent->dm_vend;
                ma_synctags[8].ti_Tag = aHidd_Sync_VTotal;
                ma_synctags[8].ti_Data = modecurrent->dm_vtotal;
                ma_synctags[9].ti_Tag = aHidd_Sync_Description;
                ma_synctags[9].ti_Data = modecurrent->dm_descr;
                ma_synctags[10].ti_Tag = TAG_DONE;

                ma_synctags = (struct TagItem  *)&ma_synctags[11];
                i++;
            }
            ma_syncs[i].ti_Tag = TAG_DONE;
        }
    }

    return (APTR)modearray;
}

OOP_Object *VideoCore__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    struct VideoCore_staticdata *xsd = XSD(cl);
    struct List                 vc_modelist;
    APTR                        vc_modearray;
    OOP_Object                  *self = NULL;

    struct TagItem pftags_24bpp[] = {
        { aHidd_PixFmt_RedShift,      8   },
        { aHidd_PixFmt_GreenShift,    16  },
        { aHidd_PixFmt_BlueShift,     24  },
        { aHidd_PixFmt_AlphaShift,    0   },
        { aHidd_PixFmt_RedMask,       0x00ff0000 },
        { aHidd_PixFmt_GreenMask,     0x0000ff00 },
        { aHidd_PixFmt_BlueMask,      0x000000ff },
        { aHidd_PixFmt_AlphaMask,     0x00000000 },
        { aHidd_PixFmt_ColorModel,    vHidd_ColorModel_TrueColor },
        { aHidd_PixFmt_Depth,         24  },
        { aHidd_PixFmt_BytesPerPixel, 4   },
        { aHidd_PixFmt_BitsPerPixel,  24  },
        { aHidd_PixFmt_StdPixFmt,     vHidd_StdPixFmt_Native },
        { aHidd_PixFmt_BitMapType,    vHidd_BitMapType_Chunky },
        { TAG_DONE, 0UL }
    };

    struct TagItem pftags_16bpp[] = {
        { aHidd_PixFmt_RedShift,      16  },
        { aHidd_PixFmt_GreenShift,    21  },
        { aHidd_PixFmt_BlueShift,     27  },
        { aHidd_PixFmt_AlphaShift,    0   },
        { aHidd_PixFmt_RedMask,       0x0000f800 },
        { aHidd_PixFmt_GreenMask,     0x000007e0 },
        { aHidd_PixFmt_BlueMask,      0x0000001f },
        { aHidd_PixFmt_AlphaMask,     0x00000000 },
        { aHidd_PixFmt_ColorModel,    vHidd_ColorModel_TrueColor },
        { aHidd_PixFmt_Depth,         16  },
        { aHidd_PixFmt_BytesPerPixel, 2   },
        { aHidd_PixFmt_BitsPerPixel,  16  },
        { aHidd_PixFmt_StdPixFmt,     vHidd_StdPixFmt_Native },
        { aHidd_PixFmt_BitMapType,    vHidd_BitMapType_Chunky },
        { TAG_DONE, 0UL }
    };

    struct TagItem pftags_15bpp[] = {
        { aHidd_PixFmt_RedShift,      17  },
        { aHidd_PixFmt_GreenShift,    22  },
        { aHidd_PixFmt_BlueShift,     27  },
        { aHidd_PixFmt_AlphaShift,    0   },
        { aHidd_PixFmt_RedMask,       0x00007c00 },
        { aHidd_PixFmt_GreenMask,     0x000003e0 },
        { aHidd_PixFmt_BlueMask,      0x0000001f },
        { aHidd_PixFmt_AlphaMask,     0x00000000 },
        { aHidd_PixFmt_ColorModel,    vHidd_ColorModel_TrueColor },
        { aHidd_PixFmt_Depth,         15  },
        { aHidd_PixFmt_BytesPerPixel, 2   },
        { aHidd_PixFmt_BitsPerPixel,  15  },
        { aHidd_PixFmt_StdPixFmt,     vHidd_StdPixFmt_Native },
        { aHidd_PixFmt_BitMapType,    vHidd_BitMapType_Chunky },
        { TAG_DONE, 0UL }
    };

    struct TagItem pftags_8bpp[] = {
        { aHidd_PixFmt_RedShift,      8  },
        { aHidd_PixFmt_GreenShift,    16  },
        { aHidd_PixFmt_BlueShift,     24  },
        { aHidd_PixFmt_AlphaShift,    0   },
        { aHidd_PixFmt_RedMask,       0x00FF0000 },
        { aHidd_PixFmt_GreenMask,     0x0000FF00 },
        { aHidd_PixFmt_BlueMask,      0x000000FF },
        { aHidd_PixFmt_AlphaMask,     0x00000000 },
        { aHidd_PixFmt_CLUTMask,      0x000000FF },
	{ aHidd_PixFmt_CLUTShift,     0 },
	{ aHidd_PixFmt_ColorModel,    vHidd_ColorModel_Palette },
        { aHidd_PixFmt_Depth,         8  },
        { aHidd_PixFmt_BytesPerPixel, 1   },
        { aHidd_PixFmt_BitsPerPixel,  8  },
        { aHidd_PixFmt_StdPixFmt,     vHidd_StdPixFmt_Native },
        { aHidd_PixFmt_BitMapType,    vHidd_BitMapType_Chunky },
        { TAG_DONE, 0UL }
    };

    struct TagItem fmttags[] = {
        { aHidd_Gfx_PixFmtTags, (IPTR)pftags_24bpp  },
        { aHidd_Gfx_PixFmtTags, (IPTR)pftags_16bpp  },
        { aHidd_Gfx_PixFmtTags, (IPTR)pftags_15bpp  },
        { aHidd_Gfx_PixFmtTags, (IPTR)pftags_8bpp   },
        { TAG_DONE, 0UL }
    };

    EnterFunc(bug("VideoCore::New()\n"));

    NEWLIST(&vc_modelist);
    
    VideoCore_SDTV_SyncGen(&vc_modelist);
    VideoCore_HDMI_SyncGen(&vc_modelist);

    if ((vc_modearray = VideoCore_GenModeArray(&vc_modelist, fmttags)) != NULL)
    {
        struct TagItem vcmodes[] =
        {
            {aHidd_Gfx_ModeTags,    vc_modearray    },
            {TAG_MORE,              msg->attrList   }
        };
        struct pRoot_New vcmsgnew;

        D(bug("[VideoCore] Generated Mode Array @ 0x%p\n", vc_modearray));

        vcmsgnew.mID = msg->mID;
        vcmsgnew.attrList = vcmodes;
        msg = &vcmsgnew;

        if ((self = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg)) != NULL)
        {
            D(bug("[VideoCore] Storing reference to self in staticdata\n"));
            XSD(cl)->videocorehidd = self;
        }
    }

    ReturnPtr("VideoCore::New", OOP_Object *, self);
}

VOID VideoCore__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    D(bug("[VideoCore] VideoCore__Root__Dispose()\n"));

    if (XSD(cl)->mouse.shape != NULL)
        FreeVec(XSD(cl)->mouse.shape);
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

VOID VideoCore__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    ULONG idx;
    BOOL found = FALSE;

    D(bug("[VideoCore] VideoCore__Root__Get()\n"));

    if (IS_GFX_ATTR(msg->attrID, idx))
    {
//        switch (idx)
//        {
//        case aoHidd_Gfx_SupportsHWCursor:
//            *msg->storage = (IPTR)TRUE;
//            found = TRUE;
//            break;
//        }
    }
    if (!found)
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

OOP_Object *VideoCore__Hidd_Gfx__NewBitMap(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_NewBitMap *msg)
{
    BOOL displayable;
    BOOL framebuffer;
    OOP_Class *classptr = NULL;
    struct TagItem tags[2];
    struct pHidd_Gfx_NewBitMap vcmsgnew;

    EnterFunc(bug("VideoCore::NewBitMap()\n"));
    displayable = GetTagData(aHidd_BitMap_Displayable, FALSE, msg->attrList);
    framebuffer = GetTagData(aHidd_BitMap_FrameBuffer, FALSE, msg->attrList);
    if (framebuffer)
        classptr = XSD(cl)->videocoreonbmclass;
    else if (displayable)
        classptr = XSD(cl)->videocoreoffbmclass;
    else
    {
        HIDDT_ModeID modeid;
        modeid = (HIDDT_ModeID)GetTagData(aHidd_BitMap_ModeID, vHidd_ModeID_Invalid, msg->attrList);
        if (modeid != vHidd_ModeID_Invalid)
            classptr = XSD(cl)->videocoreoffbmclass;
        else
        {
            HIDDT_StdPixFmt stdpf;
            stdpf = (HIDDT_StdPixFmt)GetTagData(aHidd_BitMap_StdPixFmt, vHidd_StdPixFmt_Unknown, msg->attrList);
            if (stdpf == vHidd_StdPixFmt_Unknown)
            {
                OOP_Object *friend;
                friend = (OOP_Object *)GetTagData(aHidd_BitMap_Friend, (IPTR)NULL, msg->attrList);
                if (friend != NULL)
                {
                    OOP_Class *friend_class = NULL;
                    OOP_GetAttr(friend, aHidd_BitMap_ClassPtr, (IPTR *)&friend_class);
                    if (friend_class == XSD(cl)->videocoreonbmclass)
                    {
                        classptr = XSD(cl)->videocoreoffbmclass;
                    }
                }
            }
        }
    }
    if (classptr != NULL)
    {
        tags[0].ti_Tag = aHidd_BitMap_ClassPtr;
        tags[0].ti_Data = (IPTR)classptr;
        tags[1].ti_Tag = TAG_MORE;
        tags[1].ti_Data = (IPTR)msg->attrList;
        vcmsgnew.mID = msg->mID;
        vcmsgnew.attrList = tags;
        msg = &vcmsgnew;
    }
    ReturnPtr("VideoCore::NewBitMap", OOP_Object *, (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg));
}

VOID VideoCore__Hidd_Gfx__CopyBox(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CopyBox *msg)
{
    UBYTE *src = NULL;
    UBYTE *dst = NULL;
    HIDDT_DrawMode mode;
    struct Box box;

    EnterFunc(bug("VideoCore.BitMap::CopyBox\n"));
    mode = GC_DRMD(msg->gc);
    OOP_GetAttr(msg->src, aHidd_VideoCoreBitMap_Drawable, (IPTR *)&src);
    OOP_GetAttr(msg->dest, aHidd_VideoCoreBitMap_Drawable, (IPTR *)&dst);
    if (((dst == NULL) || (src == NULL))) /* no videocoregfx bitmap */
    {
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    }
    else if (dst == src)
    {
        struct BitmapData *data;
        data = OOP_INST_DATA(OOP_OCLASS(msg->src), msg->src);
        switch (mode)
        {
/*        case vHidd_GC_DrawMode_Clear:
            clearCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_And:
            andCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_AndReverse:
            andReverseCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Copy:
            copyCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_AndInverted:
            andInvertedCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_NoOp:
            noOpCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Xor:
            xorCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Or:
            orCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Nor:
            norCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Equiv:
            equivCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Invert:
            invertCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_OrReverse:
            orReverseCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_CopyInverted:
            copyInvertedCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_OrInverted:
            orInvertedCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Nand:
            nandCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
        case vHidd_GC_DrawMode_Set:
            setCopyVideoCore(data->data, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height);
            break;
*/
        default:
            OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
        }
    }
    else
    {
        struct BitmapData *srcbd = OOP_INST_DATA(OOP_OCLASS(msg->src), msg->src);
        struct BitmapData *dstbd = OOP_INST_DATA(OOP_OCLASS(msg->dest), msg->dest);
        UBYTE *sbuffer;
        ULONG srestadd;
        UBYTE *dbuffer;
        ULONG drestadd;
        ULONG ycnt = msg->height;
        ULONG xcnt;
        LONG offset;
        /* get src/dest video data start addresses and skip sizes */
        if (srcbd->VideoData == srcbd->data->vrambase)
        {
            offset = (msg->srcX*srcbd->bytesperpix)+(msg->srcY*srcbd->data->bytesperline);
            srestadd = (srcbd->data->bytesperline - (msg->width*srcbd->bytesperpix));
//            displayCursorVideoCore(&XSD(cl)->data, 0);
            XSD(cl)->mouse.visible = 0;
        }
        else
        {
            offset = (msg->srcX+(msg->srcY*srcbd->width))*srcbd->bytesperpix;
            srestadd = (srcbd->width - msg->width)*srcbd->bytesperpix;
        }
        sbuffer = srcbd->VideoData+offset;
        if (dstbd->VideoData == dstbd->data->vrambase)
        {
            offset = (msg->destX*dstbd->bytesperpix)+(msg->destY*dstbd->data->bytesperline);
            drestadd = (dstbd->data->bytesperline - (msg->width*dstbd->bytesperpix));
//            displayCursorVideoCore(&XSD(cl)->data, 0);
            XSD(cl)->mouse.visible = 0;
        }
        else
        {
            offset = (msg->destX+(msg->destY*dstbd->width))*dstbd->bytesperpix;
            drestadd = (dstbd->width - msg->width)*dstbd->bytesperpix;
        }
        dbuffer = dstbd->VideoData+offset;
        switch (mode)
        {
        case vHidd_GC_DrawMode_Copy:
            while (ycnt--)
            {
                ULONG pixel;
                xcnt = msg->width;
                while (xcnt)
                {
                    /* get pixel from source */
                        switch (srcbd->bytesperpix)
                    {
                        case 1:
                        pixel = (ULONG)*((UBYTE *)sbuffer);
                        sbuffer++;
                        break;
                        case 2:
                        pixel = (ULONG)*((UWORD *)sbuffer);
                        sbuffer += 2;
                        break;
                        case 4:
                        pixel = (ULONG)*((ULONG *)sbuffer);
                        sbuffer += 4;
                        break;
                        default:
                        D(bug("[VideoCore] Copy: Unknown number of bytes per pixel (%d) in source!\n",srcbd->bytesperpix));
                        pixel = 0;
                        break;
                    }
                    /* write pixel to destination */
                    switch (dstbd->bytesperpix)
                    {
                        case 1:
                        *((UBYTE *)dbuffer) = (UBYTE)pixel;
                        dbuffer++;
                        break;
                        case 2:
                        *((UWORD *)dbuffer) = (UWORD)pixel;
                        dbuffer += 2;
                        break;
                        case 4:	
                        *((ULONG *)dbuffer) = (ULONG)pixel;
                        dbuffer += 4;
                        break;
                        default:
                        D(bug("[VideoCore] Copy: Unknown number of bytes per pixel (%d) in destination!\n",dstbd->bytesperpix));
                        break;
                    }
                    xcnt--;
                }
                sbuffer += srestadd;
                dbuffer += drestadd;
            }
            if (dstbd->VideoData == dstbd->data->vrambase)
            {
                box.x1 = msg->destX;
                box.y1 = msg->destY;
                box.x2 = box.x1+msg->width-1;
                box.y2 = box.y1+msg->height-1;
//                refreshAreaVideoCore(dstbd->data, &box);
            }
            break;
        default:
            kprintf("[VideoCore] mode = %ld src=%lx dst=%lx\n", mode, src, dst);
            OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
        }
        if (XSD(cl)->mouse.visible == 0)
        {
//            displayCursorVideoCore(&XSD(cl)->data, 1);
            XSD(cl)->mouse.visible = 1;
        }
    }
    ReturnVoid("VideoCore.BitMap::CopyBox");
}

BOOL VideoCore__Hidd_Gfx__SetCursorShape(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorShape *msg)
{
    struct VideoCore_staticdata *xsd = XSD(cl);

    if (msg->shape == NULL)
    {
//        displayCursorVideoCore(&XSD(cl)->data, 0);
        xsd->mouse.oopshape = NULL;
        if (xsd->mouse.shape != NULL)
            FreeVec(xsd->mouse.shape);
        xsd->mouse.shape = NULL;
        return TRUE;
    }
    else
    {
        OOP_Object *pfmt;
        OOP_Object *colmap;
        HIDDT_StdPixFmt pixfmt;
        HIDDT_Color color;
        IPTR tmp;
        OOP_GetAttr(msg->shape, aHidd_BitMap_Width, &tmp);
        xsd->mouse.width = tmp;
        OOP_GetAttr(msg->shape, aHidd_BitMap_Height, &tmp);
        xsd->mouse.height = tmp;
        OOP_GetAttr(msg->shape, aHidd_BitMap_PixFmt, (IPTR *)&pfmt);
        OOP_GetAttr(pfmt, aHidd_PixFmt_StdPixFmt, (IPTR *)&pixfmt);
        OOP_GetAttr(msg->shape, aHidd_BitMap_ColorMap, (IPTR *)&colmap);
        xsd->mouse.oopshape = msg->shape;
#if 0
        xsd->mouse.shape = cursor_shape;
        xsd->mouse.width = 11;
        xsd->mouse.height = 11;
//        defineCursorVideoCore(&XSD(cl)->data, &xsd->mouse);
        return TRUE;
#else
        /* convert shape to videocore needs */
        if (xsd->mouse.shape != NULL)
            FreeVec(xsd->mouse.shape);
//        xsd->mouse.shape = AllocVec(SVGA_PIXMAP_SIZE(xsd->mouse.width, xsd->mouse.height, VCDATA(xsd)->bitsperpixel)*4, MEMF_PUBLIC);
        if (xsd->mouse.shape != NULL)
        {
            LONG xcnt;
            LONG ycnt;
            LONG linebytes;
            LONG bytecnt;
            LONG pixelbytes;
            UBYTE *shape;
//            linebytes = SVGA_PIXMAP_SCANLINE_SIZE(xsd->mouse.width, VCDATA(xsd)->bitsperpixel)*4;
            pixelbytes = (VCDATA(xsd)->bitsperpixel+3)/8;
            shape = xsd->mouse.shape;
            for (ycnt=0;ycnt<xsd->mouse.height;ycnt++)
            {
                for (xcnt=0;xcnt<xsd->mouse.width;xcnt++)
                {
                    HIDDT_Pixel pixel;
                    pixel = HIDD_BM_GetPixel(msg->shape, xcnt, ycnt);
                    if (pixfmt == vHidd_StdPixFmt_LUT8)
                    {
                        if (HIDD_CM_GetColor(colmap, pixel, &color))
                        {
                            pixel =
                                (
                                    (((color.red  <<16)>>mask_to_shift(VCDATA(xsd)->redmask))   & VCDATA(xsd)->redmask  )+
                                    (((color.green<<16)>>mask_to_shift(VCDATA(xsd)->greenmask)) & VCDATA(xsd)->greenmask)+
                                    (((color.blue <<16)>>mask_to_shift(VCDATA(xsd)->bluemask))  & VCDATA(xsd)->bluemask )
                                );
                        }
                    }
                    if (pixelbytes == 1)
                    {
                        *((UBYTE *)shape) = (UBYTE)pixel;
                        shape++;
                    }
                    else if (pixelbytes == 2)
                    {
                        *((UWORD *)shape) = (UWORD)pixel;
                        shape += 2;
                    }
                    else if (pixelbytes == 4)
                    {
                        *((ULONG *)shape) = (ULONG)pixel;
                        shape += 4;
                    }
                }
                for (bytecnt=linebytes-(xsd->mouse.width*pixelbytes);bytecnt;bytecnt--)
                {
                    *((UBYTE *)shape) = 0; /* fill up to long boundary */
                    shape++;
                }
            }
//            defineCursorVideoCore(&XSD(cl)->data, &xsd->mouse);
            return TRUE;
        }
#endif
    }
    return FALSE;
}

BOOL VideoCore__Hidd_Gfx__SetCursorPos(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorPos *msg)
{
    XSD(cl)->mouse.x = msg->x;
    XSD(cl)->mouse.y = msg->y;
    if (XSD(cl)->mouse.x<0)
        XSD(cl)->mouse.x=0;
    if (XSD(cl)->mouse.y<0)
        XSD(cl)->mouse.y=0;
    /* TODO: check visible width/height */
//    moveCursorVideoCore(&XSD(cl)->data, msg->x, msg->y);
    return TRUE;
}

VOID VideoCore__Hidd_Gfx__SetCursorVisible(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorVisible *msg)
{
    XSD(cl)->mouse.visible = msg->visible;
//    displayCursorVideoCore(&XSD(cl)->data, msg->visible ? 1 : 0);
}


static int VideoCore_InitStatic(LIBBASETYPEPTR LIBBASE)
{
    EnterFunc(bug("[VideoCore] VideoCore_InitStatic()\n"));

    LIBBASE->vsd.mouse.x=0;
    LIBBASE->vsd.mouse.y=0;
    LIBBASE->vsd.mouse.shape = NULL;

    if (!OOP_ObtainAttrBases(attrbases))
    {
        D(bug("[VideoCore] VideoCore_InitStatic: attrbases init failed\n"));
        return FALSE;
    }
    
    D(bug("[VideoCore] VideoCore_InitStatic: ok\n"));

    ReturnInt("VideoCore_InitStatic", int, TRUE);
}

static int VideoCore_ExpungeStatic(LIBBASETYPEPTR LIBBASE)
{
    EnterFunc(bug("[VideoCore] VideoCore_ExpungeStatic()\n"));

    OOP_ReleaseAttrBases(attrbases);
    ReturnInt("VideoCore_ExpungeStatic", int, TRUE);
}

ADD2INITLIB(VideoCore_InitStatic, 0)
ADD2EXPUNGELIB(VideoCore_ExpungeStatic, 0)