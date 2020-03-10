/*******************************************************************/ 
/*  HPC(3) : TP4 parallélisation de la solution numérique des Equations
 * aux dérivées partielles, l'équation de la diffusion de la chaleur
 *  
 *  Eva Dokladalova, 2019
 */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>
#include "function.h"
#include<math.h>



int
max(int a, int b)
{
     if (a <= b)
     {
	  return a;
     }
     else
     {
	  return b;
     }
}

int
min(int a, int b)
{
     if (a >= b)
     {
	  return a;
     }
     else
     {
	  return b;
     }
}

float
get_cell(float* T, int i, int j, int rw, int cl)
{
     if (i < 0 || j < 0 || i >= rw || j >= cl)
     {
	  return 0.;
     }
     else
     {
	  return T[i + j*rw];
     }
}

//-----------------------------------------------------------
// MAIN FUNCTION
//-----------------------------------------------------------
int main (int argc, char *argv[])
{
 
  // noms des fichiers d'entrée et de sortie
  char *filename=argv[1];
  char *file_out=argv[2];

  char frame[100];
  
  // pointeurs vers les matrices pour l'intégration temporelle
  float *T;
  float *Tdt;
  float *swap;

  // dimension de matrice, valeur maximale
  int v;  // max  value in matrix
  int rw; // row size
  int cl; // column size

  // vérification des arguments d'entrée
  if (argc != 3)
    {  fprintf(stderr,"Input parameters missing:\n./program_name <inpout.pgm> <output.pgm>\n");
      return(0);
    }

  
  //-------------------------------------------------------------
  // OPEN DATA FILE AND ALLOCATE INPUT IMAGE MEMORY (float precision)
  //-------------------------------------------------------------
  T = readimg(filename, &rw, &cl, &v);
  Tdt = (float *) calloc (rw*cl,sizeof(float));
 
  //-------------------------------------------------------------
  // PUT HERE THE NUMERICAL SOLUTION OF HEAT EQUATION
  // complete variables necessary for for the numerical scheme computing
  //-------------------------------------------------------------
  int i;
  int j;
  int it;
  float dt = 0.2;
  memcpy(Tdt,T,sizeof(float)*rw*cl);
  double  t0 = omp_get_wtime ();     

  // A COMPLETER SELON LE DERNIER COURS :-)



  omp_set_num_threads(4);
  
  
  
  printf("DIMS: %d x %d\n", rw, cl);


//#pragma omp parallel for
  
  for (it = 0; it < 1000; ++it)
  {

       // une itération de la diffusion
       for (i = 0; i < rw; ++i)
       {
	    for (j = 0; j < cl; ++j)
	    {
		 
		 float laplacian =
		      get_cell(T, i+1, j, rw, cl) + get_cell(T, i-1, j, rw, cl)
		      + get_cell(T, i, j+1, rw, cl) + get_cell(T, i, j-1, rw, cl)		      - 4.*get_cell(T, i, j, rw, cl);
		 
		 Tdt[i + j*rw] = fmax(fmin(T[i + j*rw] + dt*laplacian, v), 0.);
	    }
       }

       ///////////////////////////////////////////////////
       /////// juste pour quand on veut produire un gif
       #if EXPORT_FRAMES
       if (it%50==0)
       {
	    sprintf(frame, "frames/frame_");
	    int l = 13;
	    for (int k = 100000; k >= 1; k = k/10)
	    {
		 int decimale = (it/k % 10);
		 frame[l] = (char) decimale+48;
		 ++l;
	    }
	    frame[l] = 0;

	    printf("FRAME NAME: %s\n", frame);
	    writeimg(frame, T, rw, cl, v);
       }
       #endif
       ///////////////////////////////////////////////////


       // on échange les 2 buffers pour la prochaine itération
       swap = T;
       T = Tdt;
       Tdt = swap;
  }


  
  
  double  t1 = omp_get_wtime ();
  double  temps_reel=t1-t0;
  printf( " temps  reel %lf : \n", temps_reel);
  
  //-------------------------------------------------------------
  // WRITE RESULT IN A PGN IMAGE 
  //-------------------------------------------------------------
   writeimg(file_out, T, rw, cl, v);
   free(Tdt);
   free(T);
   return(0);
}

