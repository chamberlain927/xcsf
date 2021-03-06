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
 * @file dgp.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2016--2020.
 * @brief An implementation of dynamical GP graphs with fuzzy activation functions.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "xcsf.h"
#include "utils.h"
#include "sam.h"
#include "dgp.h"

#define NUM_FUNC 3 //!< Number of selectable node functions

static const char *function_string(int function);
static double node_activate(int function, const double *inputs, int k);
static void synchronous_update(const XCSF *xcsf, const GRAPH *dgp, const double *inputs);
static int random_connection(int n_nodes, int n_inputs);

/**
 * @brief Initialises a new DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to initialise.
 * @param n The the number of nodes in the graph.
 */
void graph_init(const XCSF *xcsf, GRAPH *dgp, int n)
{
    dgp->t = 0;
    dgp->n = n;
    dgp->klen = n * xcsf->MAX_K;
    dgp->state = malloc(sizeof(double) * dgp->n);
    dgp->initial_state = malloc(sizeof(double) * dgp->n);
    dgp->tmp_state = malloc(sizeof(double) * dgp->n);
    dgp->tmp_input = malloc(sizeof(double) * xcsf->MAX_K);
    dgp->function = malloc(sizeof(int) * dgp->n);
    dgp->connectivity = malloc(sizeof(int) * dgp->klen);
    sam_init(xcsf, dgp->mu, DGP_N_MU);
}

/**
 * @brief Copies a DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dest The destination DGP graph.
 * @param src The source DGP graph.
 */
void graph_copy(const XCSF *xcsf, GRAPH *dest, const GRAPH *src)
{ 	
    (void)xcsf;
    dest->t = src->t;
    dest->n = src->n;
    dest->klen = src->klen;
    memcpy(dest->state, src->state, sizeof(double) * src->n);
    memcpy(dest->initial_state, src->initial_state, sizeof(double) * src->n);
    memcpy(dest->function, src->function, sizeof(int) * src->n);
    memcpy(dest->connectivity, src->connectivity, sizeof(int) * src->klen);
    memcpy(dest->mu, src->mu, sizeof(double) * DGP_N_MU);
}

/**
 * @brief Returns the current state of a specified node in the graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to output.
 * @param i Which node within the graph to output.
 * @return The current state of the specified node.
 */
double graph_output(const XCSF *xcsf, const GRAPH *dgp, int i)
{
    (void)xcsf;
    return dgp->state[i];
}

/**
 * @brief Resets the states to their initial state.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to reset.
 */
void graph_reset(const XCSF *xcsf, const GRAPH *dgp)
{
    (void)xcsf;
    for(int i = 0; i < dgp->n; i++) {
        dgp->state[i] = dgp->initial_state[i];
    }
}

/**
 * @brief Randomises a specified DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to randomise.
 */
void graph_rand(const XCSF *xcsf, GRAPH *dgp)
{
    dgp->t = irand_uniform(1,xcsf->MAX_T);
    for(int i = 0; i < dgp->n; i++) {
        dgp->function[i] = irand_uniform(0, NUM_FUNC);
        dgp->initial_state[i] = rand_uniform(0,1);
        dgp->state[i] = rand_uniform(0,1);
    }
    for(int i = 0; i < dgp->klen; i++) {
        dgp->connectivity[i] = random_connection(dgp->n, xcsf->x_dim);
    }
}

/**
 * @brief Returns a random connection.
 * @param n_nodes The number of nodes in the graph.
 * @param n_inputs The number of external inputs to the graph.
 */
static int random_connection(int n_nodes, int n_inputs)
{
    // another node within the graph
    if(rand_uniform(0,1) < 0.5) {
        return irand_uniform(0, n_nodes) + n_inputs;
    }
    // external input
    return irand_uniform(0, n_inputs);
}

/**
 * @brief Updates a DGP graph T cycles.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to update.
 * @param inputs The inputs to the graph.
 */
void graph_update(const XCSF *xcsf, const GRAPH *dgp, const double *inputs)
{
    if(xcsf->RESET_STATES) {
        graph_reset(xcsf, dgp);
    }
    for(int t = 0; t < dgp->t; t++) {
        synchronous_update(xcsf, dgp, inputs);
    }
}

/**
 * @brief Performs a synchronous update.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to update.
 * @param inputs The inputs to the graph.
 */
static void synchronous_update(const XCSF *xcsf, const GRAPH *dgp, const double *inputs)
{
    for(int i = 0; i < dgp->n; i++) {
        for(int k = 0; k < xcsf->MAX_K; k++) {
            int c = dgp->connectivity[i * xcsf->MAX_K + k];
            if(c < xcsf->x_dim) { // external input
                dgp->tmp_input[k] = inputs[c];
            }
            else { // another node within the graph
                dgp->tmp_input[k] = dgp->state[c - xcsf->x_dim];
            }
        }
        dgp->tmp_state[i] = node_activate(dgp->function[i], dgp->tmp_input, xcsf->MAX_K);
    }
    memcpy(dgp->state, dgp->tmp_state, sizeof(double) * dgp->n);
}

/**
 * @brief Prints a DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to print.
 */
void graph_print(const XCSF *xcsf, const GRAPH *dgp)
{
    printf("Graph: N=%d; T=%d\n", dgp->n, dgp->t);
    for(int i = 0; i < dgp->n; i++) {
        printf("Node %d: func=%s state=%f init_state=%f con=[", 
                i, function_string(dgp->function[i]),
                dgp->state[i], dgp->initial_state[i]);
        printf("%d", dgp->connectivity[0]);
        for(int j = 1; j < xcsf->MAX_K; j++) {
            printf(",%d", dgp->connectivity[i]);
        }
        printf("]\n");
    }
}

/**
 * @brief Frees a DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to be freed.
 */
void graph_free(const XCSF *xcsf, const GRAPH *dgp)
{
    (void)xcsf;
    free(dgp->connectivity);
    free(dgp->state);
    free(dgp->initial_state);
    free(dgp->tmp_state);
    free(dgp->tmp_input);
    free(dgp->function);
}

/**
 * @brief Mutates a specified DGP graph.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to be mutated.
 * @return Whether any alterations were made.
 */
_Bool graph_mutate(const XCSF *xcsf, GRAPH *dgp)
{
    sam_adapt(xcsf, dgp->mu, DGP_N_MU);
    _Bool mod = false;
    // mutate functions
    for(int i = 0; i < dgp->n; i++) {
        if(rand_uniform(0,1) < dgp->mu[0]) {
            int orig = dgp->function[i];
            dgp->function[i] = irand_uniform(0, NUM_FUNC);
            if(orig != dgp->function[i]) {
                mod = true;
            }
        }
    }
    // mutate connectivity map
    for(int i = 0; i < dgp->klen; i++) {
        if(rand_uniform(0,1) < dgp->mu[1]) {
            int orig = dgp->connectivity[i];
            dgp->connectivity[i] = random_connection(dgp->n, xcsf->x_dim);
            if(orig != dgp->connectivity[i]) {
                mod = true;
            }
        }
    }               
    // mutate number of update cycles
    if(rand_uniform(0,1) < dgp->mu[2]) {
        int orig = dgp->t;
        dgp->t = irand_uniform(1,xcsf->MAX_T);
        if(orig != dgp->t) {
            mod = true;
        }
    }
    return mod;
}

/**
 * @brief Performs uniform crossover with two DGP graphs.
 * @param xcsf The XCSF data structure.
 * @param dgp1 The first DGP graph to perform crossover.
 * @param dgp2 The second DGP graph to perform crossover.
 * @return Whether crossover was performed.
 */
_Bool graph_crossover(const XCSF *xcsf, GRAPH *dgp1, GRAPH *dgp2)
{
    (void)xcsf; (void)dgp1; (void)dgp2;
    return false;
}

/**
 * @brief Returns the result from applying a specified activation function.
 * @param function The activation function to apply.
 * @param inputs The input to the activation function.
 * @param k The number of inputs to the activation function.
 * @return The result from applying the activation function.
 */
static double node_activate(int function, const double *inputs, int k)
{
    double state = 0;
    switch(function)
    {
        case 0: // Fuzzy NOT
            state = 1 - inputs[0];
            break;
        case 1: // Fuzzy AND (CFMQVS)
            state = inputs[0];
            for(int i = 1; i < k; i++) {
                state *= inputs[i];
            }
            break;
        case 2: // Fuzzy OR (CFMQVS)
            state = inputs[0];
            for(int i = 1; i < k; i++) {
                state += inputs[i];
            }
            break;
        default: // Invalid function
            printf("Error updating node: Invalid function: %d\n", function);
            exit(EXIT_FAILURE);
    }
    state = clamp(0, 1, state);
    return state;
}

/**
 * @brief Returns the name of a specified node function.
 * @param function The node function.
 * @return The name of the node function.
 */
static const char *function_string(int function)
{
    switch(function) {
        case 0: return "Fuzzy NOT";
        case 1: return "Fuzzy AND (CFMQVS)";
        case 2: return "Fuzzy OR (CFMQVS)";
        default:
                printf("function_string(): invalid node function: %d\n", function);
                exit(EXIT_FAILURE);
    }
}

/**
 * @brief Writes DGP graph to a binary file.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to save.
 * @param fp Pointer to the file to be written.
 * @return The number of elements written.
 */
size_t graph_save(const XCSF *xcsf, const GRAPH *dgp, FILE *fp)
{
    (void)xcsf;
    size_t s = 0;
    s += fwrite(&dgp->n, sizeof(int), 1, fp);
    s += fwrite(&dgp->t, sizeof(int), 1, fp);
    s += fwrite(&dgp->klen, sizeof(int), 1, fp);
    s += fwrite(dgp->state, sizeof(double), dgp->n, fp);
    s += fwrite(dgp->initial_state, sizeof(double), dgp->n, fp);
    s += fwrite(dgp->function, sizeof(int), dgp->n, fp);
    s += fwrite(dgp->connectivity, sizeof(int), dgp->klen, fp);
    s += fwrite(dgp->mu, sizeof(double), DGP_N_MU, fp);
    return s;
}

/**
 * @brief Reads DGP graph from a binary file.
 * @param xcsf The XCSF data structure.
 * @param dgp The DGP graph to load.
 * @param fp Pointer to the file to be written.
 * @return The number of elements written.
 */
size_t graph_load(const XCSF *xcsf, GRAPH *dgp, FILE *fp)
{
    (void)xcsf;
    size_t s = 0;
    s += fread(&dgp->n, sizeof(int), 1, fp);
    s += fread(&dgp->t, sizeof(int), 1, fp);
    s += fread(&dgp->klen, sizeof(int), 1, fp);
    dgp->state = malloc(sizeof(double) * dgp->n);
    dgp->initial_state = malloc(sizeof(double) * dgp->n);
    dgp->tmp_state = malloc(sizeof(double) * dgp->n);
    dgp->tmp_input = malloc(sizeof(double) * xcsf->MAX_K);
    dgp->function = malloc(sizeof(int) * dgp->n);
    dgp->connectivity = malloc(sizeof(int) * dgp->klen);
    s += fread(dgp->state, sizeof(double), dgp->n, fp);
    s += fread(dgp->initial_state, sizeof(double), dgp->n, fp);
    s += fread(dgp->function, sizeof(int), dgp->n, fp);
    s += fread(dgp->connectivity, sizeof(int), dgp->klen, fp);
    s += fread(dgp->mu, sizeof(double), DGP_N_MU, fp);
    return s;
}
