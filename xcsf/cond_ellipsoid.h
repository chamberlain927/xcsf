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
 * @file cond_ellipsoid.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2019--2020.
 * @brief Hyperellipsoid condition functions.
 */ 

#pragma once

/**
 * @brief Hyperellipsoid condition data structure.
 */ 
typedef struct COND_ELLIPSOID {
    double *center; //!< Centers
    double *spread; //!< Spreads
    double *mu; //!< Mutation rates
} COND_ELLIPSOID;

_Bool cond_ellipsoid_crossover(const XCSF *xcsf, const CL *c1, const CL *c2);
_Bool cond_ellipsoid_general(const XCSF *xcsf, const CL *c1, const CL *c2);
_Bool cond_ellipsoid_match(const XCSF *xcsf, const CL *c, const double *x);
_Bool cond_ellipsoid_mutate(const XCSF *xcsf, const CL *c);
void cond_ellipsoid_copy(const XCSF *xcsf, CL *dest, const CL *src);
void cond_ellipsoid_cover(const XCSF *xcsf, const CL *c, const double *x);
void cond_ellipsoid_free(const XCSF *xcsf, const CL *c);
void cond_ellipsoid_init(const XCSF *xcsf, CL *c);
void cond_ellipsoid_print(const XCSF *xcsf, const CL *c);
void cond_ellipsoid_update(const XCSF *xcsf, const CL *c, const double *x, const double *y);
int cond_ellipsoid_size(const XCSF *xcsf, const CL *c);
size_t cond_ellipsoid_save(const XCSF *xcsf, const CL *c, FILE *fp);
size_t cond_ellipsoid_load(const XCSF *xcsf, CL *c, FILE *fp);

/**
 * @brief Hyperellipsoid condition implemented functions.
 */
static struct CondVtbl const cond_ellipsoid_vtbl = {
    &cond_ellipsoid_crossover,
    &cond_ellipsoid_general,
    &cond_ellipsoid_match,
    &cond_ellipsoid_mutate,
    &cond_ellipsoid_copy,
    &cond_ellipsoid_cover,
    &cond_ellipsoid_free,
    &cond_ellipsoid_init,
    &cond_ellipsoid_print,
    &cond_ellipsoid_update,
    &cond_ellipsoid_size,
    &cond_ellipsoid_save,
    &cond_ellipsoid_load
};      
