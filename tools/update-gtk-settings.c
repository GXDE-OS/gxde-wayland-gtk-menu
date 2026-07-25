/*
 * Copyright (C) 2026 CharOfString <root@charofstring.cc>
 *
 * This file is part of gxde-daemon.
 *
 * gxde-daemon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gxde-daemon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gxde-daemon.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>

#define SETTINGS_GROUP "Settings"
#define MODULES_KEY "gtk-modules"
#define MODULE_NAME "appmenu-gtk-module-wayland"
#define DEFAULT_SETTINGS_FILE "/etc/gtk-3.0/settings.ini"

static gchar* BuildModuleVal(const gchar* current, gboolean enable,
    gboolean* changed) {
  GPtrArray* modules = g_ptr_array_new_with_free_func(g_free);
  gchar** items =
    g_strsplit(current != NULL ? current : "", G_SEARCHPATH_SEPARATOR_S, -1);
  gboolean module_found = FALSE;

  for (gsize i = 0; items[i] != NULL; i++) {
    gchar* item = g_strstrip(items[i]);

    if (*item == '\0') continue;

    if (g_str_equal(item, MODULE_NAME)) {
      if (module_found || !enable) {
        *changed = TRUE;
        continue;
      }

      module_found = TRUE;
    }

    g_ptr_array_add(modules, g_strdup(item));
  }

  if (enable && !module_found) {
    g_ptr_array_add(modules, g_strdup(MODULE_NAME));
    *changed = TRUE;
  }

  g_strfreev(items);

  GString* value = g_string_new(NULL);
  for (guint i = 0; i < modules->len; i++) {
    if (i > 0) g_string_append_c(value, G_SEARCHPATH_SEPARATOR);
    g_string_append(value, g_ptr_array_index(modules, i));
  }

  g_ptr_array_free(modules, TRUE);
  return g_string_free(value, FALSE);
}

static gboolean update_settings(const gchar* path, gboolean enable,
    GError** error) {
  GKeyFile* key_file = g_key_file_new();
  gboolean file_exists = g_file_test(path, G_FILE_TEST_EXISTS);

  if (file_exists && !g_key_file_load_from_file( key_file, path,
        G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, error)) {
    g_key_file_free(key_file);
    return FALSE;
  }

  GError* value_error = NULL;
  gchar* current = g_key_file_get_string(key_file, SETTINGS_GROUP, MODULES_KEY,
    &value_error);

  if (value_error != NULL) {
    if (!g_error_matches(value_error, G_KEY_FILE_ERROR,
          G_KEY_FILE_ERROR_GROUP_NOT_FOUND) &&
        !g_error_matches(value_error, G_KEY_FILE_ERROR,
          G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
      g_propagate_error(error, value_error);
      g_key_file_free(key_file);
      return FALSE;
    }

    g_clear_error(&value_error);
  }

  if (!enable && current == NULL) {
    g_key_file_free(key_file);
    return TRUE;
  }

  gboolean changed = FALSE;
  gchar* updated = BuildModuleVal(current, enable, &changed);

  if (!changed) {
    g_free(updated);
    g_free(current);
    g_key_file_free(key_file);
    return TRUE;
  }

  if (*updated != '\0')
    g_key_file_set_string(key_file, SETTINGS_GROUP, MODULES_KEY, updated);
  else
    g_key_file_remove_key(key_file, SETTINGS_GROUP, MODULES_KEY, NULL);

  gchar* directory = g_path_get_dirname(path);
  if (g_mkdir_with_parents(directory, 0755) != 0) {
    g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
      "cannot create %s: %s", directory, g_strerror(errno));
    g_free(directory);
    g_free(updated);
    g_free(current);
    g_key_file_free(key_file);
    return FALSE;
  }
  g_free(directory);

  gsize length = 0;
  gchar* contents = g_key_file_to_data(key_file, &length, error);
  gboolean success =
    contents != NULL && g_file_set_contents(path, contents, length, error);

  if (success && !file_exists) g_chmod(path, 0644);

  g_free(contents);
  g_free(updated);
  g_free(current);
  g_key_file_free(key_file);
  return success;
}

int main(int argc, char** argv) {
  if (argc < 2 || argc > 3 ||
      (!g_str_equal(argv[1], "enable") && !g_str_equal(argv[1], "disable"))) {
    g_printerr("Usage: %s enable|disable [settings.ini]\n", argv[0]);
    return 2;
  }

  const gchar* path = argc == 3 ? argv[2] : DEFAULT_SETTINGS_FILE;
  GError* error = NULL;

  if (!update_settings(path, g_str_equal(argv[1], "enable"), &error)) {
    g_printerr("%s: %s\n", path, error->message);
    g_error_free(error);
    return 1;
  }

  return 0;
}
