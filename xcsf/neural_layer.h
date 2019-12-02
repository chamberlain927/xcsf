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
 * @file neural_layer.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2016--2019.
 * @brief Interface for neural network layers.
 */ 

#pragma once

#include <stdint.h>

#define CONNECTED 0
#define DROPOUT 1
#define NOISE 2
#define SOFTMAX 3

#define LAYER_EVOLVE_WEIGHTS    (1<<0)
#define LAYER_EVOLVE_NEURONS    (1<<1)
#define LAYER_EVOLVE_ETA        (1<<2)
#define LAYER_EVOLVE_FUNCTIONS  (1<<3)
#define LAYER_SGD_WEIGHTS       (1<<4)

/**
 * @brief Neural network layer data structure.
 */ 
typedef struct LAYER {
    int layer_type; //!< Layer type: CONNECTED, DROPOUT, etc.
    double *state; //!< Current neuron states (before activation function)
    double *output; //!< Current neuron outputs (after activation function)
    _Bool *active; //!< Whether a neuron is active
    int num_active; //!< Number of active neurons
    uint32_t options; //!< Bitwise layer options permitting weight evolution, etc.
    double *weights; //!< Weights for calculating neuron states
    double *biases; //!< Biases for calculating neuron states
    double *bias_updates; //!< Updates to biases
    double *weight_updates; //!< Updates to weights
    double *delta; //!< Delta for updating weights
    int num_inputs; //!< Number of layer inputs
    int num_outputs; //!< Number of layer outputs
    int num_weights; //!< Number of layer weights
    int function; //!< Layer activation function
    double eta; //!< Gradient descent learning rate
    double scale; //!< Usage depends on layer implementation
    double probability; //!< Usage depends on layer implementation
    double *rand; //!< Usage depends on layer implementation
    double temp; //!< Usage depends on layer implementation
    struct LayerVtbl const *layer_vptr; //!< Functions acting on layers
} LAYER;

/**
 * @brief Neural network layer interface data structure.
 * @details Neural network layer implementations must implement these functions.
 */ 
struct LayerVtbl {
    _Bool (*layer_impl_mutate)(XCSF *xcsf, LAYER *l);
    LAYER* (*layer_impl_copy)(XCSF *xcsf, LAYER *from);
    void (*layer_impl_free)(XCSF *xcsf, LAYER *l);
    void (*layer_impl_rand)(XCSF *xcsf, LAYER *l);
    void (*layer_impl_print)(XCSF *xcsf, LAYER *l, _Bool print_weights);
    void (*layer_impl_update)(XCSF *xcsf, LAYER *l);
    void (*layer_impl_backward)(XCSF *xcsf, LAYER *l, NET *net);
    void (*layer_impl_forward)(XCSF *xcsf, LAYER *l, double *input);
    double* (*layer_impl_output)(XCSF *xcsf, LAYER *l);
    size_t (*layer_impl_save)(XCSF *xcsf, LAYER *l, FILE *fp);
    size_t (*layer_impl_load)(XCSF *xcsf, LAYER *l, FILE *fp);
};

static inline size_t layer_save(XCSF *xcsf, LAYER *l, FILE *fp) {
    return (*l->layer_vptr->layer_impl_save)(xcsf, l, fp);
}

static inline size_t layer_load(XCSF *xcsf, LAYER *l, FILE *fp) {
    return (*l->layer_vptr->layer_impl_load)(xcsf, l, fp);
}

static inline double* layer_output(XCSF *xcsf, LAYER *l) {
    return (*l->layer_vptr->layer_impl_output)(xcsf, l);
}

static inline void layer_forward(XCSF *xcsf, LAYER *l, double *input) {
    (*l->layer_vptr->layer_impl_forward)(xcsf, l, input);
}

static inline void layer_backward(XCSF *xcsf, LAYER *l, NET *net) {
    (*l->layer_vptr->layer_impl_backward)(xcsf, l, net);
}

static inline void layer_update(XCSF *xcsf, LAYER *l) {
    (*l->layer_vptr->layer_impl_update)(xcsf, l);
}

static inline _Bool layer_mutate(XCSF *xcsf, LAYER *l) {
    return (*l->layer_vptr->layer_impl_mutate)(xcsf, l);
}

static inline LAYER* layer_copy(XCSF *xcsf, LAYER *from) {
    return (*from->layer_vptr->layer_impl_copy)(xcsf, from);
}

static inline void layer_free(XCSF *xcsf, LAYER *l) {
    (*l->layer_vptr->layer_impl_free)(xcsf, l);
}

static inline void layer_rand(XCSF *xcsf, LAYER *l) {
    (*l->layer_vptr->layer_impl_rand)(xcsf, l);
}

static inline void layer_print(XCSF *xcsf, LAYER *l, _Bool print_weights) {
    (*l->layer_vptr->layer_impl_print)(xcsf, l, print_weights);
}

void neural_layer_set_vptr(LAYER *l);
