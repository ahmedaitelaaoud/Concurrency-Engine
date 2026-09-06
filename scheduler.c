/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:19:04 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:40:16 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	calc_prio(t_dev *dev)
{
	long	value;

	pthread_mutex_lock(&dev->core->mtx_state);
	if (dev->core->setup.sched_type == EDF)
		value = dev->last_comp_start
			+ dev->core->setup.timeout_burn;
	else
		value = get_time_ms();
	pthread_mutex_unlock(&dev->core->mtx_state);
	return (value);
}

static void	swap_jobs(t_job *a, t_job *b)
{
	t_job	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	waitlist_add(t_waitlist *q, t_dev *dev, long prio)
{
	q->jobs[q->len].dev = dev;
	q->jobs[q->len].prio = prio;
	q->len++;
	if (q->len == 2
		&& q->jobs[0].prio > q->jobs[1].prio)
		swap_jobs(&q->jobs[0], &q->jobs[1]);
}

void	waitlist_drop(t_waitlist *q)
{
	if (q->len == 2)
		q->jobs[0] = q->jobs[1];
	if (q->len)
		q->len--;
}

t_dev	*waitlist_peek(t_waitlist *q)
{
	if (q->len == 0)
		return (NULL);
	return (q->jobs[0].dev);
}
