/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fpart.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkirszba <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/12 15:26:38 by rkirszba          #+#    #+#             */
/*   Updated: 2019/09/12 15:37:51 by rkirszba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

double	ft_fpart(double nb)
{
	return (nb > 0 ? nb - (long long)nb : (long long)nb - nb);
}
