/*
    Copyright (C) 1995-1998 AROS - The Amiga Replacement OS
    $Id$

    Desc: Set the current filesystem handler for a process.
    Lang: english
*/
#include "dos_intern.h"

/*****************************************************************************

    NAME */
#include <dos/dosextens.h>
#include <proto/dos.h>

	AROS_LH1(struct MsgPort *, SetFileSysTask,

/*  SYNOPSIS */
	AROS_LHA(struct MsgPort *, task, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 88, Dos)

/*  FUNCTION
	Set the default filesystem handler for the current process,
	the old filesystem handler will be returned.

    INPUTS
	task		- The new filesystem handler.

    RESULT
	The old filesystem handler.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	GetFileSysTask()

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)

    struct Process *pr = (struct Process *)FindTask(NULL);
    BPTR old;

    old = pr->pr_FileSystemTask;
    pr->pr_FileSystemTask = MKBADDR(task);
    return BADDR(old);

    AROS_LIBFUNC_EXIT
} /* SetFileSysTask */
