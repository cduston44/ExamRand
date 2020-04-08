/* This here code will output a script that will generate
 * a bunch of randomized exams.
 */

/* TEXFILE INSTRUCTIONS
 * This has only been tested with Philip Hirschorn's exam.cls class:
 * http://www-math.mit.edu/~psh/#ExamCls
 * 
 * In the tex file, write \Pa, \Pb, \Pc for the random variables.
 * In the parameter file, write max min steps for each of those variables.
 * The "steps" does nothing right now; maybe later it will be used 
 * for something.
 *
 * Example files should be attached here as well.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>

/*#include <gsl/gsl_math.h>
%#include <gsl/gsl_vector.h>
%#include <gsl/gsl_multiroots.h>


%#define PLOT_POINTS 100 /* Number of Points to Plot */
/*%#define MIN_PLOT 0 /* Min time for plot */
/*%#define MAX_PLOT 3 /* Max time for plot */


/* Compilation Instructions:
 * gcc -o ExamRand ExamRand.c -lm -lgsl -lgslcblas
 */



  /* Functions at end */ 


int main(int argc, char *argv[])
{
  if ( argc != 4 ){ /* argc should be for correct execution */
    printf( "usage: %s <parameter_file> <n_exams> <verb>\n\nparameter_file: file for parameters. First line should be the name of the Exam .tex file, next line should be output script name, and the rest of the lines should be \n\n min max n_steps\n\n for each parameter.\nn_exams: Number of exams to be produced, labeled by a letter\n", argv[0]);
    exit(1);
  }

  /* Variable Definition */
  int verb,*n_steps,p,i,j,n_exams;
  float *min,*max,diff;
  char texfile[100],outfile[100],alph_ex,alph_p,temp[100];
  
  /* Scan in parameters from command line */
  sscanf(argv[2],"%i",&n_exams);
  sscanf(argv[3],"%i",&verb);
  
  /* 
   * First read in the parameter file to determine
   * the infile and outfile names, as well as the number
   * of parameters
   */
  FILE *input, *input2;
  if ((input=fopen(argv[1],"r"))==NULL) {
    printf("Cannot open file\n");
    exit(1);
  }

  fscanf(input,"%[^\n]%*c",texfile);
  fscanf(input,"%[^\n]%*c",outfile);
  p=0;
  
  while(!feof(input)){
    fscanf(input,"%[^\n]%*c",&temp);
    /*printf("%i %s\n",p,temp);*/
    p++;
  }
  if (verb>0) {printf("%s, %s, %i random variables\n",texfile,outfile,p);}
  fclose(input);

  /* Set up variable arrays */
  min=malloc(p*sizeof(float));
  max=malloc(p*sizeof(float));
  n_steps=malloc(p*sizeof(int));

  /* Reopen parameter file, read to store variable info */
  if ((input2=fopen(argv[1],"r"))==NULL) {
    printf("Cannot open file\n");
    exit(1);
  }
  fscanf(input2,"%*[^\n]%*c%*[^\n]");
  if(verb>0){printf("Second pass at parameter file to get steps...\n");}
  i=0;
  for (i=0;i<p;i++){
    fscanf(input2,"%f %f %i",&min[i],&max[i],&n_steps[i]);
    if (verb>0) {printf("---> %f %f %i\n",min[i],max[i],n_steps[i]);}
  }
  fclose(input2);
  
  /* Open output file */
  FILE *output;

  if ((output=fopen(outfile,"w"))==NULL){
    printf("Cannot open file \n");
    exit(1);
  }

  fprintf(output,"#!/bin/sh \n\n");

  /* Fire up the random number generator */
  const gsl_rng_type * T;
  gsl_rng * r;
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  
  /* Now output script */
  for (i=1;i<=n_exams;i++){
    fprintf(output,"pdflatex -jobname=%s-%i \"",texfile,i);
    for (j=0;j<p;j++){
      /* Associate a character to the parameter number */
      alph_p=(char)(j+97);

      /* Get a random value between min and max */
      double u=gsl_rng_uniform (r);

      fprintf(output,"\\def\\P%c{%.3g } ",alph_p,min[j]+u*(max[j]-min[j]));
    }
    fprintf(output,"\\input{%s.tex}\"\n",texfile);
  }
  if(verb>0){printf("Processed %i random variables\n",p);}

  
  fclose(output);
  
  
}
