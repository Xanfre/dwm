/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 5;        /* snap pixel */
static const unsigned int gap       = 10;       /* gap size */
static const int enablegaps         = 1;        /* 0 means no gaps */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Hack:style=Regular:pixelsize=12" };
static const char dmenufont[]       = "Hack:style=Regular:pixelsize=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#0b6623";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  monitor */
	{ "Firefox", NULL,     NULL,           0,         0,          -1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#include <X11/XF86keysym.h>
#include "gaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "[D]",      deck },
	{ "TTT",      bstack },
	{ "|M|",      centeredmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *termcmdtab[]  = { "tabbed", "-o", col_gray1, "-O", col_gray3, "-t", col_cyan, "-T", col_gray4, "-r", "2", "st", "-w", "''",  NULL };
static const char *disploff[] = { "xset", "dpms", "force", "off", NULL };
static const char *displlock[] = { "slock", NULL };
static const char *status[] = { "instance", "slstatus", "-10", NULL };
static const char *findcur[] = { "instance", "find-cursor", "-s", "150", "-d", "50", "-w", "1000", "-g", "-o", NULL };
static const char *browser[] = { "librewolf", NULL };
static const char *voldown[] = { "amixer", "sset", "Master", "1-", NULL };
static const char *volup[] = { "amixer", "sset", "Master", "1+", NULL };
static const char *volmute[] = { "amixer", "sset", "Master", "toggle", NULL };
static const char *musstop[] = { "cmus-remote", "-s", NULL };
static const char *musprev[] = { "cmus-remote", "-r", NULL };
static const char *muspause[] = { "cmus-remote", "-u", NULL };
static const char *musnext[] = { "cmus-remote", "-n", NULL };

static const Key keys[] = {
	/* modifier                     key                      function        argument */
	{ MODKEY,                       XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,             	        XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return,               spawn,          {.v = termcmdtab } },
	{ MODKEY,                       XK_BackSpace,            spawn,          {.v = disploff } },
	{ MODKEY,                       XK_BackSpace,            spawn,          {.v = displlock } },
	{ MODKEY,                       XK_grave,                spawn,          {.v = status } },
	{ MODKEY,                       XK_b,                    togglebar,      {0} },
	{ MODKEY,                       XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_o,                    incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                    setmfact,       {.f = -0.01} },
	{ MODKEY,                       XK_l,                    setmfact,       {.f = +0.01} },
	{ MODKEY|ShiftMask,             XK_j,                    moveresize,     {.v = (int[]){ 0, 10, 0, 0} } },
	{ MODKEY|ShiftMask,             XK_k,                    moveresize,     {.v = (int[]){ 0, -10, 0, 0} } },
	{ MODKEY|ShiftMask,             XK_h,                    moveresize,     {.v = (int[]){ -10, 0, 0, 0} } },
	{ MODKEY|ShiftMask,             XK_l,                    moveresize,     {.v = (int[]){ 10, 0, 0, 0} } },
	{ MODKEY|ControlMask,           XK_j,                    moveresize,     {.v = (int[]){ 0, 0, 0, 10} } },
	{ MODKEY|ControlMask,           XK_k,                    moveresize,     {.v = (int[]){ 0, 0, 0, -10} } },
	{ MODKEY|ControlMask,           XK_h,                    moveresize,     {.v = (int[]){ 0, 0, -10, 0} } },
	{ MODKEY|ControlMask,           XK_l,                    moveresize,     {.v = (int[]){ 0, 0, 10, 0} } },
	{ MODKEY|ControlMask|ShiftMask, XK_j,                    movethrow,      {.ui = DIR_S } },
	{ MODKEY|ControlMask|ShiftMask, XK_k,                    movethrow,      {.ui = DIR_N } },
	{ MODKEY|ControlMask|ShiftMask, XK_h,                    movethrow,      {.ui = DIR_W } },
	{ MODKEY|ControlMask|ShiftMask, XK_l,                    movethrow,      {.ui = DIR_E } },
	{ MODKEY|ControlMask|ShiftMask, XK_m,                    movethrow,      {.ui = DIR_C } },
	{ MODKEY|ShiftMask,             XK_Tab,                  zoom,           {0} },
	{ MODKEY,                       XK_equal,                incrgaps,       {.i = +1 } },
	{ MODKEY,                       XK_minus,                incrgaps,       {.i = -1 } },
	{ MODKEY,                       XK_g,                    togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_g,                    defaultgaps,    {0} },
	{ MODKEY,                       XK_c,                    togglecur,      {0} },
	{ MODKEY,                       XK_Tab,                  view,           {0} },
	{ MODKEY,                       XK_x,                    killclient,     {0} },
	{ MODKEY,                       XK_t,                    setlayout,      {.v = &layouts[0]} }, /* tile */
	{ MODKEY,                       XK_m,                    setlayout,      {.v = &layouts[1]} }, /* monocle */
	{ MODKEY,                       XK_y,                    setlayout,      {.v = &layouts[2]} }, /* spiral */
	{ MODKEY,                       XK_u,                    setlayout,      {.v = &layouts[3]} }, /* dwindle */
	{ MODKEY,                       XK_p,                    setlayout,      {.v = &layouts[4]} }, /* deck */
	{ MODKEY,                       XK_n,                    setlayout,      {.v = &layouts[5]} }, /* bstack */
	{ MODKEY,                       XK_v,                    setlayout,      {.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY,                       XK_z,                    setlayout,      {.v = &layouts[7]} }, /* none */
	{ MODKEY|ShiftMask,             XK_space,                setlayout,      {0} },
	{ MODKEY,                       XK_space,                togglefloating, {0} },
	{ MODKEY,                       XK_f,                    togglefullscr,  {0} },
	{ MODKEY,                       XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                                    0)
	TAGKEYS(                        XK_2,                                    1)
	TAGKEYS(                        XK_3,                                    2)
	TAGKEYS(                        XK_4,                                    3)
	TAGKEYS(                        XK_5,                                    4)
	TAGKEYS(                        XK_6,                                    5)
	TAGKEYS(                        XK_7,                                    6)
	TAGKEYS(                        XK_8,                                    7)
	TAGKEYS(                        XK_9,                                    8)
	{ MODKEY,                       XK_s,                    spawn,          {.v = findcur } },
	{ MODKEY,                       XK_w,                    spawn,          {.v = browser } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn,          {.v = voldown } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn,          {.v = volup } },
	{ 0,                            XF86XK_AudioMute,        spawn,          {.v = volmute } },
	{ 0,                            XF86XK_AudioStop,        spawn,          {.v = musstop } },
	{ 0,                            XF86XK_AudioPrev,        spawn,          {.v = musprev } },
	{ 0,                            XF86XK_AudioPlay,        spawn,          {.v = muspause } },
	{ 0,                            XF86XK_AudioNext,        spawn,          {.v = musnext } },
	{ MODKEY|ShiftMask,             XK_q,                    quit,           {0}  },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

