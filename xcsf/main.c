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
 * @file main.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2019.
 * @brief Main function for stand-alone binary execution.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "xcsf.h"
#include "utils.h"
#include "config.h"
#include "env.h"
#include "env_csv.h"
#include "cl_set.h"
#include "xcs_single_step.h"
#include "xcs_multi_step.h"

#ifdef PARALLEL
#include <omp.h>
#endif

void regression(XCSF *xcsf, int argc, char **argv);
void classification(XCSF *xcsf, int argc, char **argv);

int main(int argc, char **argv)
{    
    if(argc < 3 || argc > 5) {
        printf("Usage: xcsf problemType{csv|mp|maze} problem{.csv|size|maze} [config.ini] [xcs.bin]\n");
        exit(EXIT_FAILURE);
    } 

    XCSF *xcsf = malloc(sizeof(XCSF));
    random_init();
    if(argc > 3) {
        constants_init(xcsf, argv[3]);
    }
    else {
        constants_init(xcsf, "default.ini");
    }
#ifdef PARALLEL
    omp_set_num_threads(xcsf->OMP_NUM_THREADS);
#endif

    // input csv file
    if(strcmp(argv[1], "csv") == 0) {
        regression(xcsf, argc, argv);
    }
    // maze or multiplexer
    else {
        classification(xcsf, argc, argv);
    }

    // clean up
    set_kill(xcsf, &xcsf->pset);
    constants_free(xcsf);
    free(xcsf);
    return EXIT_SUCCESS;
}

void regression(XCSF *xcsf, int argc, char **argv)
{
    INPUT *train_data = malloc(sizeof(INPUT));
    INPUT *test_data = malloc(sizeof(INPUT));
    env_csv_input_read(argv[2], train_data, test_data);
    xcsf->num_x_vars = train_data->x_cols;
    xcsf->num_y_vars = train_data->y_cols;
    xcsf->num_actions = 1; // regression

    // reload state of a previous experiment
    if(argc == 5) {
        printf("LOADING XCSF\n");
        xcsf->pset.size = 0;
        xcsf->pset.num = 0;
        xcsf_load(xcsf, argv[4]);
    }
    // start a new experiment
    else {
        pop_init(xcsf);
    }

    // train model
    xcsf_fit2(xcsf, train_data, test_data, true);

    //printf("SAVING XCSF\n");
    //xcsf_save(xcsf, "test.bin");

    // clean up
    env_csv_input_free(train_data);
    env_csv_input_free(test_data);
    free(train_data);
    free(test_data);
}

void classification(XCSF *xcsf, int argc, char **argv)
{
    env_init(xcsf, argv);
    // reload state of a previous experiment
    if(argc == 5) {
        printf("LOADING XCSF\n");
        xcsf->pset.size = 0;
        xcsf->pset.num = 0;
        xcsf_load(xcsf, argv[4]);
    }
    // start a new experiment
    else {
        pop_init(xcsf);
    }
    if(env_multistep(xcsf)) {
        xcs_multi_step_exp(xcsf);
    }
    else {
        xcs_single_step_exp(xcsf);
    }
    env_free(xcsf);
}
