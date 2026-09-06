/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:20:36 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:47:19 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	delay_ms(t_core *core, long tm)
{
	long	start;

	start = get_time_ms();
	while (is_core_active(core))
	{
		if (get_time_ms() - start >= tm)
			break ;
		usleep(500);
	}
}

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	is_core_active(t_core *core)
{
	int	is_active;

	pthread_mutex_lock(&core->mtx_state);
	is_active = core->is_active;
	pthread_mutex_unlock(&core->mtx_state);
	return (is_active);
}

int	is_dev_done(t_dev *dev)
{
	int	done;

	pthread_mutex_lock(&dev->core->mtx_state);
	done = dev->comps_done >= dev->core->setup.req_comps;
	pthread_mutex_unlock(&dev->core->mtx_state);
	return (done);
}

void	delay_odd_devs(t_dev *dev)
{
	if (dev->dev_id % 2)
		usleep(200);
	return ;
}
