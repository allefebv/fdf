/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fdf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: allefebv <allefebv@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/16 11:37:17 by rkirszba          #+#    #+#             */
/*   Updated: 2019/09/25 19:42:06 by allefebv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static int	init_mlx(t_fdf *fdf)
{
	if (!(fdf->mlx.ptrs.mlx_ptr = mlx_init()))
		return (print_mlx_error(1));
	if (!(fdf->mlx.ptrs.win_ptr = mlx_new_window(fdf->mlx.ptrs.mlx_ptr,\
		WIN_WDTH, WIN_HGHT, "fdf")))
		return (print_mlx_error(2));
	if (!(fdf->mlx.img.img_ptr = mlx_new_image(fdf->mlx.ptrs.mlx_ptr,\
		IMG_WDTH, IMG_HGHT)))
		return (print_mlx_error(3));
	if (!(fdf->mlx.bg_img_top.img_ptr = mlx_new_image(fdf->mlx.ptrs.mlx_ptr,\
		WIN_WDTH, START_Y_IMG)))
		return (print_mlx_error(3));
	if (!(fdf->mlx.bg_img_bot.img_ptr = mlx_new_image(fdf->mlx.ptrs.mlx_ptr,\
		WIN_WDTH, WIN_HGHT - START_Y_IMG - IMG_HGHT)))
		return (print_mlx_error(3));
	fdf->mlx.img.buf = mlx_get_data_addr(fdf->mlx.img.img_ptr,\
		&fdf->mlx.img.bits_per_pixel, &fdf->mlx.img.size_line,\
		&fdf->mlx.img.endian);
	fdf->mlx.bg_img_top.buf = mlx_get_data_addr(fdf->mlx.bg_img_top.img_ptr,\
		&fdf->mlx.bg_img_top.bits_per_pixel, &fdf->mlx.bg_img_top.size_line,\
		&fdf->mlx.bg_img_top.endian);
	fdf->mlx.bg_img_bot.buf = mlx_get_data_addr(fdf->mlx.bg_img_bot.img_ptr,\
		&fdf->mlx.bg_img_bot.bits_per_pixel, &fdf->mlx.bg_img_bot.size_line,\
		&fdf->mlx.bg_img_bot.endian);
	fdf->mlx.img.size_buf = IMG_WDTH * IMG_HGHT * 4;
	fdf->mlx.bg_img_top.size_buf = WIN_WDTH * START_Y_IMG * 4;
	fdf->mlx.bg_img_bot.size_buf = WIN_WDTH * (WIN_HGHT - START_Y_IMG - IMG_HGHT) * 4;
	fdf->mlx.img.plan.x_min = 0;
	fdf->mlx.img.plan.x_max = IMG_WDTH - 1;
	fdf->mlx.img.plan.y_min = 0;
	fdf->mlx.img.plan.y_max = IMG_HGHT - 1;
	fdf->mlx.bg_img_top.plan.x_min = 0;
	fdf->mlx.bg_img_top.plan.x_max = WIN_WDTH - 1;
	fdf->mlx.bg_img_top.plan.y_min = 0;
	fdf->mlx.bg_img_top.plan.y_max = START_Y_IMG;
	fdf->mlx.bg_img_bot.plan.x_min = 0;
	fdf->mlx.bg_img_bot.plan.x_max = WIN_WDTH - 1;
	fdf->mlx.bg_img_bot.plan.y_min = 0;
	fdf->mlx.bg_img_bot.plan.y_max = WIN_HGHT - START_Y_IMG - IMG_HGHT;
	return (0);
}

static int	init_edges_tab(t_fdf *fdf)
{
	int	ind_edges;
	int	ind_vtx;

	fdf->nb_edges = (fdf->nb_cols - 1) * fdf->nb_rows\
			+ fdf->nb_cols * (fdf->nb_rows - 1);
	if (!(fdf->edges = (t_edge*)malloc(sizeof(t_edge) * fdf->nb_edges)))
		return (print_sys_error(errno));
	ind_vtx = -1;
	ind_edges = 0;
	while (++ind_vtx < fdf->nb_vertices)
	{
		if ((ind_vtx + 1) % fdf->nb_cols)
		{
			fdf->edges[ind_edges].vtx_1 = ind_vtx;
			fdf->edges[ind_edges].vtx_2 = ind_vtx + 1;
			ind_edges++;
		}
		if (ind_vtx + fdf->nb_cols < fdf->nb_vertices)
		{
			fdf->edges[ind_edges].vtx_1 = ind_vtx;
			fdf->edges[ind_edges].vtx_2 = ind_vtx + fdf->nb_cols;
			ind_edges++;
		}
	}
	return (0);
}

static void	compute_shifts(t_fdf *fdf)
{
	double	limit1;
	double	limit2;

	if (fdf->nb_cols == 1)
		fdf->shift_x = IMG_WDTH / 2;
	else
	{
		limit1 = -cos((double)(M_PI / 6)) * fdf->nb_rows * fdf->base_scale;
		limit2 = cos((double)(M_PI / 6)) * fdf->nb_cols * fdf->base_scale;
		fdf->shift_x = (int)(-limit1) + (IMG_WDTH - (int)(limit2 - limit1)) / 2;
	}
	if (fdf->nb_rows == 1)
		fdf->shift_y = IMG_HGHT / 2;
	else
	{
		limit1 = 0;
		limit2 = sin((double)(M_PI / 6)) * (fdf->nb_cols + fdf->nb_rows) * fdf->base_scale;
		fdf->shift_y = (IMG_HGHT - (int)(limit2 - limit1)) / 2;
	}
//	fdf->shift_x = -((int)(-cos((double)(M_PI / 6)) * fdf->nb_rows * fdf->base_scale)) + X_SHIFT;
//	fdf->shift_y = Y_SHIFT;
}

static void	compute_base_scale(t_fdf *fdf)
{
	int 	scale_x;
	int 	scale_y;
	double	limit1;
	double	limit2;

	if (fdf->nb_cols == 1)
		scale_x = 0xFFFFFFFF;
	else
	{
		limit1 = -cos((double)(M_PI / 6)) * fdf->nb_rows;
		limit2 = cos((double)(M_PI / 6)) * fdf->nb_cols;
		scale_x = (IMG_WDTH - 2 * X_SHIFT) / (int)(limit2 - limit1);
	}
	if (fdf->nb_rows == 1)
		scale_y = 0xFFFFFFFF;
	else
	{
		limit1 = 0;
		limit2 = sin((double)(M_PI / 6)) * (fdf->nb_cols + fdf->nb_rows);
		scale_y = (IMG_HGHT - 2 * Y_SHIFT) / (int)(limit2 - limit1);
	}
	fdf->base_scale = (scale_x > scale_y ? scale_y : scale_x);
}

int			init_fdf(t_fdf *fdf)
{
	if ((init_mlx(fdf)))
		return (1);
	if ((init_edges_tab(fdf)))
		return (1);
	if (!(fdf->vtcs_2d = (t_vertex *)malloc(sizeof(t_vertex) * fdf->nb_vertices)))
		return ((print_sys_error(errno)));
	compute_base_scale(fdf);
	compute_shifts(fdf);
	fdf->wireframe_col.red = 0xFF;
	fdf->wireframe_col.green = 0xFF;
	fdf->wireframe_col.blue = 0xFF;
	fdf->mods.scale_coef = 1;
	fdf->mods.alt_mod = 1;
	fdf->mods.col_updt = 1;
	reinit_matrices(fdf);
	return (0);
}
