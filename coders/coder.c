/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 19:04:39 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/20 22:10:40 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_loop(void *arg)
{
	t_coder	*c;

	c = arg;
	while (c->compile_times < c->sim->compiles_required)
	{
		printf("Compile\n");
		usleep(c->sim->comp_time * 1000);
		c->compile_times += 1;
		printf("Debug\n");
		usleep(c->sim->debug_time * 1000);
		printf("Refactor\n");
		usleep(c->sim->refactor_time * 1000);
	}
	return (NULL);
}
