/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: vabad-ro <vabad-ro@student.42madrid.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/19 19:55:13 by vabad-ro         #+#    #+#              */
/*   Updated: 2026/10/20 18:04:45 by vabad-ro        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments\n");
		return (1);
	}
	if (!parse_args(&sim, argv))
		return (1);
	if (!init(&sim))
		return (1);
	cleanup(&sim);
	return (0);
}
