/* 
    Copyright � 1999, David Le Corfec.
    Copyright � 2002, The AROS Development Team.
    All rights reserved.

    $Id$
*/

#ifndef __ZUNE_PREFS_H__
#define __ZUNE_PREFS_H__

#ifndef LIBRARIES_COMMODITIES_H
#include <libraries/commodities.h>
#endif

#ifndef LIBRARIES_MUI_H
#include "mui.h"
#endif

typedef enum CycleMenuPosition {
    CYCLE_MENU_POSITION_CENTERED,
    CYCLE_MENU_POSITION_TOP,
    CYCLE_MENU_POSITION_BOTTOM,
} CycleMenuPosition;

typedef enum GroupTitlePosition {
    GROUP_TITLE_POSITION_CENTERED,
    GROUP_TITLE_POSITION_LEFT,
    GROUP_TITLE_POSITION_RIGHT,
} GroupTitlePosition;

typedef enum GroupTitleColor {
    GROUP_TITLE_COLOR_WHITE,
    GROUP_TITLE_COLOR_BLACK,
    GROUP_TITLE_COLOR_3D,
} GroupTitleColor;

typedef enum WindowPosition {
    WINDOW_POSITION_FORGET_ON_EXIT,
    WINDOW_POSITION_SAVE_ON_EXIT,
} WindowPosition;

typedef enum DNDLook {
    DND_LOOK_ALWAYS_SOLID,
    DND_LOOK_GHOSTED_ON_BOX,
    DND_LOOK_GHOSTED_OUTSIDE_BOX,
    DND_LOOK_ALWAYS_GHOSTED,
} DNDLook;

typedef enum ScrollbarLook {
    SB_LOOK_TOP,
    SB_LOOK_MIDDLE,
    SB_LOOK_BOTTOM,
} ScrollbarLook;

typedef enum BalancingLook {
    BALANCING_SHOW_FRAMES,
    BALANCING_SHOW_OBJECTS,
} BalancingLook;

typedef enum ActiveObjectLook {
    ACTIVE_OBJECT_LOOK_FRAME,
    ACTIVE_OBJECT_LOOK_CORNER,
} ActiveObjectLook;

typedef struct _ZuneKeySpec {
    STRPTR readable_hotkey;
    LONG ix_well;
    IX ix;
} ZuneKeySpec;

typedef enum RegisterLook {
    REGISTER_LOOK_TRADITIONAL,
    REGISTER_LOOK_GADTOOLS,
} RegisterLook;

/*
 * User Prefs for interface drawing
 */

struct ZunePrefsNew
{
    char *fonts[-MUIV_Font_NegCount];
    char *imagespecs[MUII_Count];
    struct MUI_FrameSpec frames[MUIV_Frame_Count];

    /* Groups */
    GroupTitlePosition group_title_position;
    GroupTitleColor    group_title_color;
    WORD     group_hspacing;
    WORD     group_vspacing;

    /* Windows */
    WindowPosition  window_position;
    WORD     window_inner_left;
    WORD     window_inner_right;
    WORD     window_inner_top;
    WORD     window_inner_bottom;

    /* MUI Keys */
    ZuneKeySpec muikeys[MUIKEY_COUNT];

    /* Zune registers */
    RegisterLook register_look;

    /* Buttons */
    WORD     radiobutton_hspacing; /* yet unused, remove this comment when handled */
    WORD     radiobutton_vspacing; /* yet unused, remove this comment when handled */

    /* Cycles */
    CycleMenuPosition cycle_menu_position; /* yet unused, remove this comment when handled */
    WORD     cycle_menu_min_entries; /* yet unused, remove this comment when handled */
    WORD     cycle_menu_speed; /* yet unused, remove this comment when handled */
    BOOL     cycle_menu_recessed_entries; /* yet unused, remove this comment when handled */

    /* Strings */
    struct MUI_PenSpec string_bg_active; /* yet unused, remove this comment when handled */
    struct MUI_PenSpec string_text_active; /* yet unused, remove this comment when handled */
    struct MUI_PenSpec string_bg_inactive; /* yet unused, remove this comment when handled */
    struct MUI_PenSpec string_text_inactive; /* yet unused, remove this comment when handled */

    /* Lists */
    WORD     list_linespacing; /* yet unused, remove this comment when handled */

    /* Navigation */
    BOOL                 dragndrop_left_button; /* yet unused, remove this comment when handled */
    STRPTR               dragndrop_left_modifier; /* yet unused, remove this comment when handled */
    BOOL                 dragndrop_middle_button; /* yet unused, remove this comment when handled */
    STRPTR               dragndrop_middle_modifier; /* yet unused, remove this comment when handled */
    LONG                 dragndrop_autostart; /* yet unused, remove this comment when handled */
    DNDLook              dragndrop_look; /* yet unused, remove this comment when handled */
    BalancingLook        balancing_look; /* yet unused, remove this comment when handled */
    ActiveObjectLook     active_object_look; /* yet unused, remove this comment when handled */
    struct MUI_PenSpec   active_object_color; /* yet unused, remove this comment when handled */

    /* Scrollbars */
    ScrollbarLook     sb_look; /* yet unused, remove this comment when handled */
};

#endif


