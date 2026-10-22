/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vabad-ro <vabad-ro@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 13:52:54 by vabad-ro          #+#    #+#             */
/*   Updated: 2026/10/22 14:45:04 by vabad-ro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <sys/time.h>
# include <pthread.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_sim	t_sim;
typedef struct s_dongle	t_dongle;
typedef struct s_coder	t_coder;

typedef struct s_sim
{
	int					stop;
	int					n_coders;
	int					compiles_required;
	int					comp_time;
	int					debug_time;
	int					refactor_time;
	int					time_to_burnout;
	int					dongle_cooldown;
	long				start_time;
	char				*scheduler;
	pthread_t			thread;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		log_mutex;
	t_coder				*coders;
	t_dongle			*dongles;
}	t_sim;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_free;
	int				cooldown_ms;
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

long	get_time_ms(void);
void	wait_until_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, long ms);

void	run_sim(t_sim *sim);
void	fill_sim(t_sim *sim, char **args);
void	cleanup(t_sim *sim);
void	*coder_loop(void *arg);
void	join_threads(t_sim *sim, int n);
void	take_dongle(t_dongle *dongle);
void	release_dongle(t_dongle *dongle);

#endif
