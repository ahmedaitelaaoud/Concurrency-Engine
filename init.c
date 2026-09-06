/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:18:15 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/05 09:18:25 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	drop_cv(t_core *core, int i)
{
	while (--i >= 0)
		pthread_cond_destroy(&core->devs[i].cv);
	return (print_err("Failed to initialize dev condition variable"));
}

static int	setup_devs(t_core *core)
{
	int	i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		core->devs[i].dev_id = i + 1;
		core->devs[i].last_comp_start = core->time_start;
		core->devs[i].comps_done = 0;
		core->devs[i].l_key = &core->keys[i];
		core->devs[i].r_key = &core->keys[(i + 1) % core->setup.dev_count];
		if (i == (core->setup.dev_count - 1))
		{
			core->devs[i].l_key = &core->keys[(i + 1)
				% core->setup.dev_count];
			core->devs[i].r_key = &core->keys[i];
		}
		core->devs[i].core = core;
		if (pthread_cond_init(&core->devs[i].cv, NULL) != 0)
			return (drop_cv(core, i));
		i++;
	}
	return (0);
}

static int	setup_keys(t_core *core)
{
	int	i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		if (pthread_mutex_init(&core->keys[i].mtx, NULL) != 0)
			return (clean_core(core, i, 0),
				print_err("Failed to initialize key mtx"));
		if (pthread_cond_init(&core->keys[i].cv, NULL) != 0)
		{
			pthread_mutex_destroy(&core->keys[i].mtx);
			return (clean_core(core, i, 0),
				print_err("Failed to initialize key condition variable"));
		}
		core->keys[i].is_free = 1;
		core->keys[i].free_time = core->time_start;
		i++;
	}
	return (0);
}

static int	mem_alloc(t_core *core)
{
	core->devs = malloc(sizeof(t_dev) * core->setup.dev_count);
	if (!core->devs)
		return (print_err("Malloc failed"));
	memset(core->devs, 0, sizeof(t_dev) * core->setup.dev_count);
	core->keys = malloc(sizeof(t_key) * core->setup.dev_count);
	if (!core->keys)
	{
		free(core->devs);
		return (print_err("Malloc failed"));
	}
	memset(core->keys, 0, sizeof(t_key) * core->setup.dev_count);
	return (0);
}

int	setup_core(t_core *core, t_setup *setup)
{
	core->setup = *setup;
	core->is_active = 1;
	core->watcher = (pthread_t)0;
	core->time_start = get_time_ms();
	if (pthread_mutex_init(&core->mtx_state, NULL) != 0)
		return (print_err("Failed to initialize state mtx"));
	if (pthread_mutex_init(&core->mtx_log, NULL) != 0)
	{
		pthread_mutex_destroy(&core->mtx_state);
		return (print_err("Failed to initialize log mtx"));
	}
	if (mem_alloc(core))
		return (drop_mtx(core), 1);
	if (setup_keys(core))
		return (1);
	if (setup_devs(core))
	{
		clean_core(core, core->setup.dev_count,
			core->setup.dev_count);
		return (1);
	}
	return (0);
}
