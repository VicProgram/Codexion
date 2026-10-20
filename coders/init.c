/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 20:48:01 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/20 18:21:08 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	sim->dongles = malloc(sizeof(t_dongle) * sim->n_coders);
	if (sim->dongles == NULL)
		return (0);
	while (i < sim->n_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		sim->dongles[i].is_free = 1;
		sim->dongles[i].cooldown_until = 0;
		i++;
	}
	return (1);
}

static int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	sim->coders = malloc(sizeof(t_coder) * sim->n_coders);
	if (sim->coders == NULL)
		return (0);
	while (i < sim->n_coders)
	{
		sim->coders[i].coder_id = i + 1;
		sim->coders[i].compile_times = 0;
		sim->coders[i].last_compile = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->n_coders];
		i++;
	}
	return (1);
}

int	init(t_sim *sim)
{
	if (!init_dongles(sim))
		return (0);
	if (!init_coders(sim))
	{
		cleanup(sim);
		return (0);
	}
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	sim->stop = 0;
	return (1);
}

void	cleanup(t_sim *sim)
{
	int	i;

	i = 0;
	while (sim->n_coders > i)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	free(sim->dongles);
	free(sim->coders);
	sim->dongles = NULL;
	sim->coders = NULL;
}
