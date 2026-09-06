/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:18:48 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:40:03 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	await_key(t_dev *dev, t_key *key)
{
	struct timespec	ts;

	pthread_mutex_lock(&key->mtx);
	while (!key->is_free || get_time_ms() < key->free_time
		|| waitlist_peek(&key->wait_q) != dev)
	{
		if (!is_core_active(dev->core))
			return (pthread_mutex_unlock(&key->mtx), 1);
		ts.tv_sec = key->free_time / 1000;
		ts.tv_nsec = (key->free_time % 1000) * 1000000;
		if (!key->is_free || waitlist_peek(&key->wait_q) != dev)
			pthread_cond_wait(&key->cv, &key->mtx);
		if (!is_core_active(dev->core))
			return (pthread_mutex_unlock(&key->mtx), 1);
		if (get_time_ms() < key->free_time)
			pthread_cond_timedwait(&key->cv, &key->mtx, &ts);
	}
	waitlist_drop(&key->wait_q);
	key->is_free = 0;
	pthread_mutex_unlock(&key->mtx);
	return (0);
}

static int	grab_keys(t_dev *dev)
{
	long			prio;

	prio = calc_prio(dev);
	pthread_mutex_lock(&dev->l_key->mtx);
	waitlist_add(&dev->l_key->wait_q, dev, prio);
	pthread_mutex_unlock(&dev->l_key->mtx);
	prio = calc_prio(dev);
	pthread_mutex_lock(&dev->r_key->mtx);
	waitlist_add(&dev->r_key->wait_q, dev, prio);
	pthread_mutex_unlock(&dev->r_key->mtx);
	if (await_key(dev, dev->l_key))
		return (1);
	if (!print_log(dev, "has taken a dongle"))
		return (1);
	if (await_key(dev, dev->r_key))
	{
		free_key(dev->l_key, dev->core);
		return (1);
	}
	if (!print_log(dev, "has taken a dongle"))
		return (1);
	return (0);
}

static void	drop_keys(t_dev *dev)
{
	free_key(dev->l_key, dev->core);
	free_key(dev->r_key, dev->core);
}

static void	do_compile(t_dev *dev)
{
	pthread_mutex_lock(&dev->core->mtx_state);
	dev->last_comp_start = get_time_ms();
	pthread_mutex_unlock(&dev->core->mtx_state);
	print_log(dev, "is compiling");
	delay_ms(dev->core, dev->core->setup.timeout_comp);
	pthread_mutex_lock(&dev->core->mtx_state);
	dev->comps_done++;
	pthread_mutex_unlock(&dev->core->mtx_state);
}

void	*dev_loop(void *arg)
{
	t_dev	*dev;

	dev = (t_dev *)arg;
	delay_odd_devs(dev);
	while (is_core_active(dev->core) && !is_dev_done(dev))
	{
		if (grab_keys(dev))
			break ;
		if (!is_core_active(dev->core))
		{
			drop_keys(dev);
			break ;
		}
		do_compile(dev);
		drop_keys(dev);
		if (!is_core_active(dev->core))
			break ;
		print_log(dev, "is debugging");
		delay_ms(dev->core, dev->core->setup.timeout_dbg);
		if (!is_core_active(dev->core))
			break ;
		print_log(dev, "is refactoring");
		delay_ms(dev->core, dev->core->setup.timeout_ref);
	}
	return (NULL);
}
