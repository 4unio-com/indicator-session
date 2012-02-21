/*
Copyright 2012 Canonical Ltd.

Authors:
    Alberto Mardegan <alberto.mardegan@canonical.com>

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranties of
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib/gi18n.h>

#include "webcredentials-mgr.h"

#include <libdbusmenu-glib/client.h>

struct _WebcredentialsMgr
{
  WebcredentialsSkeleton parent_instance;
  GDBusConnection *connection;
  DbusmenuMenuitem *menu_item;
  GHashTable *failed_accounts;
  guint bus_name_id;
};

#define WEBCREDENTIALS_OBJECT_PATH "/com/canonical/indicators/webcredentials"
#define WEBCREDENTIALS_BUS_NAME "com.canonical.indicators.webcredentials"

G_DEFINE_TYPE (WebcredentialsMgr, webcredentials_mgr,
               TYPE_WEBCREDENTIALS_SKELETON);

static void
update_failed_accounts_property (WebcredentialsMgr *self)
{
  GHashTableIter iter;
  GArray *account_ids;
  GVariant *value;
  gpointer key;

  account_ids = g_array_sized_new (FALSE, FALSE,
                                   sizeof (guint32),
                                   g_hash_table_size (self->failed_accounts));

  g_hash_table_iter_init (&iter, self->failed_accounts);
  while (g_hash_table_iter_next (&iter, &key, NULL)) {
    g_array_append_val (account_ids, key);
  }

  value = g_variant_new_fixed_array (G_VARIANT_TYPE_UINT32,
                                     account_ids->data,
                                     account_ids->len,
                                     sizeof (guint32));
  g_object_set ((GObject *)self,
                "failures", value,
                NULL);
  g_array_free (account_ids, TRUE);
}

static gboolean
webcredentials_mgr_clear_error_status (WebcredentialsMgr *self,
                                       GDBusMethodInvocation *invocation)
{
  g_return_val_if_fail (WEBCREDENTIALS_IS_MGR (self), FALSE);

  /* TODO */

  webcredentials_complete_clear_error_status ((Webcredentials *)self,
                                              invocation);
  return TRUE;
}

static gboolean
webcredentials_mgr_remove_failures (WebcredentialsMgr *self,
                                    GDBusMethodInvocation *invocation,
                                    GVariant *arg_account_ids)
{
  gsize n_accounts = 0;
  const guint32 *account_ids;
  gsize i;
  gboolean changed = FALSE;

  g_return_val_if_fail (WEBCREDENTIALS_IS_MGR (self), FALSE);

  account_ids = g_variant_get_fixed_array (arg_account_ids,
                                           &n_accounts,
                                           sizeof (guint32));
  for (i = 0; i < n_accounts; i++) {
    if (g_hash_table_remove (self->failed_accounts,
                             GINT_TO_POINTER (account_ids[i])))
      changed = TRUE;
  }

  if (changed)
    update_failed_accounts_property (self);

  webcredentials_complete_remove_failures ((Webcredentials *)self,
                                           invocation);
  return TRUE;
}

static gboolean
webcredentials_mgr_report_failure (WebcredentialsMgr *self,
                                   GDBusMethodInvocation *invocation,
                                   guint account_id,
                                   GVariant *arg_notification)
{
  g_return_val_if_fail (WEBCREDENTIALS_IS_MGR (self), FALSE);

  g_hash_table_insert (self->failed_accounts,
                       GINT_TO_POINTER(account_id),
                       GINT_TO_POINTER(TRUE));
  update_failed_accounts_property (self);

  /* TODO: emit the notification and set the error status */

  webcredentials_complete_report_failure ((Webcredentials *)self,
                                          invocation);
  return TRUE;
}

static void
on_menu_item_activated (DbusmenuMenuitem *menu_item,
                        guint timestamp,
                        WebcredentialsMgr *self)
{
  GError *error = NULL;

  if (!g_spawn_command_line_async("gnome-control-center credentials", &error))
  {
    g_warning("Unable to show control centre: %s", error->message);
    g_error_free(error);
  }
}

static void
webcredentials_mgr_init (WebcredentialsMgr *self)
{
  GError *error = NULL;

  self->menu_item = dbusmenu_menuitem_new ();
  dbusmenu_menuitem_property_set (self->menu_item,
                                  DBUSMENU_MENUITEM_PROP_TYPE,
                                  DBUSMENU_CLIENT_TYPES_DEFAULT);
  dbusmenu_menuitem_property_set (self->menu_item,
                                  DBUSMENU_MENUITEM_PROP_LABEL,
                                  _("Web Accounts…"));
  g_signal_connect (self->menu_item,
                    DBUSMENU_MENUITEM_SIGNAL_ITEM_ACTIVATED,
                    G_CALLBACK (on_menu_item_activated),
                    self);

  self->failed_accounts = g_hash_table_new (g_direct_hash,
                                            g_direct_equal);
  self->connection = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);
  if (G_UNLIKELY (error != NULL)) {
    g_warning ("Couldn't connect to session bus: %s", error->message);
    g_clear_error (&error);
    return;
  }

  g_signal_connect (self, "handle-clear-error-status",
                    G_CALLBACK (webcredentials_mgr_clear_error_status), NULL);
  g_signal_connect (self, "handle-remove-failures",
                    G_CALLBACK (webcredentials_mgr_remove_failures), NULL);
  g_signal_connect (self, "handle-report-failure",
                    G_CALLBACK (webcredentials_mgr_report_failure), NULL);

  if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (self),
                                         self->connection,
                                         WEBCREDENTIALS_OBJECT_PATH,
                                         &error)) {
    g_return_if_fail (error != NULL);
    g_warning ("Couldn't register webcredentials service: %s", error->message);
    g_clear_error (&error);
  }

  self->bus_name_id =
    g_bus_own_name_on_connection (self->connection,
                                  WEBCREDENTIALS_BUS_NAME,
                                  G_BUS_NAME_OWNER_FLAGS_REPLACE,
                                  NULL, NULL, NULL, NULL);
}

static void
webcredentials_mgr_dispose (GObject *object)
{
  WebcredentialsMgr *self = WEBCREDENTIALS_MGR (object);

  if (self->menu_item != NULL) {
    g_object_unref (self->menu_item);
    self->menu_item = NULL;
  }

  if (self->bus_name_id != 0) {
    g_bus_unown_name (self->bus_name_id);
    self->bus_name_id = 0;
  }

  if (self->connection != NULL) {
    g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (self));
    g_object_unref (self->connection);
    self->connection = NULL;
  }

  G_OBJECT_CLASS (webcredentials_mgr_parent_class)->dispose (object);
}

static void
webcredentials_mgr_finalize (GObject *object)
{
  WebcredentialsMgr *self = WEBCREDENTIALS_MGR (object);

  g_hash_table_destroy (self->failed_accounts);

  G_OBJECT_CLASS (webcredentials_mgr_parent_class)->finalize (object);
}

static void
webcredentials_mgr_class_init (WebcredentialsMgrClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = webcredentials_mgr_dispose;
  object_class->finalize = webcredentials_mgr_finalize;
}

WebcredentialsMgr *webcredentials_mgr_new ()
{
  return g_object_new (WEBCREDENTIALS_TYPE_MGR, NULL);
}

DbusmenuMenuitem *webcredentials_mgr_get_menu_item (WebcredentialsMgr *self)
{
  g_return_val_if_fail (WEBCREDENTIALS_IS_MGR (self), NULL);
  return self->menu_item;
}
