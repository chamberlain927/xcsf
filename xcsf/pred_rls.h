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
 * @file pred_rls.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2019.
 * @brief Recursive least mean squares prediction functions.
 */ 

#pragma once

double *pred_rls_compute(XCSF *xcsf, CL *c, double *x);
_Bool pred_rls_crossover(XCSF *xcsf, CL *c1, CL *c2);
_Bool pred_rls_mutate(XCSF *xcsf, CL *c);
void pred_rls_copy(XCSF *xcsf, CL *to,  CL *from);
void pred_rls_free(XCSF *xcsf, CL *c);
void pred_rls_init(XCSF *xcsf, CL *c);
void pred_rls_print(XCSF *xcsf, CL *c);
void pred_rls_update(XCSF *xcsf, CL *c, double *x, double *y);
int pred_rls_size(XCSF *xcsf, CL *c);
size_t pred_rls_save(XCSF *xcsf, CL *c, FILE *fp);
size_t pred_rls_load(XCSF *xcsf, CL *c, FILE *fp);

/**
 * @brief Recursive least mean squares prediction implemented functions.
 */
static struct PredVtbl const pred_rls_vtbl = {
    &pred_rls_crossover,
    &pred_rls_mutate,
    &pred_rls_compute,
    &pred_rls_copy,
    &pred_rls_free,
    &pred_rls_init,
    &pred_rls_print,
    &pred_rls_update,
    &pred_rls_size,
    &pred_rls_save,
    &pred_rls_load
};
