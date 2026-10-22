/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 19:04:39 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/22 14:52:52 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_dongle *dongle)
{
	long	now;

	pthread_mutex_lock(&dongle->mutex);
	while (1)
	{
		if (dongle->is_free == 1)
		{
			now = get_time_ms();
			if (now >= dongle->cooldown_until)
			{
				dongle->is_free = 1;
				pthread_mutex_unlock(&dongle->mutex);
				return ;
			}
		}
		wait_until_ms(&dongle->cond,
			&dongle->mutex, dongle->cooldown_until);
	}
	while (dongle->is_free == 0 || get_time_ms() < dongle->cooldown_until)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	dongle->is_free = 0;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->cooldown_until = get_time_ms() + dongle->cooldown_ms;
	dongle->is_free = 1;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

// QUITAR LOS PRINTS

void	*coder_loop(void *arg)
{
	t_coder	*c;

	c = arg;
	while (c->compile_times < c->sim->compiles_required)
	{
		if (c->left == c->right)
			take_dongle(c->left);
		else
		{
			take_dongle(c->left);
			take_dongle(c->right);
		}
		printf("Compile\n");
		usleep(c->sim->comp_time * 1000);
		c->compile_times += 1;
		if (c->left == c->right)
			release_dongle(c->left);
		else
		{
			release_dongle(c->left);
			release_dongle(c->right);
		}
		printf("Debug\n");
		usleep(c->sim->debug_time * 1000);
		printf("Refactor\n");
		usleep(c->sim->refactor_time * 1000);
	}
	return (NULL);
}
