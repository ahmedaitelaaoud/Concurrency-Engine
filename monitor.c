/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:18:34 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:39:32 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	alert_all_devs(t_core *core)
{
	int	i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		pthread_mutex_lock(&core->keys[i].mtx);
		pthread_cond_broadcast(&core->keys[i].cv);
		pthread_mutex_unlock(&core->keys[i].mtx);
		i++;
	}
}

static int	verify_burnout(t_core *core)
{
	long	time;
	int		i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		pthread_mutex_lock(&core->mtx_state);
		time = get_time_ms() - core->devs[i].last_comp_start;
		if (time > core->setup.timeout_burn)
		{
			core->is_active = 0;
			pthread_mutex_unlock(&core->mtx_state);
			time = get_time_ms() - core->devs[i].core->time_start;
			pthread_mutex_lock(&core->devs[i].core->mtx_log);
			printf("%ld %d burned out\n", time, i + 1);
			pthread_mutex_unlock(&core->devs[i].core->mtx_log);
			alert_all_devs(core);
			return (1);
		}
		pthread_mutex_unlock(&core->mtx_state);
		i++;
	}
	return (0);
}

static int	verify_comps(t_core *core)
{
	int	i;
	int	compiles;

	i = 0;
	compiles = 0;
	while (core->setup.dev_count > i)
	{
		pthread_mutex_lock(&core->mtx_state);
		if (core->devs[i].comps_done
			>= core->setup.req_comps)
			compiles++;
		pthread_mutex_unlock(&core->mtx_state);
		i++;
	}
	if (compiles == core->setup.dev_count)
	{
		pthread_mutex_lock(&core->mtx_state);
		core->is_active = 0;
		pthread_mutex_unlock(&core->mtx_state);
		alert_all_devs(core);
		return (1);
	}
	return (0);
}

void	*watcher(void *arg)
{
	t_core	*core;

	core = (t_core *)arg;
	while (is_core_active(core))
	{
		usleep(500);
		if (verify_burnout(core))
			return (NULL);
		if (verify_comps(core))
			return (NULL);
	}
	return (NULL);
}
