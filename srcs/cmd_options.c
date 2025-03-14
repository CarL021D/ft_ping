#include "../includes/ft_ping.h"

bool c_option_exec(t_data *data) {

	if (!data->option.c)
		return (false);

	if (data->sent_pckt_count >= data->option.c)
		return (true);
	return (false);
}

void help_option_exec() {

	printf(
		"    Usage: ping [OPTION...] HOST ...\n"
		"Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
		" Options valid for all request types:\n\n"
		"  -c, --count=NUMBER         stop after sending NUMBER packets\n"
		"  -i, --interval=NUMBER      wait NUMBER seconds between sending each packet\n"
		"  -v, --verbose              verbose output\n"
		" Options valid for --echo requests:\n"
		"  -f, --flood                flood ping (root only)\n"
		"  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n"
		"                             falling into normal mode of behavior (root only)\n"
		"  -q, --quiet                quiet output\n"
		"  -?, --help                 give this help list\n"
		"Mandatory or optional arguments to long options are also mandatory or optional\n"
		"for any corresponding short options.\n\n"
		"Options marked with (root only) are available only to superuser.\n"
	);
}