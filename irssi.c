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

#define PORT 3154

struct irssi_plugin {
	XfcePanelPlugin *plugin;
	GtkWidget *image;
	gint timer;
	gboolean showing;
	GSocket *socket;
	guint watch;
};

gint
irssi_timer_callback(gpointer data)
{
	struct irssi_plugin *irssi = (struct irssi_plugin *)data;

	if (irssi->showing) {
		gtk_widget_hide(irssi->image);
	} else {
		gtk_widget_show(irssi->image);
	}

	irssi->showing = !irssi->showing;

	return TRUE;
}

static void
irssi_set_blinking(struct irssi_plugin *irssi, gboolean enable)
{
	if (enable) {
		if (irssi->timer == 0) {
			irssi->timer = g_timeout_add(1000,
						irssi_timer_callback,
						irssi);
		}
	} else {
		if (irssi->timer > 0) {
			g_source_remove(irssi->timer);
			irssi->timer = 0;
		}

		gtk_widget_hide(irssi->image);
		irssi->showing = FALSE;
	}
}

static gboolean
irssi_event_handler(GIOChannel *channel, GIOCondition condition,
		    gpointer data)
{
	struct irssi_plugin *irssi = (struct irssi_plugin *)data;

	if (condition == G_IO_IN || condition == G_IO_PRI) {
		gchar msgid[2];
		gsize bytes_read;
		GError *err = NULL;

		g_io_channel_read_chars(channel, msgid, 2, &bytes_read, &err);
		if (!err) {
			switch (msgid[0]) {
			case '1':
				irssi_set_blinking(irssi, TRUE);
				break;
			case '2':
				irssi_set_blinking(irssi, FALSE);
				break;
			}
		} else {
			g_printerr("%s: %s", __FUNCTION__, err->message);
		}
	}

	return TRUE;
}

static struct irssi_plugin *
irssi_systray_create(XfcePanelPlugin *plugin)
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
	irssi->timer = 0;
	irssi->showing = FALSE;

	return irssi;
}
static int
irssi_socket_create(struct irssi_plugin *irssi)
{
	GError *err = NULL;

	irssi->socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
				     G_SOCKET_TYPE_DATAGRAM,
				     G_SOCKET_PROTOCOL_UDP, &err);

	if (err != NULL) {
		g_printerr("%s: %s", __FUNCTION__, err->message);
		g_assert(err == NULL);
	}

	GInetAddress *inet = g_inet_address_new_loopback(G_SOCKET_FAMILY_IPV4);
	GSocketAddress *addr = g_inet_socket_address_new(inet, PORT);
	g_socket_bind(irssi->socket, addr, FALSE, &err);
	if (err != NULL) {
		g_printerr("%s: %s", __FUNCTION__, err->message);
		g_assert(err == NULL);
	}

	int fd = g_socket_get_fd(irssi->socket);
	GIOChannel* channel = g_io_channel_unix_new(fd);
	irssi->watch = g_io_add_watch(channel, G_IO_IN, (GIOFunc) irssi_event_handler, irssi);
	g_io_channel_unref(channel);

	return 0;
}

static void
irssi_free(XfcePanelPlugin *plugin, struct irssi_plugin *irssi)
{
	GError *err;

	g_source_remove(irssi->watch);
	g_socket_close(irssi->socket, &err);
	g_free(irssi);
}

static void
irssi_construct (XfcePanelPlugin *plugin)
{
	struct irssi_plugin *irssi;

	irssi = irssi_systray_create(plugin);
	irssi_socket_create(irssi);
	g_signal_connect(plugin, "free-data", G_CALLBACK(irssi_free), irssi);
}

XFCE_PANEL_PLUGIN_REGISTER (irssi_construct);
