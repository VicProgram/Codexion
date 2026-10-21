/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 19:55:13 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/21 15:58:58 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	join_threads(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

void	run_sim(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_loop, &sim->coders[i]) != 0)
			break ;
		i++;
	}
	join_threads(sim, i);
	if (i != sim->n_coders)
		cleanup(sim);
}

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
	run_sim(&sim);
	cleanup(&sim);
	return (0);
}
