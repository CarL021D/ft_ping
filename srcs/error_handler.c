#include "../includes/ft_ping.h"

void arg_error_exit_program() {
	
	fprintf(stderr,"arg error\n");
	exit(EXIT_FAILURE);	
}

void error_exit_program(t_data *data, char *error_message) {
	if (!data->option.q)
		fprintf(stderr,"%s\n", error_message);
	close(data->sockfd);
	free(data->rtt_arr);
	exit (EXIT_FAILURE);
}