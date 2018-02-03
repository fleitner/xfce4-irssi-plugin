# Welcome to yet another irssi notification solution

Welcome!

The goal of this project is to provide a simple notification mechanism in the **xfce4** panel.  For now the only it does is to blink the icon. You can stop the notification by clicking on the icon.

## Installation

You need to build following these steps:
1. meson /tmp/builddir
2. ninja -C /tmp/builddir

Now copy the plugin, the desktop and icons to the system:
3. sudo cp builddir/libxfce4-irssi-plugin.so /usr/lib64/xfce4/panel/plugins
4. sudo cp xfce4-irssi-plugin.desktop  /usr/share/xfce4/panel/plugins
5. sudo xdg-icon-resource install --size 32 --context apps icon/32x32/xfce4-plugin-irssi-normal.png 
6. sudo xdg-icon-resource install --size 32 --context apps icon/32x32/xfce4-plugin-irssi-notify.png 

The last installation step is to install the irssi plugin:
7. mkdir -p  ~/.irssi/scripts
8. cp xfce4-irssi-plugin.pl ~/.irssi/scripts/

## Loading the script

Load the script inside irssi:
/script load xfce4-irssi-plugin.pl
 


