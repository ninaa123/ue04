/*encoding: UTF-8, break: linux, indent: spaces, tabsize: 4, language: C99/eng*/
/**
 * \file galton.c
 *
 * Template: Exercise Sheet #4
 * Tutorial "Statisik und Datenanalyse",
 * SS2014 University of Innsbruck
 *
 * This simulates a galton board. Output is the number of items in each box
 * after simulation.
 * Boxes are numbered 0 to (argv[2]-1) corresponding to (argv[2]-1) levels of
 * spacers (=nails) on the board. The number of iterations (=balls put in) can
 * be specified with argv[1]. An additional argument starting with 'n' will
 * prevent histogram output, leaving just run time, #bins and #iterations.
 *
 * Compile with:
 *    gcc -Wall -W -O2 -fopenmp -o "sad14_04_template" "sad14_04_template.c"
 *
 * \date       27. Mar 2014
 * \author     Jan Bischko
 * \copyright  Copyright Â© 2014, Leopold Franzens UniversitÃ¤t Innsbruck
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include <string.h>

/*##############################################################################
# HELPER FUNCTIONS
##############################################################################*/

/** Gets the current unix time in seconds but with micro seconds precision */
double getTimeStamp(void) {
    struct timeval timestr;
    void *Tzp = 0;
    gettimeofday(&timestr, Tzp);
    return (double)timestr.tv_sec+1.0E-06*(double)timestr.tv_usec;
}

/** Prints how to use this program. */
void usage(char* argv[]) {
    fprintf(stderr,
        "Usage: %s iterations boxes 0<probability<1 threads<=%i [nooutput]\n\n",
        argv[0], omp_get_num_procs()
    );
    exit(EXIT_FAILURE);
}

int randoffset(double prob, unsigned int *seed)
{
    return (double)rand_r(seed)>prob*RAND_MAX ? 1 : 0;
}

/*##############################################################################
# MAIN PROGRAM
##############################################################################*/

int main( int argc, char *argv[] ) {
    /*################ BORING STUFF ################*/
    /* declarations */
    unsigned    k, k2,              // multi purpose iterator
                nooutput = 0,       // suppress output (1=yes)
                boxcount,           // how many boxes there are
                **box,              // array for number of events in each box
                iterations,         // how many experiments to be made
                threads,            // number of threads to use
                max_threads;        // maximum number of threads availible
    double      time;               // time for clocking the program
    float       prob;               // probability to go in a certain direction


    /* argument handling */
    if( argc < 5 ) usage(argv);
    iterations  = (unsigned)atof(argv[1]); // atof for shorthand such as 1e7
    boxcount    = (unsigned)atof(argv[2]);
    prob        =           atof(argv[3]);
    threads     =           atoi(argv[4]);
    if( prob>1.0 || prob<0 ) usage(argv);
    for( k=0; k<(unsigned)argc; ++k) if( argv[k][0] == 'n' ) nooutput = 1;

    /* preparations */

    max_threads = (unsigned)omp_get_num_procs();
    threads = (threads > 0 && threads<=max_threads) ? threads : max_threads;
    omp_set_num_threads(threads);
    
    box = (unsigned**) calloc(threads,sizeof(unsigned int *));
    if( box == NULL ) { fprintf(stderr, "Error: Allocation failed.\n"); exit(EXIT_FAILURE); }
    
    for(k=0;k<threads;++k)
    {
        box[k]=(unsigned int *)calloc(boxcount,sizeof(unsigned int));
        if( box[k] == NULL ) { fprintf(stderr, "Error: Allocation failed.\n"); exit(EXIT_FAILURE); }
    }
    
    time = getTimeStamp();

    /*################ PARALLEL PART ################*/
    #pragma omp parallel // Every thread will execute the following block
    {
        /* declarations for each thread */
        unsigned        i, j,           // Multi-purpose iterators
                        threadID,       // Number of this thread
                        threadReps,     // Repetitions this thread has to do
                        tmpwalk,        // Position within the iteration
                        seed;           //every thread has own seed  
        unsigned int *b;

        /* preparations for each thread */
        threadID = omp_get_thread_num()+1;
        threadReps = iterations / threads;
        if( threadID <= iterations%threads ) ++threadReps;
        if( nooutput == 0 ) printf(
            "Thread %u of %u has to do %g iterations!\n",
              threadID,   threads,     (double)threadReps
        );
        
        b=box[threadID-1];
        
        seed = ((int)getTimeStamp())^threadID;
/*
        #pragma omp single // Block executed only by first thread to reach it
        {
            //srand((int)time^threadID);
        }
*/

        /*################ ACTUAL SIMULATION ################*/
        for(i=0; i<threadReps; ++i) {
            tmpwalk = 0;
            for(j=0; j<boxcount-1; ++j)
            {
                //seed = getTimeStamp(); 
                tmpwalk += randoffset(prob, &seed);
            }
            //#pragma omp atomic
            //++box[tmpwalk];
            ++b[tmpwalk];
        }
    }

    for(k=1;k<threads;++k)
    {
        for(k2=0;k2<boxcount;++k2)
        {
            box[0][k2]+=box[k][k2];
        }
    }
    
    time = getTimeStamp()-time;

    /*#################### OUTPUT #######################*/
    
    FILE *f;
    char filename[128];
     
    sprintf(filename, "rep%u_boxes%u.txt",iterations,boxcount);
    f=fopen(filename,"a+");
    
/*
    printf(
        "runtime %g\n" "iterations %g\n"   "boxes %g\n"  "probability %g\n\n\n",
             time,    (double)iterations, (double)boxcount,  (double)prob
    );
*/
    
    fprintf(f,"%u %g\n",threads,time);
        
    if( nooutput == 0 ) for( k=0; k<boxcount; ++k) printf("%u\n", box[0][k]);

    fclose(f);

    /*################## CLEAN UP ######################*/
    for(k=0;k<threads;++k)
    {
        free(box[k]);
    }
    free(box);
    return EXIT_SUCCESS;
}