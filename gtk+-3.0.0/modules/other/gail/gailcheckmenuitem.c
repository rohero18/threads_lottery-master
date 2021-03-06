/* GAIL - The GNOME Accessibility Implementation Library
 * Copyright 2002, 2003 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h>
#include "gailcheckmenuitem.h"
#include "gailchecksubmenuitem.h"

static void      gail_check_menu_item_class_init        (GailCheckMenuItemClass *klass);

static void      gail_check_menu_item_init              (GailCheckMenuItem      *item);

static void      gail_check_menu_item_toggled_gtk       (GtkWidget              *widget);

static void      gail_check_menu_item_real_notify_gtk   (GObject                *obj,
                                                         GParamSpec             *pspec);

static void      gail_check_menu_item_real_initialize   (AtkObject              *obj,
                                                         gpointer               data);

static AtkStateSet* gail_check_menu_item_ref_state_set  (AtkObject              *accessible);

G_DEFINE_TYPE (GailCheckMenuItem, gail_check_menu_item, GAIL_TYPE_MENU_ITEM)

static void
gail_check_menu_item_class_init (GailCheckMenuItemClass *klass)
{
  GailWidgetClass *widget_class;
  AtkObjectClass *class = ATK_OBJECT_CLASS (klass);

  widget_class = (GailWidgetClass*)klass;
  widget_class->notify_gtk = gail_check_menu_item_real_notify_gtk;

  class->ref_state_set = gail_check_menu_item_ref_state_set;
  class->initialize = gail_check_menu_item_real_initialize;
}

static void
gail_check_menu_item_init (GailCheckMenuItem *item)
{
}

AtkObject* 
gail_check_menu_item_new (GtkWidget *widget)
{
  GObject *object;
  AtkObject *accessible;

  g_return_val_if_fail (GTK_IS_CHECK_MENU_ITEM (widget), NULL);

  if (gtk_menu_item_get_submenu (GTK_MENU_ITEM (widget)))
    return gail_check_sub_menu_item_new (widget);

  object = g_object_new (GAIL_TYPE_CHECK_MENU_ITEM, NULL);

  accessible = ATK_OBJECT (object);
  atk_object_initialize (accessible, widget);
 
  return accessible;
}

static void
gail_check_menu_item_real_initialize (AtkObject *obj,
                                      gpointer  data)
{
  ATK_OBJECT_CLASS (gail_check_menu_item_parent_class)->initialize (obj, data);

  g_signal_connect (data,
                    "toggled",
                    G_CALLBACK (gail_check_menu_item_toggled_gtk),
                    NULL);

  obj->role = ATK_ROLE_CHECK_MENU_ITEM;
}

static void
gail_check_menu_item_toggled_gtk (GtkWidget       *widget)
{
  AtkObject *accessible;
  GtkCheckMenuItem *check_menu_item;

  check_menu_item = GTK_CHECK_MENU_ITEM (widget);

  accessible = gtk_widget_get_accessible (widget);
  atk_object_notify_state_change (accessible, ATK_STATE_CHECKED, 
                                  gtk_check_menu_item_get_active (check_menu_item));
} 

static AtkStateSet*
gail_check_menu_item_ref_state_set (AtkObject *accessible)
{
  AtkStateSet *state_set;
  GtkCheckMenuItem *check_menu_item;
  GtkWidget *widget;

  state_set = ATK_OBJECT_CLASS (gail_check_menu_item_parent_class)->ref_state_set (accessible);
  widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible));
 
  if (widget == NULL)
    return state_set;

  check_menu_item = GTK_CHECK_MENU_ITEM (widget);

  if (gtk_check_menu_item_get_active (check_menu_item))
    atk_state_set_add_state (state_set, ATK_STATE_CHECKED);

  if (gtk_check_menu_item_get_inconsistent (check_menu_item))
    atk_state_set_remove_state (state_set, ATK_STATE_ENABLED);
 
  return state_set;
}

static void
gail_check_menu_item_real_notify_gtk (GObject           *obj,
                                    GParamSpec        *pspec)
{
  GtkCheckMenuItem *check_menu_item = GTK_CHECK_MENU_ITEM (obj);
  AtkObject *atk_obj;

  atk_obj = gtk_widget_get_accessible (GTK_WIDGET (check_menu_item));

  if (strcmp (pspec->name, "inconsistent") == 0)
    atk_object_notify_state_change (atk_obj, ATK_STATE_ENABLED,
                       (gtk_widget_get_sensitive (GTK_WIDGET (check_menu_item)) &&
                        !gtk_check_menu_item_get_inconsistent (check_menu_item)));
  else if (strcmp (pspec->name, "sensitive") == 0)
    {
      /* Need to override gailwidget behavior of notifying for ENABLED */
      gboolean sensitive;
      gboolean enabled;
      sensitive = gtk_widget_get_sensitive (GTK_WIDGET (check_menu_item));
      enabled = sensitive &&
                !gtk_check_menu_item_get_inconsistent (check_menu_item);
    atk_object_notify_state_change (atk_obj, ATK_STATE_SENSITIVE, sensitive);
    atk_object_notify_state_change (atk_obj, ATK_STATE_ENABLED, enabled);
    }
  else
    GAIL_WIDGET_CLASS (gail_check_menu_item_parent_class)->notify_gtk (obj, pspec);
}
