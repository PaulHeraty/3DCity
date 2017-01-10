#!/usr/bin/perl

use IO::Socket::INET;

my ($socket,$received_data);

$socket = new IO::Socket::INET (
	LocalPort => '8888',
	Proto => 'udp',
	ReuseAddr => 1,
) or die "Socket failed ! : $!\n";

while(1)
{
	$socket->recv($recieved_data,1024);
	print "<$recieved_data>\n";
# Send Echo
#	$socket->send($recieved_data);
}

$socket->close();
