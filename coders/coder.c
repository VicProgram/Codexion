/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   coder.c                                           :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: vabad-ro <vabad-ro@student.42madrid.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/20 19:04:39 by vabad-ro         #+#    #+#              */
/*   Updated: 2026/10/20 19:14:00 by vabad-ro        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_loop(void *arg)
{
	int	i;

	i = 0;
	t_coder	*c = arg;

	while (i < c->sim->compiles_required)
	{
		printf("Compile");
		usleep(c->sim->compiles_required * 1000);
		c->compile_times += 1;
		printf("Debug");
		printf("Wait");
	}
}
