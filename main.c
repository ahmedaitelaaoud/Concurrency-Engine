/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:16:58 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:39:12 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	main(int argc, char **argv)
{
	t_setup		setup;
	t_core		core;

	if (parse_args(argc, argv, &setup))
		return (1);
	if (setup_core(&core, &setup))
		return (1);
	if (spawn_threads(&core))
	{
		clean_core(&core, core.setup.dev_count, core.setup.dev_count);
		return (1);
	}
	reap_threads(&core);
	clean_core(&core, core.setup.dev_count, core.setup.dev_count);
	return (0);
}
