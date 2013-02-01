/*
    Copyright � 2013, The AROS Development Team. All rights reserved.
    $Id$

    Desc: VideoCore Hidd initialisation code
    Lang: english
*/

#define DEBUG 1
#include <aros/debug.h>

#define __OOP_NOATTRBASES__

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/oop.h>
#include <proto/vcmbox.h>
#include <proto/kernel.h>
#include <proto/utility.h>

#include <exec/types.h>
#include <exec/lists.h>
#include <graphics/driver.h>
#include <graphics/gfxbase.h>
#include <hidd/graphics.h>
#include <oop/oop.h>
#include <utility/utility.h>
#include <aros/symbolsets.h>

#include "videocore_class.h"
#include "videocore_hardware.h"

#include LC_LIBDEFS_FILE

#define HALTCODE

static OOP_AttrBase HiddPixFmtAttrBase;	// = 0;
static OOP_AttrBase HiddPCIDeviceAttrBase;

static struct OOP_ABDescr abd[] =
{
    { IID_Hidd_PixFmt,  &HiddPixFmtAttrBase },
    { NULL,             NULL                }
};
static int VideoCore_Init(LIBBASETYPEPTR LIBBASE)
{
    struct VideoCore_staticdata *xsd = &LIBBASE->vsd;
    int retval = FALSE;
    
    if (!OOP_ObtainAttrBases(abd))
        goto failure;

    if (!(VCMBoxBase = OpenResource("vcmbox.resource")))
        goto failure;

    if (!(VCMsg = AllocVec(sizeof(IPTR) * 2 * MAX_TAGS, MEMF_CLEAR)))
        goto failure;

    D(bug("[VideoCore] Init: VideoCore Mailbox resource @ 0x%p\n", VCMBoxBase));
    D(bug("[VideoCore] Init: VideoCore message buffer @ 0x%p\n", VCMsg));

    
    VCMsg[0] = 8 * 4;
    VCMsg[1] = VCTAG_REQ;
    VCMsg[2] = VCTAG_GETVCRAM;
    VCMsg[3] = 8;
    VCMsg[4] = 0;

    VCMsg[5] = 0;
    VCMsg[6] = 0;

    VCMsg[7] = 0; // terminate tag

    VCMBoxWrite(VCMB_BASE, VCMB_FBCHAN, VCMsg);
    if (VCMBoxRead(VCMB_BASE, VCMB_FBCHAN) == VCMsg)
    {
        if (videocore_InitMem(VCMsg[5], VCMsg[6], LIBBASE))
        {
            D(bug("[VideoCore] Init: VideoCore GPU Found\n"));

            initVideoCoreGfxHW((APTR)xsd);

            if ((GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 41)) != NULL)
            {
                if (AddDisplayDriver(LIBBASE->vsd.vcsd_VideoCoreGfxClass, NULL, DDRV_BootMode, TRUE, TAG_DONE) == DD_OK)
                {
                    D(bug("[VideoCore] Init: Display Driver registered\n"));

                    /* We use ourselves, and no one else does */
                    LIBBASE->library.lib_OpenCnt = 1;
                    retval = TRUE;
                }
                CloseLibrary(&GfxBase->LibNode);
            }
        }
    }

failure:
    if (!(retval))
    {
        D(bug("[VideoCore] Init: No VideoCore GPU Found\n"));

        FreeVec(VCMsg);

        OOP_ReleaseAttrBases(abd);
    }
    return retval;
}

ADD2INITLIB(VideoCore_Init, 0)