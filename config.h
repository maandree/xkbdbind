#define MOD MOD_SUPER


/* Column 1: Key to press (definitions are available in /usr/include/X11/keysymdef.h)
 * 
 * Column 2: Modifies to combine with the keypress
 *           (OR of MOD_SHIFT, MOD_CTRL, MOD_ALT, MOD_HYPER, MOD_SUPER, MOD_ALTGR)
 * 
 * Column 3: Non-zero if the command shall be spawn repeatedly if the key is held down
 * 
 * Column 4: The command to spawn. Each argument shall be its own string, there is
 *           no shell syntax, and all arguments shall be wrapped in a CMD, e.g.
 *           CMD("printf", "%s\n", "Hello world!"), to print "Hello world!" (not
 *           a particularly useful example).
 */

static struct hotkey hotkeys[] = {
	{XK_p,   MOD,              0,  CMD("dmenu_run", "-fn", "schumacher clean-8")},
	{XK_F4,  MOD | MOD_SHIFT,  0,  CMD("xkill")},
};
