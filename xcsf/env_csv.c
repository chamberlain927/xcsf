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
 * @file env_csv.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2019.
 * @brief CSV input file handling functions.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>
#include "xcsf.h"
#include "utils.h"
#include "env_csv.h"

#define MAX_ROWS 100000 //!< Maximum number of instances
#define MAX_COLS 200 //!< Maximum line length
#define MAX_NAME 200 //!< Maximum file name length
#define DELIM "," //!< File delimiter

void env_csv_read(char *fname, double **data, int *num_prob, int *num_vars);

/**
 * @brief Parses specified csv files into training and testing data sets.
 * @param infile The base name of the csv files to read.
 * @param train_data The data structure to load the training data.
 * @param test_data The data structure to load the testing data.
 *
 * @details Expects an identical number of x and y rows.
 */
void env_csv_input_read(char *infile, INPUT *train_data, INPUT *test_data)
{
    char name[MAX_NAME];
    sprintf(name, "%s_train_x.csv", infile);
    env_csv_read(name, &train_data->x, &train_data->rows, &train_data->x_cols);
    sprintf(name, "%s_train_y.csv", infile);
    env_csv_read(name, &train_data->y, &train_data->rows, &train_data->y_cols);
    sprintf(name, "%s_test_x.csv", infile);
    env_csv_read(name, &test_data->x, &test_data->rows, &test_data->x_cols);
    sprintf(name, "%s_test_y.csv", infile);
    env_csv_read(name, &test_data->y, &test_data->rows, &test_data->y_cols);
}

/**
 * @brief Parses a specified csv file.
 * @param fname The name of the csv file to read.
 * @param data The data structure to load the data (set by this function).
 * @param num_rows The number of rows in the dataset (set by this function).
 * @param num_cols The number of columns in the dataset (set by this function).
 *
 * @details Provided a file name will set the data, num_rows, and num_cols.
 */
void env_csv_read(char *fname, double **data, int *num_rows, int *num_cols)
{
    FILE *fin = fopen(fname, "rt");
    if(fin == 0) {
        printf("Error opening file: %s. %s.\n", fname, strerror(errno));
        exit(EXIT_FAILURE);
    }    
    // ascertain the file length and number of variables per line
    *num_rows = 0;
    *num_cols = 0;
    char line[MAX_COLS];
    while(fgets(line, MAX_COLS, fin) != NULL) {
        if(*num_rows > MAX_ROWS) {
            printf("data file %s is too big; maximum: %d\n", fname, MAX_ROWS);
            exit(EXIT_FAILURE);
        }        
        // use the first line to count the number of variables on a line
        if(*num_rows == 0) {
            char *ptok = strtok(line, DELIM);
            while(ptok != NULL) {
                if(strnlen(ptok,MAX_COLS) > 0) {
                    (*num_cols)++;
                }
                ptok = strtok(NULL, DELIM);
            }
        }
        (*num_rows)++; // count the number of lines
    }
    // read data file to memory
    rewind(fin);
    *data = malloc(sizeof(double) * (*num_cols) * (*num_rows));
    for(int i = 0; fgets(line,MAX_COLS,fin) != NULL; i++) {
        (*data)[i * (*num_cols)] = atof(strtok(line, DELIM));
        for(int j = 1; j < *num_cols; j++) {
            (*data)[i * (*num_cols)+j] = atof(strtok(NULL, DELIM));
        }
    }
    fclose(fin);
    printf("Loaded: %s: %d rows, %d cols\n", fname, *num_rows, *num_cols);
}

/**
 * @brief Frees an input data structure.
 * @param data The input data structure to free.
 */
void env_csv_input_free(INPUT *data)
{                 
    free(data->x);
    free(data->y);
}
