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

#ifndef __ARCHIVE_BUTTON_PREFS_H__
#define __ARCHIVE_BUTTON_PREFS_H__

typedef struct ArchiveButtonConfig ArchiveButtonConfig;
typedef struct ArchiveButtonAccountConfig ArchiveButtonAccountConfig;

#include "prefs_account.h"

struct ArchiveButtonConfig
{
	gint		manager_win_width;
	gint		manager_win_height;
};

struct ArchiveButtonAccountConfig
{
    gboolean    enable;
	gchar 		*archive_folder;
    gboolean    mark_read_on_archive;
};

extern ArchiveButtonConfig archive_button_config;

void archive_button_prefs_init(void);
void archive_button_prefs_done(void);
struct ArchiveButtonAccountConfig *archive_button_prefs_account_get_config(
		PrefsAccount *account);
void archive_button_prefs_account_set_config(
		PrefsAccount *account, ArchiveButtonAccountConfig *config);
void archive_button_prefs_account_free_config(ArchiveButtonAccountConfig *config);

#endif /* __ARCHIVE_BUTTON_PREFS_H__ */
