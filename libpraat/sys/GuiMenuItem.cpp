/* GuiMenuItem.cpp
 *
 * Copyright (C) 1992-2012,2013,2015 Paul Boersma, 2013 Tom Naughton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "GuiP.h"
#if gtk
	#include <gdk/gdkkeysyms.h>
#endif

#define _motif_SHIFT_MASK  1
#define _motif_COMMAND_MASK  2
#define _motif_OPTION_MASK  4

Thing_implement (GuiMenuItem, GuiThing, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if gtk
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) _GuiObject_getUserData (widget)
#elif cocoa
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) [(GuiCocoaMenuItem *) widget userData];
#elif motif
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) widget -> userData
#endif

#if motif
static void NativeMenuItem_setText (GuiObject me) {
	int acc = my motiff.pushButton.acceleratorChar, modifiers = my motiff.pushButton.acceleratorModifiers;
	#if win
		static MelderString title { 0 };
		if (acc == 0) {
			MelderString_copy (& title, _GuiWin_expandAmpersands (my name));
		} else {
			static const char32 *keyStrings [256] = {
				0, U"<-", U"->", U"Up", U"Down", U"PAUSE", U"Del", U"Ins", U"Backspace", U"Tab", U"LineFeed", U"Home", U"End", U"Enter", U"PageUp", U"PageDown",
				U"Esc", U"F1", U"F2", U"F3", U"F4", U"F5", U"F6", U"F7", U"F8", U"F9", U"F10", U"F11", U"F12", 0, 0, 0,
				U"Space", U"!", U"\"", U"#", U"$", U"%", U"&", U"\'", U"(", U")", U"*", U"+", U",", U"-", U".", U"/",
				U"0", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8", U"9", U":", U";", U"<", U"=", U">", U"?",
				U"@", U"A", U"B", U"C", U"D", U"E", U"F", U"G", U"H", U"I", U"J", U"K", U"L", U"M", U"N", U"O",
				U"P", U"Q", U"R", U"S", U"T", U"U", U"V", U"W", U"X", U"Y", U"Z", U"[", U"\\", U"]", U"^", U"_",
				U"`", U"a", U"b", U"c", U"d", U"e", U"f", U"g", U"h", U"i", U"j", U"k", U"l", U"m", U"n", U"o",
				U"p", U"q", U"r", U"s", U"t", U"u", U"v", U"w", U"x", U"y", U"z", U"{", U"|", U"}", U"~", U"Del",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, U"[", U"]", U",", U"?", U".", U"\\",
				U";", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, U"-", U"`", U"=", U"\'", 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const char32 *keyString = keyStrings [acc] ? keyStrings [acc] : U"???";
			MelderString_copy (& title, _GuiWin_expandAmpersands (my name), U"\t",
				modifiers & _motif_COMMAND_MASK ? U"Ctrl-" : NULL,
				modifiers & _motif_OPTION_MASK ? U"Alt-" : NULL,
				modifiers & _motif_SHIFT_MASK ? U"Shift-" : NULL, keyString);
		}
		ModifyMenu (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | MF_STRING, my nat.entry.id, Melder_peek32toW (title.string));
	#elif mac
		static int theGlyphs [1+31] = { 0,
			kMenuLeftArrowDashedGlyph, kMenuRightArrowDashedGlyph, kMenuUpArrowDashedGlyph, kMenuDownwardArrowDashedGlyph, 0,
			kMenuDeleteRightGlyph, 0, kMenuDeleteLeftGlyph, kMenuTabRightGlyph, 0,
			0, 0, kMenuReturnGlyph, kMenuPageUpGlyph, kMenuPageDownGlyph,
			kMenuEscapeGlyph, kMenuF1Glyph, kMenuF2Glyph, kMenuF3Glyph, kMenuF4Glyph,
			kMenuF5Glyph, kMenuF6Glyph, kMenuF7Glyph, kMenuF8Glyph, kMenuF9Glyph,
			kMenuF10Glyph, kMenuF11Glyph, kMenuF12Glyph, 0, 0,
			0 };
		SetMenuItemTextWithCFString (my nat.entry.handle, my nat.entry.item, (CFStringRef) Melder_peek32toCfstring (my name));
		if (acc > 32) {
			SetItemCmd (my nat.entry.handle, my nat.entry.item, acc);
		} else {
			Melder_assert (acc > 0 && acc < 32);
			SetItemCmd (my nat.entry.handle, my nat.entry.item, ' ');   /* Funny that this should be needed. */
			SetMenuItemKeyGlyph (my nat.entry.handle, my nat.entry.item, theGlyphs [acc]);
		}
		SetMenuItemModifiers (my nat.entry.handle, my nat.entry.item,
			( modifiers & _motif_OPTION_MASK ? kMenuOptionModifier : 0 ) +
			( modifiers & _motif_SHIFT_MASK ? kMenuShiftModifier : 0 ) +
			( modifiers & _motif_COMMAND_MASK ? 0 : kMenuNoCommandModifier ));
	#endif
}
#endif

#if gtk
	static void _guiGtkMenuItem_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiMenuItem);
		trace (U"destroying GuiMenuItem ", Melder_pointer (me));
		forget (me);
	}
	static void _guiGtkMenuItem_activateCallback (GuiObject widget, gpointer void_me) {
		iam (GuiMenuItem);
		if (my d_callbackBlocked) return;
		if (G_OBJECT_TYPE (widget) == GTK_TYPE_RADIO_MENU_ITEM && ! gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget))) return;
		struct structGuiMenuItemEvent event = { me, 0 };
		if (my d_commandCallback) {
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your choice of menu item \"", Melder_peek8to32 (GTK_WIDGET (widget) -> name), U"\" was not completely handled.");
			}
		}
	}
#elif cocoa
	@implementation GuiCocoaMenuItem {
		GuiMenuItem d_userData;
	}
	- (void) dealloc {   // override
		GuiMenuItem me = self -> d_userData;
		forget (me);
		trace (U"deleting a menu item");
		[super dealloc];
	}
	- (GuiThing) userData {
		return self -> d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiMenuItem));
		self -> d_userData = static_cast <GuiMenuItem> (userData);
	}
	- (void) _guiCocoaMenuItem_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiMenuItem me = self -> d_userData;
		if (my d_commandCallback != NULL) {
			struct structGuiMenuItemEvent event = { me, 0 };
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your choice of menu item \"", U"xx", U"\" was not completely handled.");
			}
		}
	}
	@end
#elif motif
	static void _guiMotifMenuItem_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiMenuItem);
		forget (me);
	}
	static void _guiMotifMenuItem_activateCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		iam (GuiMenuItem);
		if (my d_commandCallback != NULL) {
			struct structGuiMenuItemEvent event = { me, 0 };
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your choice of menu item \"", widget -> name, U"\" was not completely handled.");
			}
		}
	}
#endif

GuiMenuItem GuiMenu_addItem (GuiMenu menu, const char32 *title, uint32 flags,
	void (*commandCallback) (void *boss, GuiMenuItemEvent event), void *boss)
{
	GuiMenuItem me = Thing_new (GuiMenuItem);
	my d_shell = menu -> d_shell;
	my d_parent = menu;
	my d_menu = menu;

	trace (U"creating item \"", title, U"\" in menu ", Melder_pointer (menu));
	bool toggle = flags & (GuiMenu_CHECKBUTTON | GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON) ? true : false;
	uint32 accelerator = flags & 127;
	Melder_assert (title != NULL);
	#if gtk
		static GSList *group = NULL;
		if (toggle) {
			if (flags & (GuiMenu_RADIO_FIRST)) group = NULL;
			if (flags & (GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT)) {
				my d_widget = gtk_radio_menu_item_new_with_label (group, Melder_peek32to8 (title));
				group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (my d_widget));
				trace (U"created a radio menu item with title \"", title, U"\", group ", Melder_pointer (group));
			} else {
				my d_widget = gtk_check_menu_item_new_with_label (Melder_peek32to8 (title));
			}
		} else {
			my d_widget = gtk_menu_item_new_with_label (Melder_peek32to8 (title));
		}
		Melder_assert (menu -> d_widget);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu -> d_widget), GTK_WIDGET (my d_widget));
		_GuiObject_setUserData (my d_widget, me);
	#elif cocoa
		(void) toggle;   // no difference between toggling and normal menu items on Cocoa
		NSString *string = (NSString *) Melder_peek32toCfstring (title);
		GuiCocoaMenuItem *menuItem = [[GuiCocoaMenuItem alloc]
			initWithTitle:string
			action: NULL
			keyEquivalent: @""];
		//Melder_assert ([string retainCount] == 2 || [string retainCount] == -1);   // the menu item retains the string (assertion can fail on 10.6)
		trace (U"string retain count = ", [string retainCount]);
		my d_widget = menuItem;
		trace (
			U"installing item in GuiMenu ", Melder_pointer (menu),
			U" (NSMenu ", Melder_pointer (menu -> d_cocoaMenu),
			U"); retain count = ", [menuItem retainCount]
		);
		[menu -> d_cocoaMenu  addItem: (NSMenuItem *) my d_widget];   // the menu will retain the item...
		trace (
			U"installed item in GuiMenu ", Melder_pointer (menu),
			U" (NSMenu ", Melder_pointer (menu -> d_cocoaMenu),
			U"); retain count = ", [menuItem retainCount]
		);
		trace (U"release the item");
		[menuItem release];   // ... so we can release the item already
		trace (U"set user data");
		[menuItem setUserData: me];
	#elif motif
		my d_widget = XtVaCreateManagedWidget (Melder_peek32to8 (title),
			toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu -> d_widget, NULL);
		_GuiObject_setUserData (my d_widget, me);
	#endif
	Melder_assert (my d_widget != NULL);

	trace (U"set sensitivity");
	if (flags & GuiMenu_INSENSITIVE)
		GuiThing_setSensitive (me, false);

	trace (U"understand toggle menu items");
	if (flags & GuiMenu_TOGGLE_ON)
		#if gtk
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (my d_widget), TRUE);
		#elif cocoa
			[menuItem setState: NSOnState];
		#elif motif
			XmToggleButtonGadgetSetState (my d_widget, True, False);
		#endif

	if (accelerator) {
		trace (U"adding accelerator ", accelerator);
		/*
		 * For printable characters, the Command key is assumed.
		 */
		if (accelerator >= 32)
			flags |= GuiMenu_COMMAND;
		
		#if gtk
			static const guint acceleratorKeys [] = { 0,
				GDK_Left, GDK_Right, GDK_Up, GDK_Down, GDK_Pause, GDK_Delete, GDK_Insert, GDK_BackSpace,
				GDK_Tab, GDK_Return, GDK_Home, GDK_End, GDK_Return, GDK_Page_Up, GDK_Page_Down, GDK_Escape,
				GDK_F1, GDK_F2, GDK_F3, GDK_F4, GDK_F5, GDK_F6, GDK_F7, GDK_F8, GDK_F9, GDK_F10, GDK_F11, GDK_F12,
				0, 0, 0 };

			GdkModifierType modifiers = (GdkModifierType) 0;
			if (flags & GuiMenu_COMMAND) modifiers = (GdkModifierType) (modifiers | GDK_CONTROL_MASK);
			if (flags & GuiMenu_SHIFT)   modifiers = (GdkModifierType) (modifiers | GDK_SHIFT_MASK);
			if (flags & GuiMenu_OPTION)  modifiers = (GdkModifierType) (modifiers | GDK_MOD1_MASK);

			guint key;
			if (accelerator < 32) {
				key = acceleratorKeys [accelerator];
			} else {
				// gdk key symbols in the ASCII range are equal to ASCII
				key = accelerator;
			}

			GtkAccelGroup *ag = gtk_menu_get_accel_group (GTK_MENU (menu -> d_widget));

			if (key != 0)
				gtk_widget_add_accelerator (GTK_WIDGET (my d_widget), toggle ? "YouShouldNotGetHere" : "activate",
					ag, key, modifiers, GTK_ACCEL_VISIBLE);

		#elif cocoa
			accelerator = tolower (accelerator);   // otherwise, a Shift key is introduced in the mask
			NSUInteger mask = 0;
			if (flags & GuiMenu_COMMAND) mask |= NSCommandKeyMask;
			if (flags & GuiMenu_SHIFT)   mask |= NSShiftKeyMask;
			if (flags & GuiMenu_OPTION)  mask |= NSAlternateKeyMask;
			[menuItem setKeyEquivalentModifierMask: mask];
			if (accelerator > 0 && accelerator < 32) {
				static unichar acceleratorKeys [] = { 0,
					NSLeftArrowFunctionKey, NSRightArrowFunctionKey, NSUpArrowFunctionKey, NSDownArrowFunctionKey, NSPauseFunctionKey, NSDeleteFunctionKey, NSInsertFunctionKey, NSBackspaceCharacter,
					NSTabCharacter, NSNewlineCharacter, NSHomeFunctionKey, NSEndFunctionKey, NSCarriageReturnCharacter, NSPageUpFunctionKey, NSPageDownFunctionKey, 27,
					NSF1FunctionKey, NSF2FunctionKey, NSF3FunctionKey, NSF4FunctionKey, NSF5FunctionKey, NSF6FunctionKey,
					NSF7FunctionKey, NSF8FunctionKey, NSF9FunctionKey, NSF10FunctionKey, NSF11FunctionKey, NSF12FunctionKey,
					0, 0, 0 };
				[menuItem   setKeyEquivalent: [NSString   stringWithCharacters: & acceleratorKeys [accelerator]   length: 1]];
				if (accelerator == GuiMenu_TAB) {
					GuiWindow window = (GuiWindow) my d_shell;
					Melder_assert (window -> classInfo == classGuiWindow);   // fairly safe, because dialogs have no menus
					if (flags & GuiMenu_SHIFT) {
						window -> d_shiftTabCallback = commandCallback;
						window -> d_shiftTabBoss = boss;
					} else {
						window -> d_tabCallback = commandCallback;
						window -> d_tabBoss = boss;
					}
				} else if (accelerator == GuiMenu_BACKSPACE) {
					GuiWindow window = (GuiWindow) my d_shell;
					Melder_assert (window -> classInfo == classGuiWindow);   // fairly safe, because dialogs have no menus
					if (flags & GuiMenu_OPTION) {
						window -> d_optionBackspaceCallback = commandCallback;
						window -> d_optionBackspaceBoss = boss;
					}
				}
			} else {
				[menuItem setKeyEquivalent: [NSString stringWithFormat: @"%c", accelerator]];
			}
		#elif motif
			int modifiers = 0;
			if (flags & GuiMenu_COMMAND) modifiers |= _motif_COMMAND_MASK;
			if (flags & GuiMenu_SHIFT)   modifiers |= _motif_SHIFT_MASK;
			if (flags & GuiMenu_OPTION)  modifiers |= _motif_OPTION_MASK;
			if (accelerator > 0 && accelerator < 32) {
				if (my d_widget -> shell) {
					my d_widget -> shell -> motiff.shell.lowAccelerators [modifiers] |= 1 << accelerator;
				} else {
					theGuiTopLowAccelerators [modifiers] |= 1 << accelerator;
				}
			} else if (accelerator == '?' || accelerator == '{' || accelerator == '}' || accelerator == '\"' ||
				accelerator == '<' || accelerator == '>' || accelerator == '|' || accelerator == '_' || accelerator == '+' || accelerator == '~')
			{
				modifiers |= _motif_SHIFT_MASK;
			}
			my d_widget -> motiff.pushButton.acceleratorChar = accelerator;
			my d_widget -> motiff.pushButton.acceleratorModifiers = modifiers;
			NativeMenuItem_setText (my d_widget);
		#endif
		trace (U"added accelerator ", accelerator);
	}
	#if mac && useCarbon
		if (flags & GuiMenu_ATTRACTIVE) {
			trace (U"attractive!");
			SetItemStyle (my d_widget -> nat.entry.handle, my d_widget -> nat.entry.item, bold);
		}
	#endif

	trace (U"install the command callback");
	my d_commandCallback = commandCallback;
	my d_boss = boss;
	#if gtk
		if (commandCallback != NULL) {
			if (flags == GuiMenu_TAB) {
				GtkWidget *shell = gtk_widget_get_toplevel (gtk_menu_get_attach_widget (GTK_MENU (menu -> d_widget)));
				trace (U"tab set in GTK window ", Melder_pointer (shell));
				g_object_set_data (G_OBJECT (shell), "tabCallback", (gpointer) _guiGtkMenuItem_activateCallback);
				g_object_set_data (G_OBJECT (shell), "tabClosure", (gpointer) me);
			} else if (flags == (GuiMenu_TAB | GuiMenu_SHIFT)) {
				GtkWidget *shell = gtk_widget_get_toplevel (gtk_menu_get_attach_widget (GTK_MENU (menu -> d_widget)));
				trace (U"shift-tab set in GTK window ", Melder_pointer (shell));
				g_object_set_data (G_OBJECT (shell), "shiftTabCallback", (gpointer) _guiGtkMenuItem_activateCallback);
				g_object_set_data (G_OBJECT (shell), "shiftTabClosure", (gpointer) me);
			} else {
				g_signal_connect (G_OBJECT (my d_widget),
					toggle ? "toggled" : "activate",
					G_CALLBACK (_guiGtkMenuItem_activateCallback), (gpointer) me);
			}
		} else {
			gtk_widget_set_sensitive (GTK_WIDGET (my d_widget), FALSE);
		}
		gtk_widget_show (GTK_WIDGET (my d_widget));
	#elif cocoa
		[(NSMenuItem *) my d_widget setTarget: (id) my d_widget];
		[(NSMenuItem *) my d_widget setAction: @selector (_guiCocoaMenuItem_activateCallback:)];
	#elif motif
		XtAddCallback (my d_widget,
			toggle ? XmNvalueChangedCallback : XmNactivateCallback,
			_guiMotifMenuItem_activateCallback, (XtPointer) me);
	#endif

	trace (U"make sure that I will be destroyed when my widget is destroyed");
	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenuItem_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenuItem_destroyCallback, me);
	#endif

	return me;
}

GuiMenuItem GuiMenu_addSeparator (GuiMenu menu) {
	GuiMenuItem me = Thing_new (GuiMenuItem);
	my d_shell = menu -> d_shell;
	my d_parent = menu;
	my d_menu = menu;
	#if gtk
		my d_widget = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu -> d_widget), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (my d_widget));
	#elif cocoa
		my d_widget = (GuiObject) [GuiCocoaMenuItem separatorItem];
		trace (U"install separator in menu ", Melder_pointer (menu));
		trace (
			U"installing separator in GuiMenu ", Melder_pointer (menu),
			U" (NSMenu ", Melder_pointer (menu -> d_cocoaMenu),
			U"); retain count = ", [((NSMenuItem *) my d_widget) retainCount]
		);
		[menu -> d_cocoaMenu  addItem: (NSMenuItem *) my d_widget];   // the menu will retain the item...
		trace (
			U"installed separator in GuiMenu ", Melder_pointer (menu),
			U" (NSMenu ", Melder_pointer (menu -> d_cocoaMenu),
			U"); retain count = ", [((NSMenuItem *) my d_widget) retainCount]
		);
		trace (U"release the item");
		//[(NSMenuItem *) my d_widget release];   // ... so we can release the item already
		trace (U"set user data");
		[(GuiCocoaMenuItem *) my d_widget setUserData: me];
	#elif motif
		my d_widget = XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu -> d_widget, NULL);
	#endif

	trace (U"make sure that I will be destroyed when my widget is destroyed");
	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenuItem_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenuItem_destroyCallback, me);
	#endif
	return me;
}

void GuiMenuItem_check (GuiMenuItem me, bool check) {
	Melder_assert (my d_widget != NULL);
	#if gtk
		my d_callbackBlocked = true;
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (my d_widget), check);
		my d_callbackBlocked = false;
	#elif cocoa
		GuiCocoaMenuItem *item = (GuiCocoaMenuItem *) my d_widget;
		[item   setState: check];
	#elif motif
		XmToggleButtonGadgetSetState (my d_widget, check, False);
	#endif
}

/* End of file GuiMenuItem.cpp */
