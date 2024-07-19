#include "../includes/ft_ping.h"

void error_exit_program(t_data *data, char *error_message) {

	fprintf(stderr,"%s\n", error_message);
	close(data->sockfd);
	free(data->rtt_arr);
	exit (EXIT_FAILURE);
}