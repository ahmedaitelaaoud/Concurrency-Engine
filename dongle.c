/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 09:38:31 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:38:39 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_key(t_key *key, t_core *core)
{
	pthread_mutex_lock(&key->mtx);
	key->is_free = 1;
	key->free_time = get_time_ms()
		+ core->setup.key_rest;
	pthread_cond_broadcast(&key->cv);
	pthread_mutex_unlock(&key->mtx);
}
