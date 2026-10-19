/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/16 13:37:19 by username         #+#    #+#              */
/*   Updated: 2026/10/19 16:06:51 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_args(int n_args, char **args)
{
	int	i;
	int	value;

	if (strcmp(args[8], "fifo") != 0 && strcmp(args[8], "edf") != 0)
	{
		fprintf(stderr, "Error: Scheduler must be 'fifo' or 'edf'\n");
		return (0);
	}
	i = 1;
	while (i < 8)
	{
		if (args[i][0] == '-')
		{
			fprintf(stderr, "Error: Negative numbers are not allowed");
			return (0);
		}
		value = atoi(args[i]);
		if (value < 0)
		{
			fprintf(stderr, "Can't use negative numbers\n");
			return (0);
		}
		i++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments\n");
		return (1);
	}
	if (!parse_args(argc, argv))
		return (1);
	return (0);
}
