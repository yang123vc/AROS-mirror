/*
    (C) 1995-99 AROS - The Amiga Research OS
    $Id$

    Desc: Intuition function MoveWindowInFrontOf()
    Lang: english
*/
#include "intuition_intern.h"

/*****************************************************************************

    NAME */
	#include <clib/intuition_protos.h>

	AROS_LH2(void, MoveWindowInFrontOf,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, window, A0),
	AROS_LHA(struct Window *, behindwindow, A1),

/*  LOCATION */
	struct IntuitionBase *, IntuitionBase, 80, Intuition)

/*  FUNCTION
	Arrange the relative depth of a window.

    INPUTS
	window - the window to reposition
	behindwindow - the window the other one will be brought in front of

    RESULT
	None.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	WindowToFront(), WindowToBack(), layers.library/MoveLayerInFrontOf()

    INTERNALS
	Uses layers.library/MoveLayerInFrontOf().

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct IntuitionBase *,IntuitionBase)

    struct shortIntuiMessage * msg;
    
    msg = AllocMem(sizeof(struct shortIntuiMessage), MEMF_CLEAR);
 
    if (NULL != msg)
    {
      msg->Class       = IDCMP_WBENCHMESSAGE;
      msg->Code        = IMCODE_MOVEWINDOWINFRONTOF;
      msg->Window      = window;
      msg->BehindWindow= behindwindow;
      
      PutMsg(window->WindowPort, (struct Message *)msg); 
    }   

    AROS_LIBFUNC_EXIT
} /* MoveWindowInFrontOf */
