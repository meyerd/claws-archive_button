/*
 * Claws Mail -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 2004-2015 the Claws Mail team
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
#include "config.h"
#include "claws-features.h"
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "defs.h"
#include "gtk/gtkutils.h"
#include "gtk/combobox.h"
#include "alertpanel.h"
#include "passcrypt.h"
#include "utils.h"
#include "prefs.h"
#include "prefs_gtk.h"
#include "folder.h"
#include "foldersel.h"

#include "archive_button_prefs.h"

#define PREFS_BLOCK_NAME "ArchiveButton"

ArchiveButtonConfig archive_button_config;

static PrefParam prefs[] = {
        {"manager_win_width", "-1", &archive_button_config.manager_win_width,
		P_INT, NULL, NULL, NULL},
        {"manager_win_height", "-1", &archive_button_config.manager_win_height,
		P_INT, NULL, NULL, NULL},
        {0,0,0,0}
};

#define PACK_HBOX(hbox, vbox) \
{ \
	hbox = gtk_hbox_new (FALSE, 5); \
	gtk_widget_show (hbox); \
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0); \
}

#define RADIO_ADD(radio, group, hbox, vbox, label) \
{ \
	PACK_HBOX(hbox, vbox); \
	gtk_container_set_border_width(GTK_CONTAINER (hbox), 0); \
	radio = gtk_radio_button_new_with_label(group, label); \
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio)); \
	gtk_widget_show(radio); \
	gtk_box_pack_start(GTK_BOX(hbox), radio, FALSE, FALSE, 0); \
}

struct ArchiveButtonAccountPage
{
	PrefsPage page;

	GtkWidget *enable_checkbtn;
	GtkWidget *archive_folder_frame;
	GtkWidget *archive_folder_entry;
	GtkWidget *mark_read_on_archive_checkbtn;

	PrefsAccount *account;
};

static struct ArchiveButtonAccountPage account_page;

static void archive_button_prefs_select_folder_cb(GtkWidget *widget, gpointer data)
{
	FolderItem *item;
	gchar *id;

	item = foldersel_folder_sel(NULL, FOLDER_SEL_COPY, NULL, FALSE);
	if (item && item->path) {
		id = folder_item_get_identifier(item);
		if (id) {
			gtk_entry_set_text(GTK_ENTRY(data), id);
			g_free(id);
		}
	}
}

static void archive_button_prefs_account_create_widget_func(PrefsPage *_page,
						 GtkWindow *window,
						 gpointer data)
{
	struct ArchiveButtonAccountPage *page = (struct ArchiveButtonAccountPage *) _page;
	PrefsAccount *account = (PrefsAccount *) data;
	ArchiveButtonAccountConfig *config;

	GtkWidget *page_vbox, *archive_button_vbox;
	GtkWidget *archive_folder_vbox, *archive_folder_frame;
	GtkWidget *hbox;

	GtkWidget *enable_checkbtn;

	GtkWidget *archive_folder_entry;
	GtkWidget *button;
	GtkWidget *mark_read_on_archive_checkbtn;

	page_vbox = gtk_vbox_new (FALSE, VSPACING);
	gtk_widget_show (page_vbox);
	gtk_container_set_border_width (GTK_CONTAINER (page_vbox), VBOX_BORDER);

	/* Enable/disable */
	PACK_CHECK_BUTTON(page_vbox, enable_checkbtn, _("Archive for this account"));


	archive_button_vbox = gtk_vbox_new (FALSE, VSPACING);
	gtk_widget_show (archive_button_vbox);
	gtk_box_pack_start (GTK_BOX (page_vbox), archive_button_vbox, FALSE, FALSE, 0);

	SET_TOGGLE_SENSITIVITY(enable_checkbtn, archive_button_vbox);
	
	archive_folder_vbox = gtkut_get_options_frame(archive_button_vbox, &archive_folder_frame,
			_("Put archived messages in"));
	gtk_widget_show (archive_folder_vbox);
	gtk_box_pack_start (GTK_BOX (page_vbox), archive_folder_vbox, FALSE, FALSE, 0);
	
	PACK_HBOX (hbox, archive_folder_vbox);
	archive_folder_entry = gtk_entry_new();
	gtk_widget_show (archive_folder_entry);
	// gtk_entry_set_max_length(GTK_ENTRY(archive_folder_entry), 1024);
	gtk_box_pack_start (GTK_BOX (hbox), archive_folder_entry, TRUE, TRUE, 0);
	CLAWS_SET_TIP(hbox,
		_("The folder where the archived mail will be moved to."));

	button = gtkut_get_browse_file_btn(_("Browse"));
	gtk_widget_show (button);
	gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT (button), "clicked", 
			G_CALLBACK (archive_button_prefs_select_folder_cb),
			archive_folder_entry);

	PACK_CHECK_BUTTON(archive_button_vbox, mark_read_on_archive_checkbtn, _("Mark read on archive."));

	// TODO: add little text with examples on how to configure

	config = archive_button_prefs_account_get_config(account);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enable_checkbtn), config->enable);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mark_read_on_archive_checkbtn),
			config->mark_read_on_archive);

	if (config->archive_folder != NULL)
		gtk_entry_set_text(GTK_ENTRY(archive_folder_entry), config->archive_folder);

	/* Add items to page struct */
	page->account = account;
	page->enable_checkbtn = enable_checkbtn;
	page->archive_folder_entry = archive_folder_entry;
	page->archive_folder_frame = archive_folder_frame;
	page->mark_read_on_archive_checkbtn = mark_read_on_archive_checkbtn;
	page->page.widget = page_vbox;

	/* Update things */

	/* Free things */
}

static void archive_button_prefs_account_destroy_widget_func(PrefsPage *_page)
{
}

static gint archive_button_prefs_account_apply(struct ArchiveButtonAccountPage *page)
{
	ArchiveButtonAccountConfig *config;

	config = archive_button_prefs_account_get_config(page->account);
	
	config->enable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(page->enable_checkbtn));
	config->archive_folder = gtk_editable_get_chars(GTK_EDITABLE(page->archive_folder_entry), 0, -1);
	config->mark_read_on_archive = gtk_toggle_button_get_active(
			GTK_TOGGLE_BUTTON(page->mark_read_on_archive_checkbtn));

	archive_button_prefs_account_set_config(page->account, config);
	archive_button_prefs_account_free_config(config);
	return TRUE;
}

static gboolean archive_button_prefs_account_check(struct ArchiveButtonAccountPage *page)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(page->enable_checkbtn)) &&
		(*gtk_entry_get_text(GTK_ENTRY(page->archive_folder_entry)) == '\0')) {
		alertpanel_error(_("Archive folder has to be specified."));
		return FALSE;
	}

	return TRUE;
}

void archive_button_prefs_account_save_func(PrefsPage *_page)
{
	struct ArchiveButtonAccountPage *page = (struct ArchiveButtonAccountPage *) _page;
	if (archive_button_prefs_account_check(page)) {
		archive_button_prefs_account_apply(page);
	}
}

gboolean archive_button_prefs_account_can_close(PrefsPage *_page)
{
	struct ArchiveButtonAccountPage *page = (struct ArchiveButtonAccountPage *) _page;
	return archive_button_prefs_account_check(page);
}

void archive_button_prefs_init()
{
	gchar *rcpath;

	/* Account prefs */
	static gchar *path[3];
	path[0] = _("Plugins");
	path[1] = _("ArchiveButton");
	path[2] = NULL;

	account_page.page.path = path;
	account_page.page.create_widget = archive_button_prefs_account_create_widget_func;
	account_page.page.destroy_widget = archive_button_prefs_account_destroy_widget_func;
	account_page.page.save_page = archive_button_prefs_account_save_func;
	account_page.page.can_close = archive_button_prefs_account_can_close;
	account_page.page.weight = 30.0;
	prefs_account_register_page((PrefsPage *) &account_page);

	/* Common prefs */
	prefs_set_default(prefs);
	rcpath = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S, COMMON_RC, NULL);
	prefs_read_config(prefs, PREFS_BLOCK_NAME, rcpath, NULL);
	g_free(rcpath);
}

void archive_button_prefs_done(void)
{
	PrefFile *pref_file;
	gchar *rc_file_path;

	prefs_account_unregister_page((PrefsPage *) &account_page);

	rc_file_path = g_strconcat(get_rc_dir(), G_DIR_SEPARATOR_S,
				   COMMON_RC, NULL);
	pref_file = prefs_write_open(rc_file_path);
	g_free(rc_file_path);

	if (!pref_file || prefs_set_block_label(pref_file, PREFS_BLOCK_NAME) < 0)
		return;

	if (prefs_write_param(prefs, pref_file->fp) < 0) {
		g_warning("failed to write ArchiveButton Plugin configuration");
		prefs_file_close_revert(pref_file);
		return;
	}

	if (fprintf(pref_file->fp, "\n") < 0) {
		FILE_OP_ERROR(rc_file_path, "fprintf");
		prefs_file_close_revert(pref_file);
	} else
		prefs_file_close(pref_file);
}

struct ArchiveButtonAccountConfig *archive_button_prefs_account_get_config(
		PrefsAccount *account)
{
	ArchiveButtonAccountConfig *config;
	const gchar *confstr;
	gchar enable, mark_read_on_archive;
	gchar tmparchivefolder[512];

	config = g_new0(ArchiveButtonAccountConfig, 1);

	config->archive_folder = NULL;
	config->enable = FALSE;
	config->mark_read_on_archive = FALSE;

	confstr = prefs_account_get_privacy_prefs(account, "archive_button");
	if (confstr == NULL)
		return config;

	sscanf(confstr, "%c%c%511[^\t]",
			&enable,
			&mark_read_on_archive,
			tmparchivefolder
			);

	config->archive_folder = g_strndup(tmparchivefolder, 511);
	config->enable = enable == 'y';
	config->mark_read_on_archive = mark_read_on_archive == 'y';

	if (config->archive_folder[0] == '!' && !config->archive_folder[1]) {
		g_free(config->archive_folder);
		config->archive_folder = NULL;
	}

	return config;
}

void archive_button_prefs_account_set_config(
		PrefsAccount *account, ArchiveButtonAccountConfig *config)
{
	gchar *confstr = NULL;

	confstr = g_strdup_printf("%c%c%s\t",
			config->enable ? 'y' : 'n',
			config->mark_read_on_archive ? 'y' : 'n',
			config->archive_folder && config->archive_folder[0] ? config->archive_folder : "!"
			);

	prefs_account_set_privacy_prefs(account, "archive_button", confstr);

	g_free(confstr);

	//archive_button_account_prefs_updated(account);
}

void archive_button_prefs_account_free_config(ArchiveButtonAccountConfig *config)
{
	g_free(config->archive_folder);
	g_free(config);
}


