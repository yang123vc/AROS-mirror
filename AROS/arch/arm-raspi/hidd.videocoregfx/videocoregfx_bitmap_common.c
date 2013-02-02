/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 0
#include <aros/debug.h>

#include <exec/alerts.h>
#include <string.h>    // memset() prototype

#include "videocoregfx_hardware.h"

#ifdef OnBitmap
/*********  BitMap::Clear()  *************************************/
VOID MNAME_BM(Clear)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_Clear *msg) 
{
    struct BitmapData *data = OOP_INST_DATA(cl, o);
    IPTR    	    	width, height;

    D(bug("[VideoCoreGfx] VideoCoreGfx.BitMap::Clear()\n"));

    /* Get width & height from bitmap */

    OOP_GetAttr(o, aHidd_BitMap_Width,  &width);
    OOP_GetAttr(o, aHidd_BitMap_Height, &height);

#warning "TODO: Implement HW accelerated bitmap clear"
}
#endif

BOOL MNAME_BM(SetColors)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_SetColors *msg) 
{
    struct BitmapData *data = OOP_INST_DATA(cl, o);
    HIDDT_PixelFormat *pf;
    HIDDT_Pixel red;
    HIDDT_Pixel green;
    HIDDT_Pixel blue;
    ULONG xc_i;
    ULONG col_i;

    pf = BM_PIXFMT(o);
    if (
            (vHidd_ColorModel_StaticPalette == HIDD_PF_COLMODEL(pf)) ||
            (vHidd_ColorModel_TrueColor == HIDD_PF_COLMODEL(pf))
        )
        return OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    if (!OOP_DoSuperMethod(cl, o, (OOP_Msg)msg))
        return FALSE;
    if ((msg->firstColor + msg->numColors) > (1<<data->bpp))
        return FALSE;
    for (xc_i = msg->firstColor, col_i = 0; col_i < msg->numColors; xc_i++, col_i++)
    {
        red = msg->colors[col_i].red >> 8;
        green = msg->colors[col_i].green >> 8;
        blue = msg->colors[col_i].blue >> 8;
        data->cmap[xc_i] = 0x01000000 | red | (green << 8) | (blue << 16);
#ifdef OnBitmap
#warning "TODO: Load the LUT colors"
#endif
        msg->colors[col_i].pixval = xc_i;
    }
    return TRUE;
}

/*** BitMap::Get() *******************************************/

VOID MNAME_ROOT(Get)(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    struct BitmapData *data = OOP_INST_DATA(cl, o);
    ULONG idx;

    if (IS_VideoCoreGfxBM_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
        case aoHidd_VideoCoreGfxBitMap_Drawable:
            *msg->storage = (IPTR)data->VideoData;
            break;
        default:
            OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
        }
    }
    else
    {
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    }
}
