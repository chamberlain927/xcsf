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
 * @file xcsf.h
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2019.
 * @brief XCSF data structures.
 */ 
 
#pragma once

/**
 * @brief Classifier data structure.
 */
typedef struct CL {
    struct CondVtbl const *cond_vptr; //!< Functions acting on conditions
    struct PredVtbl const *pred_vptr; //!< Functions acting on predictions
    struct ActVtbl const *act_vptr; //!< Functions acting on actions
    void *cond; //!< Condition structure
    void *pred; //!< Prediction structure
    void *act; //!< Action structure
    double *mu; //!< Self-adaptive mutation rates
    double err; //!< Error
    double fit; //!< Fitness
    int num; //!< Numerosity
    int exp; //!< Experience
    double size; //!< Average participated set size
    int time; //!< Time EA last executed in a participating set
    _Bool m; //!< Whether the classifier matches current input
    double *prediction; //!< Current classifier prediction
    double *prev_prediction; //!< Previous classifier prediction
    int action; //!< Current classifier action
    _Bool *mhist; //!< (THETA_SUB) recent matching decisions
} CL;

/**
 * @brief Classifier linked list.
 */
typedef struct CLIST {
    CL *cl; //!< Pointer to classifier data structure
    struct CLIST *next; //!< Pointer to the next list element
} CLIST;

/**
 * @brief Classifier set.
 */
typedef struct SET {
    CLIST *list; //!< Linked list of classifiers
    int size; //!< Number of macro-classifiers
    int num; //!< The total numerosity of classifiers
} SET;

/**
 * @brief XCSF data structure.
 */
typedef struct XCSF {
    SET pset; //!< Population set
    int time; //!< Current number of executed trials
    double msetsize; //!< Average match set size

    // experiment parameters
    int OMP_NUM_THREADS; //!< Number of threads for parallel processing
    _Bool POP_INIT; //!< Population initially empty or filled with random conditions
    int MAX_TRIALS; //!< Number of problem instances to run in one experiment
    int PERF_AVG_TRIALS; //!< Number of problem instances to average performance output
    int POP_SIZE; //!< Maximum number of macro-classifiers in the population
    int LOSS_FUNC; //!< Which loss/error function to apply

    // multi-step problem parameters
    double GAMMA; //!< Discount factor in calculating the reward for multi-step problems
    int TELETRANSPORTATION; //!< Num steps to reset a multi-step problem if goal not found
    double P_EXPLORE; //!< Probability of exploring vs. exploiting

    // classifier parameters
    double ALPHA; //!< Linear coefficient used in calculating classifier accuracy
    double BETA; //!< Learning rate for updating error, fitness, and set size
    double DELTA; //!< Fit used in prob of deletion if fit less than this frac of avg pop fit 
    double EPS_0; //!< Classifier target error, under which the fitness is set to 1
    double ERR_REDUC; //!< Amount to reduce an offspring's error
    double FIT_REDUC; //!< Amount to reduce an offspring's fitness
    double INIT_ERROR; //!< Initial classifier error value
    double INIT_FITNESS; //!< Initial classifier fitness value
    double NU; //!< Exponent used in calculating classifier accuracy
    int THETA_DEL; //!< Min experience before fitness used in probability of deletion
    int COND_TYPE; //!< Classifier condition type: hyperrectangles, GP trees, etc.
    int PRED_TYPE; //!< Classifier prediction type: least squares, neural nets, etc.
    int ACT_TYPE; //!< Classifier action type

    // evolutionary algorithm parameters
    double P_CROSSOVER; //!< Probability of applying crossover (for hyperrectangles)
    double P_MUTATION; //!< Probability of mutation occuring per allele
    double F_MUTATION; //!< Probability of performing mutating a graph/net function
    double S_MUTATION; //!< Maximum amount to mutate an allele
    double E_MUTATION; //!< Rate of gradient descent mutation
    double THETA_EA; //!< Average match set time between EA invocations
    int LAMBDA; //!< Number of offspring to create each EA invocation
    int EA_SELECT_TYPE; //!< Roulette or tournament for EA parental selection
    double EA_SELECT_SIZE; //!< Fraction of set size for tournaments

    // self-adaptive mutation parameters
    int SAM_TYPE; //!< 0 = log normal, 1 = ten normally distributed rates
    int SAM_NUM; //!< Number of self-adaptive mutation rates
    double SAM_MIN; //!< Minimum value of a log normal adaptive mutation rate

    // classifier condition parameters
    double MAX_CON; //!< Maximum value expected from inputs
    double MIN_CON; //!< Minimum value expected from inputs
    int DGP_NUM_NODES; //!< Number of nodes in a DGP graph
    _Bool RESET_STATES; //!< Whether to reset the initial states of DGP graphs
    int MAX_K; //!< Maximum number of connections a DGP node may have
    int MAX_T; //!< Maximum number of cycles to update a DGP graph
    int GP_NUM_CONS; //!< Number of constants available for GP trees
    int GP_INIT_DEPTH; //!< Initial depth of GP trees
    double *gp_cons; //!< Stores constants available for GP trees

    double COND_ETA; //!< Gradient descent rate for updating the condition
    _Bool COND_EVOLVE_WEIGHTS; //!< Whether to evolve condition network weights
    _Bool COND_EVOLVE_NEURONS; //!< Whether to evolve number of condition network neurons
    _Bool COND_EVOLVE_FUNCTIONS; //!< Whether to evolve condition network activation functions
    int COND_NUM_HIDDEN_NEURONS; //!< Initial number of hidden neurons (random if <= 0)
    int COND_MAX_HIDDEN_NEURONS; //!< Maximum number of neurons if evolved
    int COND_HIDDEN_NEURON_ACTIVATION; //!< Activation function for the hidden layer
 
    // prediction parameters
    _Bool PRED_RESET; //!< Whether to reset offspring predictions instead of copying
    double PRED_ETA; //!< Gradient desecnt rate for updating the prediction
    double PRED_X0; //!< Prediction weight vector offset value
    double PRED_RLS_SCALE_FACTOR; //!< Initial diagonal values of the RLS gain-matrix
    double PRED_RLS_LAMBDA; //!< Forget rate for RLS: small values may be unstable
    _Bool PRED_EVOLVE_WEIGHTS; //!< Whether to evolve prediction network weights
    _Bool PRED_EVOLVE_NEURONS; //!< Whether to evolve number of prediction network neurons
    _Bool PRED_EVOLVE_FUNCTIONS; //!< Whether to evolve prediction network activation functions
    _Bool PRED_EVOLVE_ETA; //!< Whether to evolve prediction gradient descent rates
    _Bool PRED_SGD_WEIGHTS; //!< Whether to use gradient descent for predictions
    double PRED_MOMENTUM; //!< Momentum for gradient descent
    int PRED_NUM_HIDDEN_NEURONS; //!< Initial number of hidden neurons (random if <= 0)
    int PRED_MAX_HIDDEN_NEURONS; //!< Maximum number of neurons if evolved
    int PRED_HIDDEN_NEURON_ACTIVATION; //!< Activation function for the hidden layer

    // subsumption parameters
    _Bool EA_SUBSUMPTION; //!< Whether to try and subsume offspring classifiers
    _Bool SET_SUBSUMPTION; //!< Whether to perform match set subsumption
    int THETA_SUB; //!< Minimum experience of a classifier to become a subsumer

    // built-in environments
    struct EnvVtbl const *env_vptr; //!< Functions acting on environments
    void *env; //!< Environment structure

    // prediction array
    double *pa; //!< Prediction array (stores fitness weighted predictions)
    double *nr; //!< Prediction array (stores total fitness)

    // set by environment
    int stage; //!< Current stage of training
    _Bool train; //!< Training or test mode
    int num_x_vars; //!< Number of problem input variables
    int num_y_vars; //!< Number of problem output variables
    int num_actions; //!< Number of class labels / actions
    double (*loss_ptr)(struct XCSF*, double*, double*); //!< Pointer to loss/error function
} XCSF;                  

/**
 * @brief Input data structure.
 */
typedef struct INPUT {
    double *x; //!< Feature variables
    double *y; //!< Target variables
    int x_cols; //!< Number of feature variables
    int y_cols; //!< Number of target variables
    int rows; //!< Number of instances
} INPUT;

double xcsf_fit1(XCSF *xcsf, INPUT *train_data, _Bool shuffle);
double xcsf_fit2(XCSF *xcsf, INPUT *train_data, INPUT *test_data, _Bool shuffle);
double xcsf_score(XCSF *xcsf, INPUT *test_data);
void xcsf_predict(XCSF *xcsf, double *input, double *output, int rows);
void xcsf_print_match_set(XCSF *xcsf, double *input, _Bool printc, _Bool printa, _Bool printp);
void xcsf_print_pop(XCSF *xcsf, _Bool printc, _Bool printa, _Bool printp);
size_t xcsf_load(XCSF *xcsf, char *fname);
size_t xcsf_save(XCSF *xcsf, char *fname);
double xcsf_version();
