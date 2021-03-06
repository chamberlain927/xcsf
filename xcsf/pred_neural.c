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
 * @file pred_neural.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2016--2020.
 * @brief Multi-layer perceptron neural network prediction functions.
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "xcsf.h"
#include "utils.h"
#include "cl.h"
#include "neural_activations.h"
#include "neural.h"
#include "neural_layer.h"
#include "neural_layer_connected.h"
#include "neural_layer_dropout.h"
#include "neural_layer_noise.h"
#include "neural_layer_softmax.h"
#include "prediction.h"
#include "pred_neural.h"

static uint32_t pred_neural_lopt(const XCSF *xcsf);

void pred_neural_init(const XCSF *xcsf, CL *c)
{
    PRED_NEURAL *new = malloc(sizeof(PRED_NEURAL));
    neural_init(xcsf, &new->net);
    // hidden layers
    uint32_t lopt = pred_neural_lopt(xcsf);
    LAYER *l;
    int i = 0;
    int n_inputs = xcsf->x_dim;
    while(i < MAX_LAYERS && xcsf->PRED_NUM_NEURONS[i] > 0) {
        int hinit = xcsf->PRED_NUM_NEURONS[i];
        int hmax = xcsf->PRED_MAX_NEURONS[i];
        if(hmax < hinit || !xcsf->PRED_EVOLVE_NEURONS) {
            hmax = hinit;
        }
        int f = xcsf->PRED_HIDDEN_ACTIVATION;
        l = neural_layer_connected_init(xcsf, n_inputs, hinit, hmax, f, lopt);
        neural_layer_insert(xcsf, &new->net, l, i);
        n_inputs = hinit;
        i++;
    }
    // output layer
    int f = xcsf->PRED_OUTPUT_ACTIVATION;
    lopt &= ~LAYER_EVOLVE_NEURONS; // never evolve the number of output neurons
    l = neural_layer_connected_init(xcsf, n_inputs, xcsf->y_dim, xcsf->y_dim, f, lopt);
    neural_layer_insert(xcsf, &new->net, l, i);
    c->pred = new;
}

static uint32_t pred_neural_lopt(const XCSF *xcsf)
{
    uint32_t lopt = 0;
    if(xcsf->PRED_EVOLVE_ETA) {
        lopt |= LAYER_EVOLVE_ETA;
    }
    if(xcsf->PRED_SGD_WEIGHTS) {
        lopt |= LAYER_SGD_WEIGHTS;
    }
    if(xcsf->PRED_EVOLVE_WEIGHTS) {
        lopt |= LAYER_EVOLVE_WEIGHTS;
    }
    if(xcsf->PRED_EVOLVE_NEURONS) {
        lopt |= LAYER_EVOLVE_NEURONS;
    }
    if(xcsf->PRED_EVOLVE_FUNCTIONS) {
        lopt |= LAYER_EVOLVE_FUNCTIONS;
    }
    return lopt;
}

void pred_neural_free(const XCSF *xcsf, const CL *c)
{
    PRED_NEURAL *pred = c->pred;
    neural_free(xcsf, &pred->net);
    free(pred);
}

void pred_neural_copy(const XCSF *xcsf, CL *dest, const CL *src)
{
    PRED_NEURAL *new = malloc(sizeof(PRED_NEURAL));
    const PRED_NEURAL *src_pred = src->pred;
    neural_copy(xcsf, &new->net, &src_pred->net);
    dest->pred = new;
}

void pred_neural_update(const XCSF *xcsf, const CL *c, const double *x, const double *y)
{
    if(xcsf->PRED_SGD_WEIGHTS) {
        PRED_NEURAL *pred = c->pred;
        neural_learn(xcsf, &pred->net, y, x);
    }
}

void pred_neural_compute(const XCSF *xcsf, const CL *c, const double *x)
{
    const PRED_NEURAL *pred = c->pred;
    neural_propagate(xcsf, &pred->net, x);
    for(int i = 0; i < xcsf->y_dim; i++) {
        c->prediction[i] = neural_output(xcsf, &pred->net, i);
    }
}

void pred_neural_print(const XCSF *xcsf, const CL *c)
{
    const PRED_NEURAL *pred = c->pred;
    neural_print(xcsf, &pred->net, false);
}  

_Bool pred_neural_crossover(const XCSF *xcsf, const CL *c1, const CL *c2)
{
    (void)xcsf; (void) c1; (void)c2;
    return false;
}

_Bool pred_neural_mutate(const XCSF *xcsf, const CL *c)
{
    const PRED_NEURAL *pred = c->pred;
    return neural_mutate(xcsf, &pred->net);
}

int pred_neural_size(const XCSF *xcsf, const CL *c)
{
    const PRED_NEURAL *pred = c->pred;
    return neural_size(xcsf, &pred->net);
}

size_t pred_neural_save(const XCSF *xcsf, const CL *c, FILE *fp)
{
    const PRED_NEURAL *pred = c->pred;
    size_t s = neural_save(xcsf, &pred->net, fp);
    return s;
}

size_t pred_neural_load(const XCSF *xcsf, CL *c, FILE *fp)
{
    PRED_NEURAL *new = malloc(sizeof(PRED_NEURAL));
    size_t s = neural_load(xcsf, &new->net, fp);
    c->pred = new;
    return s;
}

double pred_neural_eta(const XCSF *xcsf, const CL *c, int layer)
{
    (void)xcsf;
    const PRED_NEURAL *pred = c->pred;
    int i = 0;
    for(const LLIST *iter = pred->net.tail; iter != NULL; iter = iter->prev) {
        if(i == layer) {
            return iter->layer->eta;
        }
        i++;
    }
    return 0;
}

int pred_neural_neurons(const XCSF *xcsf, const CL *c, int layer)
{
    (void)xcsf;
    const PRED_NEURAL *pred = c->pred;
    const NET *net = &pred->net;
    int i = 0;
    for(const LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        if(i == layer) {
            return iter->layer->n_outputs;
        }
        i++;
    }
    return 0;
}

int pred_neural_layers(const XCSF *xcsf, const CL *c)
{
    (void)xcsf;
    const PRED_NEURAL *pred = c->pred;
    const NET *net = &pred->net;
    return net->n_layers;
}

void pred_neural_expand(const XCSF *xcsf, const CL *c)
{
    PRED_NEURAL *pred = c->pred;
    NET *net = &pred->net;
    // insert a new layer
    int size = net->head->layer->n_inputs;
    int max = net->head->next->layer->max_outputs;
    int pos = net->n_layers - 1;
    int f = xcsf->PRED_HIDDEN_ACTIVATION;
    uint32_t lopt = pred_neural_lopt(xcsf);
    // initially same size as last hidden layer
    LAYER *l = neural_layer_connected_init(xcsf, size, size, max, f, lopt);
    neural_layer_insert(xcsf, net, l, pos);
    // resize layers as necessary
    neural_resize(xcsf, net);
}
