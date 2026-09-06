/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 12:38:14 by aait-ela          #+#    #+#             */
/*   Updated: 2026/08/31 13:50:27 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <limits.h>
# include <sys/time.h>
# include <time.h>

# define FIFO	1
# define EDF	2

typedef struct s_core	t_core;
typedef struct s_dev	t_dev;
typedef struct s_key	t_key;

typedef struct s_job
{
	t_dev		*dev;
	long		prio;
}	t_job;

typedef struct s_waitlist
{
	t_job	jobs[2];
	int		len;
}	t_waitlist;

typedef struct s_setup
{
	int		dev_count;
	long	timeout_burn;
	long	timeout_comp;
	long	timeout_dbg;
	long	timeout_ref;
	int		req_comps;
	long	key_rest;
	int		sched_type;
}	t_setup;

struct s_core
{
	pthread_t		watcher;
	t_setup			setup;
	long			time_start;
	pthread_mutex_t	mtx_state;
	int				is_active;
	pthread_mutex_t	mtx_log;
	t_dev			*devs;
	t_key			*keys;
};

typedef struct s_dev
{
	int				dev_id;
	pthread_t		th_id;
	pthread_cond_t	cv;
	long			last_comp_start;
	long			comps_done;
	t_key			*l_key;
	t_key			*r_key;
	t_core			*core;
}	t_dev;

typedef struct s_key
{
	pthread_mutex_t	mtx;
	pthread_cond_t	cv;
	int				is_free;
	long			free_time;
	t_waitlist		wait_q;
}	t_key;

/*parser.c */
int		parse_args(int argc, char **argv, t_setup *setup);

/*init.c*/
int		setup_core(t_core *core, t_setup *setup);

/*cleanup.c */
void	clean_core(t_core *core, int key_count, int dev_count);
void	drop_mtx(t_core *core);
int		print_err(char *msg);

/*routine.c*/
void	*dev_loop(void *arg);

/*threads.c */
int		spawn_threads(t_core *core);
void	reap_threads(t_core *core);

/*watcher.c*/
void	*watcher(void *arg);

/*key.c*/
void	free_key(t_key *key, t_core *core);

/* sched_type.c */
long	calc_prio(t_dev *dev);
void	waitlist_add(t_waitlist *q, t_dev *dev, long prio);
void	waitlist_drop(t_waitlist *q);
t_dev	*waitlist_peek(t_waitlist *q);

/*logger.c*/
int		print_log(t_dev *dev, char *msg);

/*utils.c*/
long	get_time_ms(void);
void	delay_ms(t_core *core, long time);
int		is_core_active(t_core *core);
int		is_dev_done(t_dev *dev);
void	delay_odd_devs(t_dev *dev);

#endif
