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

struct PanelViewInst
{
        ULONG   idcmp;
        Object  *panel;                 // Destination Panel Object
        struct  Gadget  *sliderright;
        struct  Gadget  *sliderbottom;

        LONG    minx;
        LONG    miny;
        LONG    maxx;
        LONG    maxy;
        LONG    left;
        LONG    top;
        UWORD   movegadid;
};


