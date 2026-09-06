/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 09:38:55 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:39:03 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	print_log(t_dev *dev, char *msg)
{
	long	time;

	pthread_mutex_lock(&dev->core->mtx_log);
	if (!is_core_active(dev->core))
	{
		pthread_mutex_unlock(&dev->core->mtx_log);
		return (0);
	}
	time = get_time_ms() - dev->core->time_start;
	printf("%ld %d %s\n", time, dev->dev_id, msg);
	pthread_mutex_unlock(&dev->core->mtx_log);
	return (1);
}
