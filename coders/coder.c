/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 19:04:39 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/21 17:15:21 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	while (dongle->is_free == 0)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	dongle->is_free = 0;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{

}

void	*coder_loop(void *arg)
{
	t_coder	*c;

	c = arg;
	while (c->compile_times < c->sim->compiles_required)
	{
		take_dongle(c->left);
		take_dongle(c->right);
		printf("Compile\n");
		usleep(c->sim->comp_time * 1000);
		c->compile_times += 1;
		release_dongle(c->left);
		release_dongle(c->right);
		printf("Debug\n");
		usleep(c->sim->debug_time * 1000);
		printf("Refactor\n");
		usleep(c->sim->refactor_time * 1000);
	}
	return (NULL);
}
