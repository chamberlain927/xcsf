/*
 * Copyright (C) 2016--2019 Richard Preen <rpreen@gmail.com>
 *
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
 *
 **************
 * Description: 
 **************
 * The neural classifier condition module.
 *
 * Provides functionality to create MLP neural networks that compute whether
 * the classifier matches for a given problem instance. Includes operations for
 * covering, matching, copying, mutating, printing, etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "xcsf.h"
#include "utils.h"
#include "cl.h"
#include "neural_activations.h"
#include "neural.h"
#include "neural_layer.h"
#include "neural_layer_connected.h"
#include "neural_layer_dropout.h"
#include "condition.h"
#include "cond_neural.h"

typedef struct COND_NEURAL {
    BPN bpn;
} COND_NEURAL;

void cond_neural_rand(XCSF *xcsf, CL *c);

void cond_neural_init(XCSF *xcsf, CL *c)
{
    COND_NEURAL *new = malloc(sizeof(COND_NEURAL));
    // initialise empty network
    neural_init(xcsf, &new->bpn);
    // create and add layers to the network
    neural_layer_connected_init(xcsf, &new->bpn, 
            xcsf->num_x_vars, xcsf->NUM_HIDDEN_NEURONS, xcsf->HIDDEN_NEURON_ACTIVATION);
    neural_layer_connected_init(xcsf, &new->bpn, xcsf->NUM_HIDDEN_NEURONS, 1, IDENTITY);
    // initialise all weights randomly
    neural_rand(xcsf, &new->bpn);
    c->cond = new;
}

void cond_neural_free(XCSF *xcsf, CL *c)
{
    COND_NEURAL *cond = c->cond;
    neural_free(xcsf, &cond->bpn);
    free(c->cond);
}                  

void cond_neural_copy(XCSF *xcsf, CL *to, CL *from)
{
    COND_NEURAL *new = malloc(sizeof(COND_NEURAL));
    COND_NEURAL *from_cond = from->cond;
    neural_copy(xcsf, &new->bpn, &from_cond->bpn);
    to->cond = new;
}

void cond_neural_rand(XCSF *xcsf, CL *c)
{
    COND_NEURAL *cond = c->cond;
    neural_rand(xcsf, &cond->bpn);
}

void cond_neural_cover(XCSF *xcsf, CL *c, double *x)
{
    do {
        cond_neural_rand(xcsf, c);
    } while(!cond_neural_match(xcsf, c, x));
}
 
void cond_neural_update(XCSF *xcsf, CL *c, double *x, double *y)
{
    (void)xcsf; (void)c; (void)x; (void)y;
}
 
_Bool cond_neural_match(XCSF *xcsf, CL *c, double *x)
{
    COND_NEURAL *cond = c->cond;
    neural_propagate(xcsf, &cond->bpn, x);
    if(neural_output(xcsf, &cond->bpn, 0) > 0.5) {
        c->m = true;
    }
    else {
        c->m = false;
    }
    return c->m;
}                

_Bool cond_neural_mutate(XCSF *xcsf, CL *c)
{
    COND_NEURAL *cond = c->cond;
    return neural_mutate(xcsf, &cond->bpn);
}

_Bool cond_neural_crossover(XCSF *xcsf, CL *c1, CL *c2)
{
    (void)xcsf; (void)c1; (void)c2;
    return false;
}

_Bool cond_neural_general(XCSF *xcsf, CL *c1, CL *c2)
{
    (void)xcsf; (void)c1; (void)c2;
    return false;
}   

void cond_neural_print(XCSF *xcsf, CL *c)
{
    COND_NEURAL *cond = c->cond;
    neural_print(xcsf, &cond->bpn, true);
}
