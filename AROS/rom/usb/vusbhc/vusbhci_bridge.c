/*
    Copyright © 2015-2016, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Virtual USB host controller
    Lang: English
*/

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1

#include <aros/debug.h>

#include <proto/exec.h>
#include <proto/hostlib.h>

#include <devices/usb.h>
#include <devices/usb_hub.h>

#include "vusbhci_device.h"
#include "vusbhci_bridge.h"

APTR HostLibBase;
struct libusb_func libusb_func;

static void *libusbhandle;

static libusb_device_handle *dev_handle = NULL;

void ctrl_callback_handler(struct libusb_transfer *transfer) {

    struct IOUsbHWReq *ioreq = transfer->user_data;
    struct VUSBHCIUnit *unit = (struct VUSBHCIUnit *) ioreq->iouh_Req.io_Unit;

    mybug_unit(-1, ("ctrl_callback_handler!\n\n"));
}

int hotplug_callback_event_handler(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {

    struct VUSBHCIBase *VUSBHCIBase = (struct VUSBHCIBase *)user_data;
    struct VUSBHCIUnit *unit = VUSBHCIBase->usbunit200;

    struct libusb_device_descriptor desc;
    int rc, speed;

    mybug_unit(-1, ("Hotplug callback event!\n"));

    switch(event) {

        case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
            mybug_unit(-1, ("- Device attached\n"));

            if(unit->allocated && (!dev_handle)) {
                rc = LIBUSBCALL(libusb_get_device_descriptor, dev, &desc);
                if (LIBUSB_SUCCESS != rc) {
                    mybug_unit(-1, ("Failed to read device descriptor\n"));
                    return 0;
                } else {
                    mybug_unit(-1, ("Device attach: %04x:%04x\n", desc.idVendor, desc.idProduct));

                    rc = LIBUSBCALL(libusb_open, dev, &dev_handle);
                    if(dev_handle) {

                        LIBUSBCALL(libusb_set_auto_detach_kernel_driver, dev_handle, 0);

                        if ( LIBUSBCALL(libusb_kernel_driver_active, dev_handle, 0) ) {
                            mybug_unit(-1, ("Kernel driver active\n"));
                            if ( (LIBUSBCALL(libusb_detach_kernel_driver, dev_handle, 0) == 0) ) {
                                mybug_unit(-1, ("Kernel driver detached\n"));
                            } else {
                                mybug_unit(-1, ("Kernel driver NOT detached\n"));
                            }
                        } else {
                            mybug_unit(-1, ("Kernel driver doesn't appear to be active\n"));
                        }

                        /* Check again for debug purposes */
                        if ( LIBUSBCALL(libusb_kernel_driver_active, dev_handle, 0) ) {
                            mybug_unit(-1, ("Kernel driver active\n"));
                            if ( (LIBUSBCALL(libusb_detach_kernel_driver, dev_handle, 0) == 0) ) {
                                mybug_unit(-1, ("Kernel driver detached\n"));
                            } else {
                                mybug_unit(-1, ("Kernel driver NOT detached\n"));
                            }
                        } else {
                            mybug_unit(-1, ("Kernel driver doesn't appear to be active\n"));
                        }

                        LIBUSBCALL(libusb_set_configuration, dev_handle, 0);
                        LIBUSBCALL(libusb_claim_interface, dev_handle, 0);

                        speed = LIBUSBCALL(libusb_get_device_speed, dev);
                        switch(speed) {
                            case LIBUSB_SPEED_LOW:
                                unit->roothub.portstatus.wPortStatus |= AROS_WORD2LE(UPSF_PORT_LOW_SPEED);
                            break;
                            case LIBUSB_SPEED_FULL:
                                unit->roothub.portstatus.wPortStatus &= ~(AROS_WORD2LE(UPSF_PORT_HIGH_SPEED)|AROS_WORD2LE(UPSF_PORT_LOW_SPEED));
                            break;
                            case LIBUSB_SPEED_HIGH:
                                unit->roothub.portstatus.wPortStatus |= AROS_WORD2LE(UPSF_PORT_HIGH_SPEED);
                            break;
                            //case LIBUSB_SPEED_SUPER:
                            //break;
                        }

                        unit->roothub.portstatus.wPortStatus |= AROS_WORD2LE(UPSF_PORT_CONNECTION);
                        unit->roothub.portstatus.wPortChange |= AROS_WORD2LE(UPSF_PORT_CONNECTION);

                        uhwCheckRootHubChanges(unit);
                    } else {
                        if(rc == LIBUSB_ERROR_ACCESS) {
                            mybug_unit(-1, ("libusb_open, access error, try running as superuser\n\n"));
                        }
                    }
                }
            }
        break;

        case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
            mybug_unit(-1, (" - Device detached\n"));

            if(unit->allocated) {

                unit->roothub.portstatus.wPortStatus &= ~UPSF_PORT_CONNECTION;
                unit->roothub.portstatus.wPortChange |= UPSF_PORT_CONNECTION;

                uhwCheckRootHubChanges(unit);
            }

            if(dev_handle != NULL) {
                LIBUSBCALL(libusb_close, dev_handle);
                dev_handle = NULL;
            }

        break;

        default:
            mybug_unit(-1, (" - Unknown event arrived\n"));
        break;

    }

  return 0;
}

void *hostlib_load_so(const char *sofile, const char **names, int nfuncs, void **funcptr) {
    void *handle;
    char *err;
    int i;

    if ((handle = HostLib_Open(sofile, &err)) == NULL) {
        (bug("[LIBUSB] failed to open '%s': %s\n", sofile, err));
        return NULL;
    }else{
        bug("[LIBUSB] opened '%s'\n", sofile);
    }

    for (i = 0; i < nfuncs; i++) {
        funcptr[i] = HostLib_GetPointer(handle, names[i], &err);
        if (err != NULL) {
            bug("[LIBUSB] failed to get symbol '%s' (%s)\n", names[i], err);
            HostLib_Close(handle, NULL);
            return NULL;
        }else{
            bug("[LIBUSB] managed to get symbol '%s'\n", names[i]);
        }
    }

    return handle;
}

BOOL libusb_bridge_init(struct VUSBHCIBase *VUSBHCIBase) {

    int rc;

    HostLibBase = OpenResource("hostlib.resource");

    if (!HostLibBase)
        return FALSE;

    libusbhandle = hostlib_load_so("libusb.so", libusb_func_names, LIBUSB_NUM_FUNCS, (void **)&libusb_func);

    if (!libusbhandle)
        return FALSE;

    if(!LIBUSBCALL(libusb_init, NULL)) {
        LIBUSBCALL(libusb_set_debug, NULL, 1);
        bug("[LIBUSB] Checking hotplug support of libusb\n");
        if (LIBUSBCALL(libusb_has_capability, LIBUSB_CAP_HAS_HOTPLUG)) {
            bug("[LIBUSB]  - Hotplug supported\n");

            rc = (LIBUSBCALL(libusb_hotplug_register_callback,
                            NULL,
                            (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
                            0,
                            LIBUSB_HOTPLUG_MATCH_ANY,
                            LIBUSB_HOTPLUG_MATCH_ANY,
                            LIBUSB_HOTPLUG_MATCH_ANY,
                            hotplug_callback_event_handler,
                            (void *)VUSBHCIBase,
                            NULL)
            );

            if(rc == LIBUSB_SUCCESS) {
                bug("[LIBUSB]  - Hotplug callback installed rc = %d\n", rc);
                return TRUE;
            }

            bug("[LIBUSB]  - Hotplug callback installation failure! rc = %d\n", rc);

        } else {
            bug("[LIBUSB]  - Hotplug not supported, failing...\n");
            LIBUSBCALL(libusb_exit, NULL);
        }
        libusb_bridge_cleanup();
    }

    return FALSE;
}

VOID libusb_bridge_cleanup() {
    HostLib_Close(libusbhandle, NULL);
}

void call_libusb_event_handler() {
    LIBUSBCALL(libusb_handle_events, NULL);
}

/*
    FIXME: libusb expects buffer to precede with enough space for setup data (8 bytes or LIBUSB_CONTROL_SETUP_SIZE)
            - Copy buffer need to be used

    IGNORE or CHECKME: LIBUSB_xxx_SETUP_SIZE is needed for asynchronous use of libusb

*/
int do_libusb_ctrl_transfer(struct IOUsbHWReq *ioreq) {
    struct VUSBHCIUnit *unit = (struct VUSBHCIUnit *) ioreq->iouh_Req.io_Unit;

    int rc;

    UWORD bmRequestType      = (ioreq->iouh_SetupData.bmRequestType);
    UWORD bRequest           = (ioreq->iouh_SetupData.bRequest);
    UWORD wValue             = (ioreq->iouh_SetupData.wValue);
    UWORD wIndex             = (ioreq->iouh_SetupData.wIndex);
    UWORD wLength            = (ioreq->iouh_SetupData.wLength);

    switch(((ULONG)ioreq->iouh_SetupData.bmRequestType<<16)|((ULONG)ioreq->iouh_SetupData.bRequest)) {

        /* FIXME: Keep track on device address (what poseidon thinks the address is and what Linux think it is) */

        case ((((URTF_OUT|URTF_STANDARD|URTF_DEVICE))<<16)|(USR_SET_ADDRESS)):
            mybug_unit(-1, ("Filtering out SET_ADDRESS\n\n"));
            ioreq->iouh_Actual = ioreq->iouh_Length;
            return UHIOERR_NO_ERROR;
        break;

        /* FIXME: Parse messages related to configurations and use related libusb calls directly */

        case ((((URTF_OUT|URTF_STANDARD|URTF_DEVICE))<<16)|(USR_SET_CONFIGURATION)):
            mybug_unit(-1, ("Filtering out SET_CONFIGURATION\n\n"));
            ioreq->iouh_Actual = ioreq->iouh_Length;
            return UHIOERR_NO_ERROR;
        break;

    }

    mybug_unit(0, ("wLength %d\n", wLength));
    mybug_unit(0, ("ioreq->iouh_Length %d\n", ioreq->iouh_Length));

    rc = LIBUSBCALL(libusb_control_transfer, dev_handle, bmRequestType, bRequest, wValue, wIndex, ioreq->iouh_Data, ioreq->iouh_Length, ioreq->iouh_NakTimeout);

    if(rc<0) {
        rc = 0;
    }

    ioreq->iouh_Actual = rc;

    mybug_unit(0, ("Done!\n\n"));
    return UHIOERR_NO_ERROR;   
}

int do_libusb_intr_transfer(struct IOUsbHWReq *ioreq) {
    struct VUSBHCIUnit *unit = (struct VUSBHCIUnit *) ioreq->iouh_Req.io_Unit;

    int rc = 0, transferred = 0;
    UBYTE endpoint = ioreq->iouh_Endpoint;

    mybug_unit(-1, ("ioreq->iouh_Length %d\n", ioreq->iouh_Length));
    mybug_unit(-1, ("direction %d\n", (ioreq->iouh_Dir)));

    switch(ioreq->iouh_Dir) {
        case UHDIR_IN:
            mybug_unit(-1, ("ioreq->iouh_Endpoint %d (IN)(%x)\n", endpoint, (endpoint|LIBUSB_ENDPOINT_IN)));
            rc = LIBUSBCALL(libusb_interrupt_transfer, dev_handle, (endpoint|LIBUSB_ENDPOINT_IN), (UBYTE *)ioreq->iouh_Data, ioreq->iouh_Length, &transferred, ioreq->iouh_NakTimeout);
        break;

        case UHDIR_OUT:
            mybug_unit(-1, ("ioreq->iouh_Endpoint %d (OUT)(%x)\n", endpoint, (endpoint|LIBUSB_ENDPOINT_OUT)));
            rc = LIBUSBCALL(libusb_interrupt_transfer, dev_handle, (endpoint|LIBUSB_ENDPOINT_OUT), (UBYTE *)ioreq->iouh_Data, ioreq->iouh_Length, &transferred, ioreq->iouh_NakTimeout);
        break;
    }

    mybug_unit(-1, ("libusb_interrupt_transfer rc = %d, transferred %d\n", rc, transferred));

    switch(rc) {
	    case LIBUSB_SUCCESS:
	        mybug_unit(-1, ("Success (no error)\n"));
        break;

	    case LIBUSB_ERROR_IO:
	        mybug_unit(-1, ("Input/output error\n"));
        break;

	    case LIBUSB_ERROR_INVALID_PARAM:
	        mybug_unit(-1, ("Invalid parameter\n"));
        break;

	    case LIBUSB_ERROR_ACCESS:
	        mybug_unit(-1, ("Access denied (insufficient permissions)\n"));
        break;

	    case LIBUSB_ERROR_NO_DEVICE:
	        mybug_unit(-1, ("No such device (it may have been disconnected)\n"));
        break;

	    case LIBUSB_ERROR_NOT_FOUND:
	        mybug_unit(-1, ("Entity not found\n"));
        break;

	    case LIBUSB_ERROR_BUSY:
	        mybug_unit(-1, ("Resource busy\n"));
        break;

	    case LIBUSB_ERROR_TIMEOUT:
	        mybug_unit(-1, ("Operation timed out\n"));
        break;

	    case LIBUSB_ERROR_OVERFLOW:
	        mybug_unit(-1, ("Overflow\n"));
        break;

	    case LIBUSB_ERROR_PIPE:
	        mybug_unit(-1, ("Pipe error */\n"));
        break;

	    case LIBUSB_ERROR_INTERRUPTED:
	        mybug_unit(-1, ("System call interrupted (perhaps due to signal)\n"));
        break;

	    case LIBUSB_ERROR_NO_MEM:
	        mybug_unit(-1, ("Insufficient memory\n"));
        break;

	    case LIBUSB_ERROR_NOT_SUPPORTED:
	        mybug_unit(-1, ("Operation not supported or unimplemented on this platform\n"));
        break;

	    case LIBUSB_ERROR_OTHER:
	        mybug_unit(-1, ("Other error\n"));
        break;
    }

    if(rc<0) {
        rc = 0;
    } else {
        if(transferred) {
            ioreq->iouh_Actual = transferred;
        }
    }

    mybug_unit(-1, ("Done!\n\n"));
    return UHIOERR_NO_ERROR;  
}

int do_libusb_bulk_transfer(struct IOUsbHWReq *ioreq) {
    struct VUSBHCIUnit *unit = (struct VUSBHCIUnit *) ioreq->iouh_Req.io_Unit;

    int rc = 0, transferred = 0;
    UBYTE endpoint = ioreq->iouh_Endpoint;

    mybug_unit(0, ("ioreq->iouh_Length %d\n", ioreq->iouh_Length));
    mybug_unit(0, ("direction %d\n", (ioreq->iouh_Dir)));

    switch(ioreq->iouh_Dir) {
        case UHDIR_IN:
            mybug_unit(0, ("ioreq->iouh_Endpoint %d (IN)\n", endpoint));
            rc = LIBUSBCALL(libusb_bulk_transfer, dev_handle, (endpoint|LIBUSB_ENDPOINT_IN), (UBYTE *)ioreq->iouh_Data, ioreq->iouh_Length, &transferred, ioreq->iouh_NakTimeout);
        break;

        case UHDIR_OUT:
            mybug_unit(0, ("ioreq->iouh_Endpoint %d (OUT)\n", endpoint));
            rc = LIBUSBCALL(libusb_bulk_transfer, dev_handle, (endpoint|LIBUSB_ENDPOINT_OUT), (UBYTE *)ioreq->iouh_Data, ioreq->iouh_Length, &transferred, ioreq->iouh_NakTimeout);
        break;
    }

    mybug_unit(0, ("libusb_bulk_transfer rc = %d, transferred %d\n", rc, transferred));

/*
    0 on success (and populates transferred) 
    LIBUSB_ERROR_TIMEOUT if the transfer timed out (and populates transferred) 
    LIBUSB_ERROR_PIPE if the endpoint halted 
    LIBUSB_ERROR_OVERFLOW if the device offered more data, see Packets and overflows 
    LIBUSB_ERROR_NO_DEVICE if the device has been disconnected 
    another LIBUSB_ERROR code on other failures 
*/
    if(rc<0) {
        rc = 0;
    } else {
        if(transferred) {
            ioreq->iouh_Actual = transferred;
        }
    }

    mybug_unit(0, ("Done!\n\n"));
    return UHIOERR_NO_ERROR;
}

int do_libusb_isoc_transfer(struct IOUsbHWReq *ioreq) {
    struct VUSBHCIUnit *unit = (struct VUSBHCIUnit *) ioreq->iouh_Req.io_Unit;

    //UWORD bmRequestType      = (ioreq->iouh_SetupData.bmRequestType) & (URTF_STANDARD | URTF_CLASS | URTF_VENDOR);
    //UWORD bmRequestDirection = (ioreq->iouh_SetupData.bmRequestType) & (URTF_IN | URTF_OUT);
    //UWORD bmRequestRecipient = (ioreq->iouh_SetupData.bmRequestType) & (URTF_DEVICE | URTF_INTERFACE | URTF_ENDPOINT | URTF_OTHER);

    //UWORD bRequest           = (ioreq->iouh_SetupData.bRequest);
    //UWORD wValue             = AROS_WORD2LE(ioreq->iouh_SetupData.wValue);
    //UWORD wIndex             = AROS_WORD2LE(ioreq->iouh_SetupData.wIndex);
    //UWORD wLength            = AROS_WORD2LE(ioreq->iouh_SetupData.wLength);

    mybug_unit(-1, ("Done!\n\n"));
    return 0;    
}
