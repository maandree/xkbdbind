#define MOD MOD_SUPER

static struct hotkey hotkeys[] = {
	{XK_p,  MOD,             0, CMD("dmenu_run", "-fn", "schumacher clean-8")},
	{XK_F4, MOD | MOD_SHIFT, 0, CMD("xkill")},
};
