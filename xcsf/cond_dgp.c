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
 * @file cond_dgp.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2016--2020.
 * @brief Dynamical GP graph condition functions.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xcsf.h"
#include "utils.h"
#include "sam.h"
#include "dgp.h"
#include "cl.h"
#include "condition.h"
#include "cond_dgp.h"

void cond_dgp_init(const XCSF *xcsf, CL *c)
{
    COND_DGP *new = malloc(sizeof(COND_DGP));
    graph_init(xcsf, &new->dgp, xcsf->DGP_NUM_NODES);
    graph_rand(xcsf, &new->dgp);
    c->cond = new;
}

void cond_dgp_free(const XCSF *xcsf, const CL *c)
{
    const COND_DGP *cond = c->cond;
    graph_free(xcsf, &cond->dgp);
    free(c->cond);
}                  

void cond_dgp_copy(const XCSF *xcsf, CL *dest, const CL *src)
{
    COND_DGP *new = malloc(sizeof(COND_DGP));
    const COND_DGP *src_cond = src->cond;
    graph_init(xcsf, &new->dgp, src_cond->dgp.n);
    graph_copy(xcsf, &new->dgp, &src_cond->dgp);
    dest->cond = new;
}

void cond_dgp_cover(const XCSF *xcsf, const CL *c, const double *x)
{
    COND_DGP *cond = c->cond;
    do {
        graph_rand(xcsf, &cond->dgp);
    } while(!cond_dgp_match(xcsf, c, x));
}
 
void cond_dgp_update(const XCSF *xcsf, const CL *c, const double *x, const double *y)
{
    (void)xcsf; (void)c; (void)x; (void)y;
}
 
_Bool cond_dgp_match(const XCSF *xcsf, const CL *c, const double *x)
{
    const COND_DGP *cond = c->cond;
    graph_update(xcsf, &cond->dgp, x);
    if(graph_output(xcsf, &cond->dgp, 0) > 0.5) {
        return true;
    }
    return false;
}            

_Bool cond_dgp_mutate(const XCSF *xcsf, const CL *c)
{
    COND_DGP *cond = c->cond;
    return graph_mutate(xcsf, &cond->dgp);
}

_Bool cond_dgp_crossover(const XCSF *xcsf, const CL *c1, const CL *c2)
{
    COND_DGP *cond1 = c1->cond;
    COND_DGP *cond2 = c2->cond;
    return graph_crossover(xcsf, &cond1->dgp, &cond2->dgp);
}

_Bool cond_dgp_general(const XCSF *xcsf, const CL *c1, const CL *c2)
{
    (void)xcsf; (void)c1; (void)c2;
    return false;
}

void cond_dgp_print(const XCSF *xcsf, const CL *c)
{
    (void)xcsf;
    const COND_DGP *cond = c->cond;
    graph_print(xcsf, &cond->dgp);
}  

int cond_dgp_size(const XCSF *xcsf, const CL *c)
{
    (void)xcsf;
    const COND_DGP *cond = c->cond;
    return cond->dgp.n;
}

size_t cond_dgp_save(const XCSF *xcsf, const CL *c, FILE *fp)
{
    const COND_DGP *cond = c->cond;
    size_t s = graph_save(xcsf, &cond->dgp, fp);
    return s;
}

size_t cond_dgp_load(const XCSF *xcsf, CL *c, FILE *fp)
{
    COND_DGP *new = malloc(sizeof(COND_DGP));
    size_t s = graph_load(xcsf, &new->dgp, fp);
    c->cond = new;
    return s;
}
