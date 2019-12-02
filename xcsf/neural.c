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
 * @file neural.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2012--2019.
 * @brief An implementation of a multi-layer perceptron neural network.
 */ 
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "xcsf.h"
#include "utils.h"
#include "neural_activations.h"
#include "neural.h"
#include "neural_layer.h"
#include "neural_layer_connected.h"
#include "neural_layer_dropout.h"
#include "neural_layer_noise.h"
#include "neural_layer_softmax.h"

/**
 * @brief Initialises an empty neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to initialise.
 */
void neural_init(XCSF *xcsf, NET *net)
{
    (void)xcsf;
    net->head = NULL;
    net->tail = NULL;
    net->num_layers = 0;
    net->num_inputs = 0;
    net->num_outputs = 0;
}

/**
 * @brief Inserts a layer into a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network receiving the layer.
 * @param l The layer to insert.
 * @param p The position in the network to insert the layer.
 */
void neural_layer_insert(XCSF *xcsf, NET *net, LAYER *l, int p)
{
    (void)xcsf;
    // empty list
    if(net->head == NULL) {
        net->head = malloc(sizeof(LLIST));
        net->head->layer = l;
        net->head->prev = NULL;
        net->head->next = NULL;
        net->tail = net->head;
        net->num_inputs = l->num_inputs;
        net->num_outputs = l->num_outputs;
    } 
    // insert at head
    else if(p >= net->num_layers) {
        LLIST *new = malloc(sizeof(LLIST));
        new->layer = l;
        new->next = net->head;
        new->prev = NULL;
        net->head->prev = new;
        net->head = new;
        net->num_outputs = l->num_outputs;
    }
    // insert before head
    else {
        LLIST *iter = net->tail; 
        for(int i = 0; i < p && iter != NULL; i++) {
            iter = iter->prev;
        }
        LLIST *new = malloc(sizeof(LLIST));
        new->layer = l;
        new->prev = iter;
        new->next = iter->next;
        iter->next = new;
        // new tail
        if(new->next == NULL) {
            net->tail = new;
            net->num_inputs = l->num_inputs;
        }
        else {
            new->next->prev = new;
        }
    }
    net->num_layers++;
}

/**
 * @brief Removes a layer from a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network removing the layer.
 * @param p The position of the layer in the network to be removed.
 */
void neural_layer_remove(XCSF *xcsf, NET *net, int p)
{
    LLIST *iter = net->tail; 
    for(int i = 0; i < p && iter != NULL; i++) {
        iter = iter->prev;
    }
    // head
    if(iter->prev == NULL) {
        net->head = iter->next;
        if(iter->next != NULL) {
            iter->next->prev = NULL;
        }
    }
    // tail
    if(iter->next == NULL) {
        net->tail = iter->prev;
        if(iter->prev != NULL) {
            iter->prev->next = NULL;
        }
    }
    // middle
    if(iter->prev != NULL && iter->next != NULL) {
        iter->next->prev = iter->prev;
        iter->prev->next = iter->next;
    }
    net->num_layers--;
    layer_free(xcsf, iter->layer);
    free(iter->layer);
    free(iter);
}

/**
 * @brief Copies a neural network.
 * @param xcsf The XCSF data structure.
 * @param to The destination neural network.
 * @param from The source neural network.
 */
void neural_copy(XCSF *xcsf, NET *to, NET *from)
{
    neural_init(xcsf, to);
    int p = 0;
    for(LLIST *iter = from->tail; iter != NULL; iter = iter->prev) {
        LAYER *f = iter->layer;
        LAYER *l = layer_copy(xcsf, f);
        neural_layer_insert(xcsf, to, l, p); 
        p++;
    }
}

/**
 * @brief Frees a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to free.
 */
void neural_free(XCSF *xcsf, NET *net)
{
    LLIST *iter = net->tail;
    while(iter != NULL) {
        layer_free(xcsf, iter->layer);
        free(iter->layer);
        net->tail = iter->prev;
        free(iter);
        iter = net->tail;
        net->num_layers--;
    }  
}

/**
 * @brief Randomises the layers within a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to randomise.
 */
void neural_rand(XCSF *xcsf, NET *net)
{
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        layer_rand(xcsf, iter->layer);
    }
}    

/**
 * @brief Mutates a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to mutate.
 * @return Whether any alterations were made.
 */
_Bool neural_mutate(XCSF *xcsf, NET *net)
{
    _Bool mod = false;
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        if(layer_mutate(xcsf, iter->layer)) {
            mod = true;
        }
    }
    return mod;
}

/**
 * @brief Forward propagates a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to propagate.
 * @param input The input state.
 */
void neural_propagate(XCSF *xcsf, NET *net, double *input)
{
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        layer_forward(xcsf, iter->layer, input);
        input = layer_output(xcsf, iter->layer);
    }
}

/**
 * @brief Performs a gradient descent update on a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to be updated.
 * @param truth The desired network output.
 * @param input The input state.
 */
void neural_learn(XCSF *xcsf, NET *net, double *truth, double *input)
{
    /* reset deltas */
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        LAYER *l = iter->layer;
        for(int j = 0; j < l->num_outputs; j++) {
            l->delta[j] = 0;
        }
    }

    // calculate output layer error
    LAYER *p = net->head->layer;
    for(int i = 0; i < p->num_outputs; i++) {
        p->delta[i] = (truth[i] - p->output[i]);
    }

    /* backward phase */
    for(LLIST *iter = net->head; iter != NULL; iter = iter->next) {
        LAYER *l = iter->layer;
        if(iter->next == NULL) {
            net->input = input;
            net->delta = 0;
        }
        else {
            LAYER *prev = iter->next->layer;
            net->input = prev->output;
            net->delta = prev->delta;
        }
        layer_backward(xcsf, l, net);
    }

    /* update phase */
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        layer_update(xcsf, iter->layer);
    }
} 

/**
 * @brief Returns the output of a specified neuron in the output layer of a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to output.
 * @param i Which neuron in the output layer to return.
 * @return The output of the specified neuron.
 */
double neural_output(XCSF *xcsf, NET *net, int i)
{
    if(i < net->num_outputs) {
        double *output = layer_output(xcsf, net->head->layer);
        return constrain(xcsf->MIN_CON, xcsf->MAX_CON, output[i]);
    }
    printf("neural_output(): requested (%d) in output layer of size (%d)\n",
            i, net->num_outputs);
    exit(EXIT_FAILURE);
}

/**
 * @brief Prints a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to print.
 * @param print_weights Whether to print the weights in each layer.
 */
void neural_print(XCSF *xcsf, NET *net, _Bool print_weights)
{
    int i = 0;
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        printf("layer (%d) ", i);
        layer_print(xcsf, iter->layer, print_weights);
        i++;
    }
}

/**
 * @brief Returns the total number of active neurons in a neural network.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to calculate the number of active neurons.
 * @return The total number of active neurons.
 */
int neural_size(XCSF *xcsf, NET *net)
{
    (void)xcsf;
    int size = 0;
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        size += iter->layer->num_active;
    }
    return size;
}

/**
 * @brief Writes a neural network to a binary file.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to save.
 * @param fp Pointer to the file to be written.
 * @return The number of elements written.
 */
size_t neural_save(XCSF *xcsf, NET *net, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&net->num_layers, sizeof(int), 1, fp);
    s += fwrite(&net->num_inputs, sizeof(int), 1, fp);
    s += fwrite(&net->num_outputs, sizeof(int), 1, fp);
    for(LLIST *iter = net->tail; iter != NULL; iter = iter->prev) {
        s += fwrite(&iter->layer->layer_type, sizeof(int), 1, fp);
        s += layer_save(xcsf, iter->layer, fp);
    }
    //printf("neural saved %lu elements\n", (unsigned long)s);
    return s;
}

/**
 * @brief Reads a neural network from a binary file.
 * @param xcsf The XCSF data structure.
 * @param net The neural network to load.
 * @param fp Pointer to the file to be read.
 * @return The number of elements read.
 */
size_t neural_load(XCSF *xcsf, NET *net, FILE *fp)
{
    size_t s = 0;
    int nlayers = 0, ninputs = 0, noutputs = 0;
    s += fread(&nlayers, sizeof(int), 1, fp);
    s += fread(&ninputs, sizeof(int), 1, fp);
    s += fread(&noutputs, sizeof(int), 1, fp);
    neural_init(xcsf, net);
    for(int i = 0; i < nlayers; i++) {
        LAYER *l = malloc(sizeof(LAYER));
        s += fread(&l->layer_type, sizeof(int), 1, fp);
        neural_layer_set_vptr(l);
        s += layer_load(xcsf, l, fp);
        neural_layer_insert(xcsf, net, l, i);
    }
    //printf("neural loaded %lu elements\n", (unsigned long)s);
    return s;
}
