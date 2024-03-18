/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_casting.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smonte-e <smonte-e@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 06:42:02 by smonte-e          #+#    #+#             */
/*   Updated: 2024/03/15 19:05:46 by smonte-e         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

t_raycast	initialize_raycast(t_cube *cube, int col)
{
	t_raycast	ray;

	ray.col = col;
	ray.ra = (cube->map->player->p_pos_a - 90) * M_PI / 180;
	ray.ratio = ((col * 2) / (double)X_RES) - 1;
	ray.dir.x = (cos(ray.ra + M_PI_2) * ray.ratio) + cos(ray.ra);
	ray.dir.y = (sin(ray.ra + M_PI_2) * ray.ratio) + sin(ray.ra);
	ray.map.x = floor(cube->map->player->p_pos_x / MINI_SCALE);
	ray.map.y = floor(cube->map->player->p_pos_y / MINI_SCALE);
	ray.delta.x = fabs(1 / ray.dir.x);
	ray.delta.y = fabs(1 / ray.dir.y);
	if (ray.dir.x < 0)
		ray.sided.x = ((cube->map->player->p_pos_x / (double)MINI_SCALE
					- ray.map.x) * ray.delta.x);
	else
		ray.sided.x = ((ray.map.x + 1.0 - cube->map->player->p_pos_x
					/ (double)MINI_SCALE) * ray.delta.x);
	if (ray.dir.y < 0)
		ray.sided.y = ((cube->map->player->p_pos_y / (double)MINI_SCALE
					- ray.map.y) * ray.delta.y);
	else
		ray.sided.y = ((ray.map.y + 1.0 - cube->map->player->p_pos_y
					/ (double)MINI_SCALE) * ray.delta.y);
	return (ray);
}

void	perform_dda(t_cube *cube, t_raycast *ray)
{
	while (!cube->ray->hit)
	{
		if (ray->sided.x < ray->sided.y)
		{
			ray->sided.x += ray->delta.x;
			if (ray->dir.x < 0)
				ray->map.x += -1;
			else
				ray->map.x += 1;
			ray->side = 0;
		}
		else
		{
			ray->sided.y += ray->delta.y;
			if (ray->dir.y < 0)
				ray->map.y += -1;
			else
				ray->map.y += 1;
			ray->side = 1;
		}
		if (cube->map->m_mini_map[(int)ray->map.y][(int)ray->map.x] == '1')
			cube->ray->hit = 1;
		else if (cube->map->m_mini_map[(int)ray->map.y][(int)ray->map.x] == 'D')
			cube->ray->hit = 2;
		else if (cube->map->m_mini_map[(int)ray->map.y][(int)ray->map.x] == 'x')
			cube->ray->hit = 3;
	}
}

void	set_wall_parameters(t_raycast *ray, t_cube *cube, double camera_height)
{
	double	wall_height;
	double	wall_center;

	if (ray->side == 0)
		ray->p_walld = (ray->sided.x - ray->delta.x);
	else
		ray->p_walld = (ray->sided.y - ray->delta.y);
	wall_height = (Y_RES) / ray->p_walld;
	wall_center = ((Y_RES - HUD) / 2) - cube->map->player->offset;
	ray->wall_top = wall_center - (wall_height / 2) - (camera_height
			- cube->map->player->offset);
	ray->wall_bottom = wall_center + (wall_height / 2) - (camera_height
			- cube->map->player->offset);
	if (ray->wall_top < 0)
		ray->wall_top = 0;
	if (ray->wall_bottom >= Y_RES - HUD)
		ray->wall_bottom = Y_RES - HUD - 1;
}

void	render_wall(t_cube *cube, t_raycast *ray)
{
	double	camera_height;

	camera_height = cube->map->player->offset;
	set_wall_parameters(ray, cube, camera_height);
	cube->ray = ray;
	draw_textures(cube, (t_vec){ray->col, ray->wall_top}, (t_vec){ray->col,
		ray->wall_bottom}, 0);
}

void	draw_wall(t_cube *cube)
{
	t_raycast	ray;

	ray.col = 0;
	while (ray.col < X_RES)
	{
		ray = initialize_raycast(cube, ray.col);
		cube->ray->hit = 0;
		perform_dda(cube, &ray);
		render_wall(cube, &ray);
		ray.col++;
	}
}
