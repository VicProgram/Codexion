/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 19:55:13 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/20 22:57:24 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	int i;
	t_sim	sim;

	i = 0;
	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments\n");
		return (1);
	}
	if (!parse_args(&sim, argv))
		return (1);
	if (!init(&sim))
		return (1);
	while (i < sim.n_coders)
	{
		pthread_create(&sim.coders[i].thread, NULL, coder_loop, &sim.coders[i]);
		i++;
	}
	i = 0;
	while (i < sim.n_coders)
	{
		pthread_join(sim.coders[i].thread, NULL);
		i++;
	}

	cleanup(&sim);
	return (0);
}
