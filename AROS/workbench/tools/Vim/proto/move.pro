/* move.c */
extern void update_topline_redraw __ARGS((void));
extern void update_topline __ARGS((void));
extern void update_curswant __ARGS((void));
extern void check_cursor_moved __ARGS((win_T *wp));
extern void changed_window_setting __ARGS((void));
extern void changed_window_setting_win __ARGS((win_T *wp));
extern void set_topline __ARGS((win_T *wp, linenr_T lnum));
extern void changed_cline_bef_curs __ARGS((void));
extern void changed_cline_bef_curs_win __ARGS((win_T *wp));
extern void changed_line_abv_curs __ARGS((void));
extern void changed_line_abv_curs_win __ARGS((win_T *wp));
extern void validate_botline __ARGS((void));
extern void invalidate_botline __ARGS((void));
extern void invalidate_botline_win __ARGS((win_T *wp));
extern void approximate_botline_win __ARGS((win_T *wp));
extern int cursor_valid __ARGS((void));
extern void validate_cursor __ARGS((void));
extern void validate_cline_row __ARGS((void));
extern void validate_virtcol __ARGS((void));
extern void validate_virtcol_win __ARGS((win_T *wp));
extern void validate_cursor_col __ARGS((void));
extern int win_col_off __ARGS((win_T *wp));
extern int curwin_col_off __ARGS((void));
extern int win_col_off2 __ARGS((win_T *wp));
extern int curwin_col_off2 __ARGS((void));
extern void curs_columns __ARGS((int scroll));
extern void scrolldown __ARGS((long line_count, int byfold));
extern void scrollup __ARGS((long line_count, int byfold));
extern void check_topfill __ARGS((win_T *wp, int down));
extern void scrolldown_clamp __ARGS((void));
extern void scrollup_clamp __ARGS((void));
extern void scroll_cursor_top __ARGS((int min_scroll, int always));
extern void set_empty_rows __ARGS((win_T *wp, int used));
extern void scroll_cursor_bot __ARGS((int min_scroll, int set_topbot));
extern void scroll_cursor_halfway __ARGS((int atend));
extern void cursor_correct __ARGS((void));
extern int onepage __ARGS((int dir, long count));
extern void halfpage __ARGS((int flag, linenr_T Prenum));
/* vim: set ft=c : */
