#ifndef DBGOUT_H
#define DBGOUT_H

/* Usage */
/* dbgf(int colour, char *fmt, ...);
 * colour:
 *   DBG_CL(bg_col,fg_col) macro can be used to
 *   specify background colour and foreground colour.
 *   Use the colour name provided by
 *   enum "dbg_colour_bg" & "dbg_colour" respectively.
 *   Prefix of enum name, "DBG_" & "DBG_BG_", should be
 *   omitted WITH this macro.
 *   You do not have to choose background colour.
 * fmt:
 *   %d, %u, %s, %c and %f can be used for format string.
 */


#define DBG_CL(BG_COL,COL) ((DBG_BG_##BG_COL<<8) | DBG_##COL)

enum dbg_colour{
  DBG_RED = 0,
  DBG_GREEN,
  DBG_YELLOW,
  DBG_BLUE,
  DBG_MAGENTA,
  DBG_CYAN,
  DBG_WHITE,
  DBG_COLOUR_DEFAULT,
  DBG_COLOUR_NUM
};

enum dbg_colour_bg{
  DBG_BG_COLOUR_DEFAULT = 0,
  DBG_BG_RED,
  DBG_BG_GREEN,
  DBG_BG_YELLOW,
  DBG_BG_BLUE,
  DBG_BG_MAGENTA,
  DBG_BG_CYAN,
  DBG_BG_GRAY,
  DBG_BG_NAVY_BLUE,
  DBG_BG_COTTON_PINK,
  DBG_BG_COLOUR_NUM
};


#ifdef DEBUG
#define dbgf(...) dbgf_(__VA_ARGS__)
#define dbgf_net(...) dbgf_(DBG_CL(COTTON_PINK,WHITE),__VA_ARGS__)
#else
#define dbgf(...)
#define dbgf_net(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  void dbgf_(int colour,const char *fmt,...);
  
#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
