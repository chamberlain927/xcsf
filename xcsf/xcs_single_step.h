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
 * @file xcs_single_step.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2019.
 * @brief Single-step reinforcement learning functions.
 */ 
 
#pragma once

double xcs_single_error(XCSF *xcsf, double reward);
double xcs_single_step_exp(XCSF *xcsf);
int xcs_single_decision(XCSF *xcsf, SET *mset, SET *kset, double *x);
void xcs_single_update(XCSF *xcsf, SET *mset, SET *aset, SET *kset, double *x, int a, double r);