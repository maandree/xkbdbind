/* See LICENSE file for copyright and license details. */
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>
#include <X11/keysym.h>

#define eprint(STR) (fprintf(stderr, "%s: %s\n", argv0, (STR)), exit(1))

#define _MOD_CAPS  XCB_MOD_MASK_LOCK
#define _MOD_NUM   XCB_MOD_MASK_2

#define MOD_SHIFT  XCB_MOD_MASK_SHIFT
#define MOD_CTRL   XCB_MOD_MASK_CONTROL
#define MOD_ALT    XCB_MOD_MASK_1
#define MOD_HYPER  XCB_MOD_MASK_3
#define MOD_SUPER  XCB_MOD_MASK_4
#define MOD_ALTGR  XCB_MOD_MASK_5

#define CMD(...) (const char *[]){__VA_ARGS__, NULL}

struct hotkey {
	uint32_t key; /* xcb_keysym_t is uint32_t and xcb_keycode_t is uint8_t */
	uint16_t modifiers;
	pid_t repeatable;
	const char **command;
};

#include "config.h"

static char *argv0;
static xcb_connection_t *xconn;

static pid_t
spawn(const char **command)
{
	pid_t r;
	switch ((r = fork())) {
	case -1:
		perror(argv0);
		break;
	case 0:
		execvp(*command, (char *const *)command);
		fprintf(stderr, "%s: %s: %s\n", argv0, *command, strerror(errno));
		exit(1);
		break;
	default:
		break;
	}
	return r;
}

static void
sigchld(int signo)
{
	pid_t r;
	size_t i;
	r = wait(NULL);
	if (r > 0)
		for (i = 0; i < sizeof(hotkeys) / sizeof(*hotkeys); i++)
			if (hotkeys[i].repeatable == r)
				hotkeys[i].repeatable = -1;
	(void) signo;
}

static void
setup(void)
{
#define GRAB_KEY(KEY, MODS) (xcb_grab_key(xconn, 1, xscreen->root, (MODS), (KEY), XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC))

	const xcb_setup_t *xsetup;
	xcb_screen_iterator_t xscreen_iter;
	xcb_screen_t *xscreen;
	int default_xscreen;
	xcb_key_symbols_t *keysyms;
	size_t i;

	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		eprint("failed to setup handler for SIGCHLD");

	xconn = xcb_connect(NULL, &default_xscreen);
	if (!xconn || xcb_connection_has_error(xconn))
		eprint("failed to connect to display");
	if ((xsetup = xcb_get_setup(xconn)) == NULL)
		eprint("failed to get list of X screens");
	xscreen_iter = xcb_setup_roots_iterator(xsetup);
	if (xscreen_iter.rem <= default_xscreen)
		eprint("could not find the X screen");
	for (; default_xscreen && xscreen_iter.rem; default_xscreen--)
		xcb_screen_next(&xscreen_iter);
	if (!xscreen_iter.rem || !(xscreen = xscreen_iter.data))
		eprint("could not find the X screen");

	keysyms = xcb_key_symbols_alloc(xconn);
	for (i = 0; i < sizeof(hotkeys) / sizeof(*hotkeys); i++) {
		hotkeys[i].key = *xcb_key_symbols_get_keycode(keysyms, hotkeys[i].key);
		GRAB_KEY(hotkeys[i].key, hotkeys[i].modifiers);
		GRAB_KEY(hotkeys[i].key, hotkeys[i].modifiers | _MOD_CAPS);
		GRAB_KEY(hotkeys[i].key, hotkeys[i].modifiers | _MOD_NUM);
		GRAB_KEY(hotkeys[i].key, hotkeys[i].modifiers | _MOD_CAPS | _MOD_NUM);
		if (hotkeys[i].repeatable)
			hotkeys[i].repeatable = -1;
	}
	xcb_key_symbols_free(keysyms);

	xcb_flush(xconn);
}

int
main(int argc, char *argv[])
{
	xcb_generic_event_t *e, *next_e = NULL;
	xcb_keycode_t key;
	uint16_t mods;
	int suppress = 0;
	size_t i;

	argv0 = argv[0];
	if (argc > 1) {
		fprintf(stderr, "usage: %s\n", argv0);
		return 1;
	}

	setup();

	while ((e = next_e ? next_e : xcb_wait_for_event(xconn))) {
		next_e = xcb_poll_for_event(xconn);
		if (suppress > 0) {
			suppress = 0;
			continue;
		}
		switch (e->response_type & ~0x80) {
		case XCB_KEY_RELEASE:
			if (suppress >= 0 && next_e && (next_e->response_type & ~0x80) == XCB_KEY_PRESS &&
			    ((xcb_key_press_event_t *)next_e)->time == ((xcb_key_press_event_t *)e)->time)
				suppress = 1;
			break;
		case XCB_KEY_PRESS:
			key = ((xcb_key_press_event_t *)e)->detail;
			mods = ((xcb_key_press_event_t *)e)->state & ~(_MOD_CAPS | _MOD_NUM);
			for (i = 0; i < sizeof(hotkeys) / sizeof(*hotkeys); i++) {
				if (key == hotkeys[i].key && mods == hotkeys[i].modifiers) {
					suppress = hotkeys[i].repeatable ? -1 : 0;
					if (hotkeys[i].repeatable == -1)
						hotkeys[i].repeatable = spawn(hotkeys[i].command);
					else if (!hotkeys[i].repeatable)
						spawn(hotkeys[i].command);
					break;
				}
			}
			break;
		}
	}

	perror(argv0);
	return 1;
}
