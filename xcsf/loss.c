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
 * @file loss.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2019--2020.
 * @brief Loss functions for calculating prediction error.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <float.h>
#include <errno.h>
#include "xcsf.h"
#include "loss.h"

/**
 * @brief Mean absolute error loss function.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The mean absolute error.
 */
double loss_mae(const XCSF *xcsf, const double *pred, const double *y)
{
    double error = 0;
    for(int i = 0; i < xcsf->y_dim; i++) {
        error += fabs(y[i] - pred[i]);
    }
    error /= xcsf->y_dim;
    return error;
}
 
/**
 * @brief Mean squared error loss function.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The mean squared error.
 */
double loss_mse(const XCSF *xcsf, const double *pred, const double *y)
{
    double error = 0;
    for(int i = 0; i < xcsf->y_dim; i++) {
        error += (y[i] - pred[i]) * (y[i] - pred[i]);
    }
    error /= xcsf->y_dim;
    return error;
}

/**
 * @brief Root mean squared error loss function.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The root mean squared error.
 */
double loss_rmse(const XCSF *xcsf, const double *pred, const double *y)
{
    double error = loss_mse(xcsf, pred, y);
    return sqrt(error);
}

/**
 * @brief Logistic log loss for multi-class classification.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The log error.
 *
 * @details Assumes the sum of predictions = 1 and a single target y_i has a value of 1.
 */
double loss_log(const XCSF *xcsf, const double *pred, const double *y)
{
    double error = 0;
    for(int i = 0; i < xcsf->y_dim; i++) {
        error += y[i] * log(fmax(pred[i], DBL_EPSILON));
    }
    return -error;
}

/**
 * @brief Binary logistic log loss for binary-class classification.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The log error.
 */
double loss_binary_log(const XCSF *xcsf, const double *pred, const double *y)
{
    double error = 0;
    for(int i = 0; i < xcsf->y_dim; i++) {
        error += y[i] * log(fmax(pred[i], DBL_EPSILON)) +
            (1-y[i]) * log(fmax((1-pred[i]), DBL_EPSILON));
    }
    return -error;
}
 
/**
 * @brief One-hot classification error.
 * @param xcsf The XCSF data structure.
 * @param pred The predicted values.
 * @param y The true values.
 * @return The one-hot classification error.
 */
double loss_onehot_acc(const XCSF *xcsf, const double *pred, const double *y)
{
    int p = 0;
    for(int i = 1; i < xcsf->y_dim; i++) {
        if(pred[i] > pred[p]) {
            p = i;
        }
    }
    if(y[p] != 1) {
        return 1;
    }
    return 0;
}
 
/**
 * @brief Sets the XCSF error function to the implemented function.
 * @param xcsf The XCSF data structure.
 */
void loss_set_func(XCSF *xcsf)
{
    switch(xcsf->LOSS_FUNC) {
        case LOSS_MAE: xcsf->loss_ptr = &loss_mae; break;
        case LOSS_MSE: xcsf->loss_ptr = &loss_mse; break;
        case LOSS_RMSE: xcsf->loss_ptr = &loss_rmse; break;
        case LOSS_LOG: xcsf->loss_ptr = &loss_log; break;
        case LOSS_BINARY_LOG: xcsf->loss_ptr = &loss_binary_log; break;
        case LOSS_ONEHOT_ACC: xcsf->loss_ptr = &loss_onehot_acc; break;
        default:
            printf("invalid loss function: %d\n", xcsf->LOSS_FUNC);
            exit(EXIT_FAILURE);
    }
}
