/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aait-ela <aait-ela@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/05 09:17:40 by aait-ela          #+#    #+#             */
/*   Updated: 2026/09/06 09:39:51 by aait-ela         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	str_to_long(const char *str)
{
	long	result;
	int		i;

	if (!str || !*str || (str[0] == '0' && str[1]))
		return (-1);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		result = result * 10 + (str[i++] - '0');
	}
	return (result);
}

static int	parse_nums(char **av, t_setup *cfg)
{
	long	v[7];
	int		i;

	i = 0;
	while (7 > i)
	{
		v[i] = str_to_long(av[i + 1]);
		if (v[i] < 0 || v[i] > INT_MAX)
			return (print_err("Invalid numeric argument"));
		i++;
	}
	cfg->dev_count = (int)v[0];
	cfg->timeout_burn = v[1];
	cfg->timeout_comp = v[2];
	cfg->timeout_dbg = v[3];
	cfg->timeout_ref = v[4];
	cfg->req_comps = (int)v[5];
	cfg->key_rest = v[6];
	return (0);
}

static int	check_setup(t_setup *cfg)
{
	if (cfg->dev_count <= 0 || cfg->dev_count > 1000)
		return (print_err("Invalid number of devs"));
	if (cfg->timeout_burn <= 0 || cfg->timeout_comp <= 0)
		return (print_err("Invalid timing"));
	if (cfg->timeout_dbg <= 0 || cfg->timeout_ref <= 0)
		return (print_err("Invalid timing"));
	if (cfg->req_comps <= 0)
		return (print_err("Invalid required compiles"));
	if (cfg->key_rest < 0)
		return (print_err("Invalid cooldown"));
	return (0);
}

static int	parse_sched(char *arg, t_setup *cfg)
{
	if (!strcmp(arg, "fifo"))
		cfg->sched_type = FIFO;
	else if (!strcmp(arg, "edf"))
		cfg->sched_type = EDF;
	else
		return (print_err("Invalid sched_type"));
	return (0);
}

int	parse_args(int ac, char **av, t_setup *cfg)
{
	*cfg = (t_setup){0};
	if (ac != 9)
		return (print_err("Usage: ./codexion <devs> <burnout> <do_compile> "
				"<debug> <refactor> <required_compiles> "
				"<cooldown> <fifo|edf>"));
	if (parse_nums(av, cfg))
		return (1);
	if (parse_sched(av[8], cfg))
		return (1);
	return (check_setup(cfg));
}
