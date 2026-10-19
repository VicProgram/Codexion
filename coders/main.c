/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/16 13:37:19 by username         #+#    #+#              */
/*   Updated: 2026/10/19 19:37:11 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_number(char **args)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	while (i < 8)
	{
		while (args[i][j] != '\0')
		{
			if (args[i][j] >= '0' && args[i][j] <= '9')
				j++;
			else
				return (0);
		}
		if (j == 0)
			return (0);
		i++;
		j = 0;
	}
	return (1);
}

int	check_scheduler(char **args)
{
	if (strcmp(args[8], "fifo") != 0 && strcmp(args[8], "edf") != 0)
		return (0);
	return (1);
}

int	parse_args(char **args)
{
	if (!check_number(args))
	{
		fprintf(stderr, "Error: invalid argument\n");
		return (0);
	}
	if (!check_scheduler(args))
	{
		fprintf(stderr, "Error: invalid argument\n");
		return (0);
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
	if (!parse_args(argv))
		return (1);
	return (0);
}
