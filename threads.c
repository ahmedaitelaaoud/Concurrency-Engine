/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:19:24 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:40:24 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	spawn_threads(t_core *core)
{
	int	i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		if (pthread_create(&core->devs[i].th_id, NULL,
				dev_loop, &core->devs[i]))
		{
			while (--i >= 0)
				pthread_join(core->devs[i].th_id, NULL);
			return (print_err("Failed to create th_id"));
		}
		i++;
	}
	if (pthread_create(&core->watcher, NULL, watcher, core))
	{
		reap_threads(core);
		return (print_err("Failed to create watcher th_id"));
	}
	return (0);
}

void	reap_threads(t_core *core)
{
	int	i;

	i = 0;
	while (core->setup.dev_count > i)
	{
		pthread_join(core->devs[i].th_id, NULL);
		i++;
	}
	if (core->watcher)
		pthread_join(core->watcher, NULL);
}
