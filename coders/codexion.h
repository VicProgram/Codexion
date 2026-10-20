/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 13:52:54 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/20 18:21:24 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_sim	t_sim;
typedef struct s_dongle	t_dongle;
typedef struct s_coder	t_coder;

typedef struct s_sim
{
	int				stop;
	int				n_coders;
	int				compiles_required;
	int				comp_time;
	int				debug_time;
	int				refactor_time;
	int				time_to_burnout;
	int				dongle_cooldown;
	long			start_time;
	char			*scheduler;
	pthread_t		thread;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_sim;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	int				is_free;
	long			cooldown_until;
}	t_dongle;

typedef struct s_coder
{
	int			coder_id;
	int			compile_times;
	long		last_compile;
	t_sim		*sim;
	t_dongle	*left;
	t_dongle	*right;
	pthread_t	thread;
}	t_coder;

int		check_number(char **args);
int		check_scheduler(char **args);
int		check_range(t_sim *sim);
int		parse_args(t_sim *sim, char **args);
int		init(t_sim *sim);

void	fill_sim(t_sim *sim, char **args);
void	cleanup(t_sim *sim);

#endif
