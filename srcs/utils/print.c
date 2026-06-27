/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsato <nsato@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 21:46:42 by nsato             #+#    #+#             */
/*   Updated: 2026/06/27 23:13:28 by nsato            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/// """
/// Output an error message to standard error.
/// """
#include "../../hdrs/codexion.h"

/// """
/// If even 'fprintf' fails, exit without doing anything.
/// """
int	print_error(const char *msg)
{
	if (fprintf(stderr, "Error: %s\n", msg) < 0)
		return (1);
	return (1);
}
