/* Copyright (c) 2018 Flavio Leitner <fbl@sysclose.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>

struct irssi_plugin {
	XfcePanelPlugin *plugin;
	GtkWidget *image;
};


static struct irssi_plugin *
irssi_create(XfcePanelPlugin *plugin)
{
	struct irssi_plugin *irssi;
	gint size;

	size = xfce_panel_plugin_get_size(plugin)
		/ xfce_panel_plugin_get_nrows(plugin);

	irssi = g_new(struct irssi_plugin, 1);
	irssi->plugin = plugin;
	irssi->image = xfce_panel_image_new_from_source("help-about");
	xfce_panel_image_set_size(XFCE_PANEL_IMAGE(irssi->image), size);
	xfce_panel_plugin_set_small(plugin, TRUE);
	gtk_container_add(GTK_CONTAINER(plugin), irssi->image);
	gtk_widget_show(irssi->image);

	return irssi;
}

static void
irssi_free(XfcePanelPlugin *plugin, struct irssi_plugin *irssi)
{
	g_free(irssi);
}

static void
irssi_construct (XfcePanelPlugin *plugin)
{
	struct irssi_plugin *irssi;

	irssi = irssi_create(plugin);
	g_signal_connect(plugin, "free-data", G_CALLBACK(irssi_free), irssi);
}

XFCE_PANEL_PLUGIN_REGISTER (irssi_construct);
