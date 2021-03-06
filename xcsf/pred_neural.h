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
 * @file pred_neural.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2016--2020.
 * @brief Multi-layer perceptron neural network prediction functions.
 */ 

#pragma once

/**
 * @brief Multi-layer perceptron neural network prediction data structure.
 */ 
typedef struct PRED_NEURAL {
    NET net; //!< Neural network
} PRED_NEURAL;

_Bool pred_neural_crossover(const XCSF *xcsf, const CL *c1, const CL *c2);
_Bool pred_neural_mutate(const XCSF *xcsf, const CL *c);
double pred_neural_eta(const XCSF *xcsf, const CL *c, int layer);
int pred_neural_layers(const XCSF *xcsf, const CL *c);
int pred_neural_neurons(const XCSF *xcsf, const CL *c, int layer);
int pred_neural_size(const XCSF *xcsf, const CL *c);
size_t pred_neural_load(const XCSF *xcsf, CL *c, FILE *fp);
size_t pred_neural_save(const XCSF *xcsf, const CL *c, FILE *fp);
void pred_neural_compute(const XCSF *xcsf, const CL *c, const double *x);
void pred_neural_copy(const XCSF *xcsf, CL *dest, const CL *src);
void pred_neural_free(const XCSF *xcsf, const CL *c);
void pred_neural_init(const XCSF *xcsf, CL *c);
void pred_neural_print(const XCSF *xcsf, const CL *c);
void pred_neural_update(const XCSF *xcsf, const CL *c, const double *x, const double *y);
void pred_neural_expand(const XCSF *xcsf, const CL *c);

/**
 * @brief Multi-layer perceptron neural network prediction implemented functions.
 */
static struct PredVtbl const pred_neural_vtbl = {
    &pred_neural_crossover,
    &pred_neural_mutate,
    &pred_neural_compute,
    &pred_neural_copy,
    &pred_neural_free,
    &pred_neural_init,
    &pred_neural_print,
    &pred_neural_update,
    &pred_neural_size,
    &pred_neural_save,
    &pred_neural_load
};
