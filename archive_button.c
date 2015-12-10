/*
 * Claws Mail -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2015 the Claws Mail Team
 * Copyright (C) 2015 Dominik Meyer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#include "claws-features.h"
#endif

#include <glib.h>
#include <glib/gi18n.h>

#include "defs.h"

#include "version.h"
#include "claws.h"
#include "plugin.h"
#include "utils.h"
#include "folder.h"
#include "account.h"
#include "alertpanel.h"

#include "main.h"
#include "mainwindow.h"
#include "summaryview.h"
#include "messageview.h"
#include "prefs_toolbar.h"

#include "archive_button.h"
#include "archive_button_prefs.h"

#define PLUGIN_NAME (_("ArchiveButton"))

static void toolbar_archive_button_mainwindow_cb (gpointer parent,
						  const gchar *item_name,
				 		  gpointer        data);

static void toolbar_archive_button_messagewindow_cb (gpointer parent,
						  const gchar *item_name,
				 		  gpointer        data);

static gboolean check_enabled(ArchiveButtonAccountConfig *ac_config) {
	if(!ac_config->enable) {
		alertpanel_error(_("Archive not enabled for this account."));
		return FALSE;
	}

	return TRUE;
}

static void move_message_to_archive_summaryview(SummaryView *summaryview) 
{
	FolderItem *dest_folder = NULL;
	PrefsAccount *ac = NULL;
	ArchiveButtonAccountConfig *ac_config = NULL;

	if(!summaryview->folder_item || 
	   FOLDER_TYPE(summaryview->folder_item->folder) == F_NEWS)
		return;
	
	if(NULL != (ac = account_find_from_item(summaryview->folder_item))) {
		if(NULL != (ac_config = archive_button_prefs_account_get_config(ac))) {
			if(!check_enabled(ac_config)) {
				return;
			}
			dest_folder = folder_find_item_from_identifier(ac_config->archive_folder); 
		} else {
			alertpanel_error(_("ArchiveButton: move: could not find archive folder from config."));
			return;
		}
     	} else {
		alertpanel_error(_("ArchiveButton: move: could not retrieve account for message."));
		return;
	}
	
	if(!dest_folder) {
		alertpanel_error(_("ArchiveButton: move: archive folder not found."));
		return;
	}

	summary_move_selected_to(summaryview, dest_folder);
	// TODO; mark read if enabled in preferences
}

static void move_message_to_archive_messageview(MessageView *msgview)
{
	MsgInfo *msginfo = NULL;
	FolderItem *dest_folder = NULL;
	PrefsAccount *ac = NULL;
	ArchiveButtonAccountConfig *ac_config = NULL;

	if (msgview->msginfo && msgview->mainwin && msgview->mainwin->summaryview)
		msginfo = summary_get_selected_msg(msgview->mainwin->summaryview);

	/* need a procmsg_msginfo_equal() */
	if (msginfo && msgview->msginfo &&
			msginfo->msgnum == msgview->msginfo->msgnum &&
			msginfo->folder == msgview->msginfo->folder) {
		move_message_to_archive_summaryview(msgview->mainwin->summaryview);
	} else {
		msginfo = msgview->msginfo;

		cm_return_if_fail(msginfo != NULL);

		if (msginfo->folder) {
			if (NULL != (ac = account_find_from_item(msginfo->folder))) {
				if(NULL != (ac_config = archive_button_prefs_account_get_config(ac))) {
					if(!check_enabled(ac_config)) {
						return;
					}
					dest_folder = folder_find_item_from_identifier(ac_config->archive_folder);
				} else {
					alertpanel_error(_("ArchiveButton: move: could not find archive folder from config."));
					return;
				}
			} else {                
				alertpanel_error(_("ArchiveButton: move: could not retrieve account for message."));
				return;
			}
		}

		if(!dest_folder) {
			alertpanel_error(_("ArchiveButton: move: archive folder not found."));
			return;
		}

		if(folder_item_move_msg(dest_folder, msginfo) == -1) {
			alertpanel_error(_("ArchiveButton: move: could not move message."));
			return;
		}

		// TODO: mark read if enabled in preferences
	}

#ifdef GENERIC_UMPC
	if (msgview->window && !prefs_common.always_show_msg) {
		messageview_destroy(msgview);
	}
#endif
}

static void toolbar_archive_button_mainwindow_cb(gpointer parent,
		const gchar *item_name, gpointer data)
{
	debug_print("ArchiveButton: button mainwindow pressed\n");
	MainWindow *mainwindow = (MainWindow *)parent;
	move_message_to_archive_summaryview(mainwindow->summaryview);
}

static void toolbar_archive_button_messagewindow_cb(gpointer parent,
		const gchar *item_name, gpointer data)
{
	debug_print("ArchiveButton: button messagewindow pressed\n");
	MessageView *messageview = (MessageView *)parent;
	move_message_to_archive_messageview(messageview);
}

gint plugin_init(gchar **error)
{
	if (!check_plugin_version(MAKE_NUMERIC_VERSION(2,9,2,72),
				VERSION_NUMERIC, PLUGIN_NAME, error))
		return -1;

	debug_print("registering toolbar item\n");
	
	prefs_toolbar_register_plugin_item(TOOLBAR_MAIN, PLUGIN_NAME, "Archive",
			toolbar_archive_button_mainwindow_cb, NULL);
	prefs_toolbar_register_plugin_item(TOOLBAR_MSGVIEW, PLUGIN_NAME, "Archive",
			toolbar_archive_button_messagewindow_cb, NULL);


	archive_button_prefs_init();

	debug_print("ArchiveButton plugin loaded\n");

	return 0;
}

gboolean plugin_done(void)
{
	archive_button_prefs_done();

	debug_print("ArchiveButton plugin unloaded\n");
	return TRUE;
}

const gchar *plugin_name(void)
{
	return PLUGIN_NAME;
}

const gchar *plugin_desc(void)
{
	return _("This Plugin adds an archive button to Claws Mail."
	         "It allows moving selected mails to a predefined archive folder"
		 "for each account.");
}

const gchar *plugin_type(void)
{
	return "Common";
}

const gchar *plugin_licence(void)
{
	return "GPL3+";
}

const gchar *plugin_version(void)
{
	return VERSION;
}

struct PluginFeature *plugin_provides(void)
{
	static struct PluginFeature features[] = 
		{ {PLUGIN_UTILITY, N_("ArchiveButton")},
		  {PLUGIN_NOTHING, NULL}};
	return features;
}
