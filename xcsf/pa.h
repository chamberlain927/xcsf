/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
                 
/**
 * @file pa.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2019.
 * @brief Prediction array functions.
 */ 
 
#pragma once

void pa_build(XCSF *xcsf, SET *set, double *x);
double pa_best_val(XCSF *xcsf);
double pa_val(XCSF *xcsf, int act);   
int pa_best_action(XCSF *xcsf);
int pa_rand_action(XCSF *xcsf);
void pa_init(XCSF *xcsf);
void pa_free(XCSF *xcsf);
