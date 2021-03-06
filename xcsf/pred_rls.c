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
 * @file pred_rls.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2020.
 * @brief Recursive least mean squares prediction functions.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "xcsf.h"
#include "utils.h"
#include "blas.h"
#include "cl.h"
#include "prediction.h"
#include "pred_rls.h"

void pred_rls_init(const XCSF *xcsf, CL *c)
{
    PRED_RLS *pred = malloc(sizeof(PRED_RLS));
    c->pred = pred;
    // set the length of weights per predicted variable
    if(xcsf->PRED_TYPE == PRED_TYPE_RLS_QUADRATIC) {
        // offset(1) + n linear + n quadratic + n*(n-1)/2 mixed terms
        pred->n = 1 + 2 * xcsf->x_dim + xcsf->x_dim * (xcsf->x_dim - 1) / 2;
    }
    else {
        pred->n = xcsf->x_dim + 1;
    }
    // initialise weights
    pred->n_weights = pred->n * xcsf->y_dim;
    pred->weights = calloc(pred->n_weights, sizeof(double));
    blas_fill(xcsf->y_dim, xcsf->PRED_X0, pred->weights, pred->n);
    // initialise gain matrix
    int n_sqrd = pred->n * pred->n;
    pred->matrix = calloc(n_sqrd, sizeof(double));
    for(int i = 0; i < pred->n; i++) {
        pred->matrix[i * pred->n + i] = xcsf->PRED_RLS_SCALE_FACTOR;
    }
    // initialise temporary storage for weight updating
    pred->tmp_input = malloc(pred->n * sizeof(double));
    pred->tmp_vec = calloc(pred->n, sizeof(double));
    pred->tmp_matrix1 = calloc(n_sqrd, sizeof(double));
    pred->tmp_matrix2 = calloc(n_sqrd, sizeof(double));
}

void pred_rls_copy(const XCSF *xcsf, CL *dest, const CL *src)
{
    pred_rls_init(xcsf, dest);
    const PRED_RLS *dest_pred = dest->pred;
    const PRED_RLS *src_pred = src->pred;
    memcpy(dest_pred->weights, src_pred->weights, src_pred->n_weights * sizeof(double));
}

void pred_rls_free(const XCSF *xcsf, const CL *c)
{
    (void)xcsf;
    PRED_RLS *pred = c->pred;
    free(pred->weights);
    free(pred->matrix);
    free(pred->tmp_input);
    free(pred->tmp_vec);
    free(pred->tmp_matrix1);
    free(pred->tmp_matrix2);
    free(pred);
}

void pred_rls_update(const XCSF *xcsf, const CL *c, const double *x, const double *y)
{
    (void)x;
    const PRED_RLS *pred = c->pred;
    int n = pred->n;
    // gain vector = matrix * tmp_input
    blas_gemm(0, 0, n, 1, n, 1, pred->matrix, n, pred->tmp_input, 1, 0, pred->tmp_vec, 1);
    // divide gain vector by lambda + tmp_vec
    double divisor = blas_dot(n, pred->tmp_input, 1, pred->tmp_vec, 1);
    divisor = 1 / (divisor + xcsf->PRED_RLS_LAMBDA);
    blas_scal(n, divisor, pred->tmp_vec, 1);
    // update weights using the error
    for(int var = 0; var < xcsf->y_dim; var++) {
        double error = y[var] - c->prediction[var];
        blas_axpy(n, error, pred->tmp_vec, 1, &pred->weights[var*n], 1);
    }
    // update gain matrix
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            double tmp = pred->tmp_vec[i] * pred->tmp_input[j];
            if(i == j) {
                pred->tmp_matrix1[i*n+j] = 1 - tmp;
            }
            else {
                pred->tmp_matrix1[i*n+j] = -tmp;
            }
        }
    }
    // tmp_matrix2 = tmp_matrix1 * pred_matrix
    blas_gemm(0, 0, n, n, n, 1, pred->tmp_matrix1, n, pred->matrix, n, 0, pred->tmp_matrix2, n);
    // divide gain matrix entries by lambda
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            pred->matrix[i*n+j] = pred->tmp_matrix2[i*n+j] / xcsf->PRED_RLS_LAMBDA;
        }
    }
}

void pred_rls_compute(const XCSF *xcsf, const CL *c, const double *x)
{
    const PRED_RLS *pred = c->pred;
    int n = pred->n;
    pred_transform_input(xcsf, x, pred->tmp_input);
    for(int var = 0; var < xcsf->y_dim; var++) {
        c->prediction[var] = blas_dot(n, &pred->weights[var*n], 1, pred->tmp_input, 1);
    }
} 

void pred_rls_print(const XCSF *xcsf, const CL *c)
{
    const PRED_RLS *pred = c->pred;
    printf("RLS weights: ");
    int n = pred->n;
    for(int var = 0; var < xcsf->y_dim; var++) {
        for(int i = 0; i < n; i++) {
            printf("%f, ", pred->weights[var*n+i]);
        }
        printf("\n");
    }
}

_Bool pred_rls_crossover(const XCSF *xcsf, const CL *c1, const CL *c2)
{
    (void)xcsf; (void)c1; (void)c2;
    return false;
}

_Bool pred_rls_mutate(const XCSF *xcsf, const CL *c)
{
    (void)xcsf; (void)c;
    return false;
}

int pred_rls_size(const XCSF *xcsf, const CL *c)
{
    (void)xcsf;
    const PRED_RLS *pred = c->pred;
    return pred->n_weights;
}

size_t pred_rls_save(const XCSF *xcsf, const CL *c, FILE *fp)
{
    (void)xcsf;
    const PRED_RLS *pred = c->pred;
    size_t s = 0;
    s += fwrite(&pred->n, sizeof(int), 1, fp);
    s += fwrite(&pred->n_weights, sizeof(int), 1, fp);
    s += fwrite(pred->weights, sizeof(double), pred->n_weights, fp);
    int n_sqrd = pred->n * pred->n;
    s += fwrite(pred->matrix, sizeof(double), n_sqrd, fp);
    return s;
}

size_t pred_rls_load(const XCSF *xcsf, CL *c, FILE *fp)
{
    pred_rls_init(xcsf, c);
    PRED_RLS *pred = c->pred;
    size_t s = 0;
    s += fread(&pred->n, sizeof(int), 1, fp);
    s += fread(&pred->n_weights, sizeof(int), 1, fp);
    s += fread(pred->weights, sizeof(double), pred->n_weights, fp);
    int n_sqrd = pred->n * pred->n;
    s += fread(pred->matrix, sizeof(double), n_sqrd, fp);
    return s;
}
