/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */

static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     	/* 0 means no systray */

static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

static const Bool viewontag         = True;     /* Switch view on tag switch */

/*  Display modes of the tab bar: never shown, always shown, shown only in  */
/*  monocle mode in the presence of several windows.                        */
/*  Modes after showtab_nmodes are disabled.                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab			= showtab_auto;        /* Default tab bar show mode */
static const int toptab				= True;               /* False means bottom tab bar */

static const char *fonts[]          = { "monospace:size=13", "Font Awesome 5 Free:style=Solid:size=12", "Font Awesome 5 Brands:size=12" };
static const char dmenufont[]       = "monospace:size=13";

static const char col_gray1[]       = "#2e3440";	//background color
static const char col_gray2[]       = "#3b4252";	//inactive window border color
static const char col_gray3[]       = "#e5e9f0";	//font color
static const char col_gray4[]       = "#2e3440";	//current tag and current window font color
static const char col_cyan[]        = "#88c0d0";	//top bar second color and active window border color
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *tagsalt[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#include "shiftview.c"

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "discord",  NULL,       NULL,       1 << 4,       0,           -1 },
 	{ "notion",   NULL,       NULL,       1 << 6,       0,           -1 },
	{ "notion", NULL, "notion-enhancer menu", 0,        True,        -1 },
	{ "spotify",  NULL,       NULL,       1 << 5,       0,           -1 },
	{ "clearine", NULL,       "clearine", 0,            True,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "t",      tile },    /* first entry is default */
	{ "f",      NULL },    /* no layout function means floating behavior */
	{ "tab",      monocle },
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

static const char *rofidrun[] = { "rofi", "-show", "drun"};

static const char *lockcmd[] = { "clearine", NULL };

static const char *termcmd[]  = { "kitty", NULL };
static const char *fmcmd[] = { "nautilus", NULL };
static const char *browsercmd[] = { "vivaldi-stable", NULL };

/* volume and brightness control */
static const char *togglemutecmd[] = { "amixer", "-D", "pulse", "sset", "Master", "toggle", NULL };
static const char *volumeup[] = { "amixer", "-D", "pulse", "sset", "Master", "5%+", NULL };
static const char *volumedown[] = { "amixer", "-D", "pulse", "sset", "Master", "5%-", NULL };
static const char *brightnessup[] = { "light", "-A", "10"};
static const char *brightnessdown[] = { "light", "-U", "10"};

static Key keys[] = {
	/* modifier                     key        function        argument */
	//{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } }, 	//dmenu
	{ MODKEY,                       XK_t,      spawn,          {.v = termcmd } }, 		//launch terminal
	{ MODKEY,                       XK_f,      spawn,          {.v = fmcmd } }, 		//launch file manager
	{ MODKEY,                       XK_r,      spawn,          {.v = browsercmd } }, 	//launch browser
	//{ MODKEY,                       XK_b,      togglebar,      {0} }, 			//toggle top bar
	{ MODKEY,                       XK_w,      tabmode,        {-1} }, 			//toggle showing tabs
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } }, 		//switch focus down
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } }, 		//switch focus up
	{ MODKEY|ShiftMask,             XK_u,      incnmaster,     {.i = +1 } }, 		//increase master stack
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } }, 		//decrease master stack
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} }, 		//decrease size
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} }, 		//increase size
	{ MODKEY,                       XK_Return, zoom,           {0} }, 			//change master
	//{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} }, 			//kill focused app
	{ MODKEY,                       XK_b,      setlayout,      {.v = &layouts[0]} }, 	//tiling mode
	{ MODKEY,                       XK_n,      setlayout,      {.v = &layouts[1]} }, 	//floating mode
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} }, 	//monocle/tab mode
	{ MODKEY,                       XK_v,      setlayout,      {0} }, 			//switch layout
	{ MODKEY|ShiftMask,             XK_n,      togglefloating, {0} }, 			//toggle floating on window
	//{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	//{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	//{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	//{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	//{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } }, 			//decrease gaps
	{ MODKEY,                       XK_plus,   setgaps,        {.i = +1 } }, 			//increase gaps
	{ MODKEY|ShiftMask,             XK_plus,   setgaps,        {.i = 0  } }, 			//set gaps to 0
	{ MODKEY,                       XK_i,      shiftview,  { .i = +1 } }, 				//move to next view
	{ MODKEY,                       XK_u,      shiftview,  { .i = -1 } }, 				//move to previous view
	{ MODKEY,                       XK_Tab,    togglealttag,   {0} }, 				//toggle secoundary tag view
	{ 0,                            XF86XK_AudioMute, spawn,      {.v = togglemutecmd } }, 		//mute volume
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = volumeup } }, 		//volume up
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = volumedown } }, 		//volume down
	{ 0,                            XF86XK_MonBrightnessUp, spawn, {.v = brightnessup} }, 		//brightness up
	{ 0,                            XF86XK_MonBrightnessDown, spawn, {.v = brightnessdown} }, 	//brightness down
	{ MODKEY,                       XK_space,  spawn,          {.v = rofidrun} }, 			//rofi drun
	{ MODKEY|ShiftMask,             XK_z,      quit,           {0} }, 				//quit dwm (will restart with dwm-run)
	{ MODKEY|Mod1Mask,              XK_m,      spawn,          {.v = lockcmd} },			//reboot


	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
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
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};

