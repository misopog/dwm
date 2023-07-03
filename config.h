/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "movestack.c"

/*Gaps configuration*/

/*
 * horiz inner gap between windows
 * vert inner gap between windows
 * horiz outer gap between windows and screen edge
 * vert outer gap between windows and screen edge
*/
/*
static const unsigned int gappih    = 0;       
static const unsigned int gappiv    = 0;      
static const unsigned int gappoh    = 0;     
static const unsigned int gappov    = 0;    
*/
static const unsigned int gappih    = 6;   
static const unsigned int gappiv    = 6;  
static const unsigned int gappoh    = 6; 
static const unsigned int gappov    = 6;

static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */

/* appearance */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray             = 1;   /* 0 means no systray */
static const char *fonts[]          = { "JetBrains Mono Nerd Font:size=10", "Noto Sans CJK JP:style=Regular:pixelsize=12" }; 
static const char dmenufont[]       = "JetBrains Mono:size=10";
static const unsigned int baralpha = 1280731835;
static const unsigned int borderalpha = OPAQUE;
static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_gray5[]       = "#000000";
static const char *colors[][3]      = {

/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray1 },
	[SchemeSel]  = { col_gray4, col_gray5, col_gray2 },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, OPAQUE, borderalpha },
	[SchemeSel]  = { OPAQUE, OPAQUE, borderalpha },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcmus", "-g", "126x31", "-e", "cmus", NULL };
const char *spcmd3[] = {"st", "-n", "spcalc", "-g", "126x31", "-e", "qalc", NULL };
const char *spcmd4[] = {"st", "-n", "spfm", "-g", "126x31", "-e", "ranger", NULL };

static Sp scratchpads[] = {
	/* name	       cmd */
	{"spterm",    spcmd1},
	{"spcmus",    spcmd2},
	{"spcalc",    spcmd3},
	{"spfm",    spcmd4},
};

/* tagging */
static const char *tags[] = { "一", "二", "三", "四", "五", "六" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       0,       0,           -1 },
	{ "St",       NULL,       NULL,            0,         0,          1,           0,        -1 },
	{ NULL,     "spterm",	  NULL,	      SPTAG(0),	      1,				 -1 },	
	{ NULL,     "spcmus",	  NULL,	      SPTAG(1),	      1,				 -1 },	
	{ NULL,     "spcalc",	  NULL,	      SPTAG(2),	      1,				 -1 },	
	{ NULL,     "spfm",	  NULL,	      SPTAG(3),	      1,				 -1 },	
	{ NULL,       NULL,       "Event Tester",  0,         0,          0,           1,        -1 }, /* xev */

};

static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
 
#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[\\]",     dwindle },
	{ "[]=",      tile }, 
	{ "[@]",      spiral },
	{ "[M]",      monocle },
	{ "D[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
 
};
/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define print	    0x0000ff61

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-b", "-fn", dmenufont, NULL };
static const char *rofidruncmd[] = { "rofi", "-show", "drun", "-show-icons", NULL };
static const char *rofiruncmd[] = { "rofi", "-show", "run", "-show-icons", NULL };
static const char *termcmd[]  = { "st"};

static Key keys[] = {
	/* modifier                     key        function        argument */
 	{ MODKEY,                       XK_s,      togglescratch,  {.ui = 0} },
 	{ MODKEY,                       XK_m,      togglescratch,  {.ui = 1} },
 	{ MODKEY,                       XK_c,      togglescratch,  {.ui = 2} },
 	{ MODKEY|ShiftMask,             XK_e,      togglescratch,  {.ui = 3} },
  { 0,                            print,     spawn,          SHCMD("screen full")},
  { MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("screen select")},
  { MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("walrandom")},
 	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("cam") },
 	{ MODKEY,                       XK_w,      spawn,          SHCMD("qutebrowser") },
 	{ MODKEY,                       XK_e,      spawn,          SHCMD("pcmanfm") },
/*	{ MODKEY,			                  XK_d,	     spawn,          {.v = dmenucmd } }, */
	{ MODKEY,			                  XK_d,	     spawn,          {.v = rofidruncmd } },
	{ MODKEY|ShiftMask,             XK_d,	     spawn,          {.v = rofiruncmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ 0,		     XF86XK_AudioRaiseVolume,      spawn, 	       SHCMD("changevolume up")   },
	{ 0, 	     	 XF86XK_AudioLowerVolume,      spawn, 	       SHCMD("changevolume down") },
  { 0,                XF86XK_AudioMute,      spawn,          SHCMD("changevolume mute") },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
/*	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },*/
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
  { MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_space,  setlayout,      {0} },
	{ MODKEY,                       XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_r,      quit,           {0} }, /* with my dotfiles, it restarts dwm, it does not close it*/
  { MODKEY|ShiftMask,             XK_q,      spawn,          SHCMD("pkill X")},
	{ MODKEY,                       XK_i,      incrgaps,       {.i = +1 } },
	{ MODKEY,                       XK_u,      incrgaps,       {.i = -1 } },
/*	{ MODKEY|Mod1Mask,              XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
*/
}; 

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
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

