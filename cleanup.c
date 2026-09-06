/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 09:38:03 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:38:15 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	clean_keys(t_core *core, int key_count)
{
	int	i;

	i = 0;
	while (key_count > i)
	{
		pthread_cond_destroy(&core->keys[i].cv);
		pthread_mutex_destroy(&core->keys[i].mtx);
		i++;
	}
}

static void	clean_devs(t_core *core, int dev_count)
{
	int	i;

	i = 0;
	while (dev_count > i)
	{
		pthread_cond_destroy(&core->devs[i].cv);
		i++;
	}
}

void	clean_core(t_core *core, int key_count, int dev_count)
{
	if (core->devs)
	{
		clean_devs(core, dev_count);
		free(core->devs);
		core->devs = NULL;
	}
	if (core->keys)
	{
		clean_keys(core, key_count);
		free(core->keys);
		core->keys = NULL;
	}
	pthread_mutex_destroy(&core->mtx_log);
	pthread_mutex_destroy(&core->mtx_state);
}

void	drop_mtx(t_core *core)
{
	pthread_mutex_destroy(&core->mtx_log);
	pthread_mutex_destroy(&core->mtx_state);
}

int	print_err(char *msg)
{
	printf("%s\n", msg);
	return (1);
}
