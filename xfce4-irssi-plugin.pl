use strict;
use Irssi;
use vars qw($VERSION %IRSSI);
use HTML::Entities;

$VERSION = "0.1";
%IRSSI = (
    authors     => 'Flavio Leitner',
    contact     => 'fbl@sysclose.org',
    name        => 'xfce4-irssi-plugin.pl',
    description => 'Use xfce4-irssi-plugin.pl to notify',
    license     => 'GNU General Public License',
    url         => 'github',
);

sub notify {
    my ($server) = @_;
    my $cmd = "EXEC - " .  " exec printf '1' | nc -u 127.0.0.1 3154";
    $server->command($cmd);
}

sub print_text_notify {
    my ($dest, $text, $stripped) = @_;
    my $server = $dest->{server};

    return if (!$server || !($dest->{level} & MSGLEVEL_HILIGHT));
    notify($server);
}

sub message_private_notify {
    my ($server, $msg, $nick, $address) = @_;

    return if (!$server);
    notify($server);
}

sub dcc_request_notify {
    my ($dcc, $sendaddr) = @_;
    my $server = $dcc->{server};

    return if (!$dcc);
    notify($server);
}

Irssi::signal_add('print text', 'print_text_notify');
Irssi::signal_add('message private', 'message_private_notify');
Irssi::signal_add('dcc request', 'dcc_request_notify');


