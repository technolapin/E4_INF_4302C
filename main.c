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


//-----------------------------------------------------------
// MAIN FUNCTION
//-----------------------------------------------------------
int main (int argc, char *argv[])
{
 
  // noms des fichiers d'entrée et de sortie
  char *filename=argv[1];
  char *conductivity=argv[2];
  char *file_out=argv[3];

  char frame[100];
  
  // pointeurs vers les matrices pour l'intégration temporelle
  float *T;
  float *Tdt;
  float *swap;

  float *C;

  
  // dimension de matrice, valeur maximale
  int v;  // max  value in matrix
  int rw; // row size
  int cl; // column size

  // vérification des arguments d'entrée
  if (argc != 4)
    {  fprintf(stderr,"Input parameters missing:\n./program_name <inpout.pgm> <conductivity.pgm> <output.pgm>\n");
      return(0);
    }

  
  
  //-------------------------------------------------------------
  // OPEN DATA FILE AND ALLOCATE INPUT IMAGE MEMORY (float precision)
  //-------------------------------------------------------------
  int crw, ccl;
  T = readimg(filename, &rw, &cl, &v);
  C = readimg(conductivity, &crw, &ccl, &v);
  Tdt = (float *) calloc (rw*cl,sizeof(float));

  if (crw != rw || ccl != ccl)
  {
       fprintf(stderr,"Images size missmatch \n");
       return(0);
  }
  
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



  omp_set_num_threads(2);
  
  
  
  printf("DIMS: %d x %d\n", rw, cl);


  for (i = 0; i < rw; ++i)
  {
       for (j = 0; j < cl; ++j) // note: j MUST be private
       {
	    C[i + j*rw] = C[i+j*rw]*dt/255.;
       }
  }

  
  for (it = 0; it < 1000; ++it)
  {

       // une itération de la diffusion
#pragma omp parallel for private(i, j)
       for (i = 1; i < rw-1; ++i)
       {
	    
	    for (j = 1; j < cl-1; ++j) // note: j MUST be private
	    {
		 Tdt[i + j*rw] = T[i + j*rw]
		      + C[i + j*rw]*(
			   T[i+1 + rw*j] + T[i-1 + rw*j]
			   + T[i + rw*(j+1)] + T[i + rw*(j-1)]
			   - 4.*T[i + rw*j]
			   );
	    }
       }
#pragma omp parallel for private(i)
       for (i = 1; i < rw-1; ++i)
       {
	    Tdt[i] = T[i]
		 + C[i]*(
		      T[i+1] + T[i-1]
		      + T[i + rw]
		      - 3.*T[i]
		      );
       } 
#pragma omp parallel for private(i)
       for (i = 1; i < rw-1; ++i)
       {
	    Tdt[i + (cl-1)*rw] = T[i + (cl-1)*rw]
		 + C[i + (cl-1)*rw]*(
		      T[i+1 + rw*(cl-1)] + T[i-1 + rw*(cl-1)]
		      + T[i + rw*(cl-2)]
		      - 3.*T[i + rw*(cl-1)]
		      );
       }
#pragma omp parallel for private(j)
       for (j = 1; j < cl-1; ++j)
       {
	    Tdt[j*rw] = T[j*rw]
		 + C[j*rw]*(
		      T[1 + rw*j]
		      + T[rw*(j+1)] + T[rw*(j-1)]
		      - 3.*T[rw*j]
		      );
       } 
#pragma omp parallel for private(j)
       for (j = 1; j < cl-1; ++j)
       {
	    Tdt[rw-1 + j*rw] = T[rw-1 + j*rw]
		 + C[rw-1 + j*rw]*(
		      T[rw-2 + rw*j]
			   + T[rw-1 + rw*(j+1)] + T[rw-1 + rw*(j-1)]
		      - 3.*T[rw-1 + rw*j]
		      );
       }
       
       Tdt[0] = T[0]
	    + C[0]*(
		 T[1]
		 + T[rw]
		 - 2.*T[0]
		 );
       Tdt[rw-1] = T[rw-1]
	    + C[rw-1]*(
		 T[rw - 2]
		 + T[2*rw-1]
		 - 2.*T[rw-1]
		 );
       Tdt[rw*(cl-1)] = T[rw*(cl-1)]
	    + C[rw*(cl-1)]*(
		 T[rw*(cl-1)+1]
		 + T[rw*(cl-2)]
		 - 2.*T[rw*(cl-1)]
		 );
       Tdt[rw*cl-1] = T[rw*cl-1]
	    + C[rw*cl-1]*(
		 T[rw*cl-2]
		 + T[rw*(cl-1)-1]
		 - 2.*T[rw*cl-1]
		 );

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

