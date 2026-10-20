/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   testpthread.c                                     :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: vabad-ro <vabad-ro@student.42madrid.com>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/20 16:06:16 by vabad-ro         #+#    #+#              */
/*   Updated: 2026/10/20 16:08:13 by vabad-ro        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>

int				compartida = 0;
pthread_mutex_t	mut;

void	*trabajo(void *arg)
{
	int	i;

	(void) arg;
	i = 0;
	while (i < 200000)
	{
		pthread_mutex_lock(&mut);
		compartida = compartida + 1;
		pthread_mutex_unlock(&mut);
		i++;
	}
	return (NULL);
}

int	main(void)
{
	pthread_t	t[5];
	int			i;

	pthread_mutex_init(&mut, NULL);
	i = 0;
	while (i < 5)
	{
		pthread_create(&t[i], NULL, trabajo, NULL);
		i++;
	}
	i = 0;
	while (i < 5)
	{
		pthread_join(t[i], NULL);
		i++;
	}
	printf("esperado: 1000000  |  real: %d\n", compartida);
	pthread_mutex_destroy(&mut);
	return (0);
}
