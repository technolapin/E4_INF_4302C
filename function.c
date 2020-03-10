/*******************************************************************/ 
/*  INF-4101C - Travaux pratiques 2
 *  Lecture / écriture d'une matrice 2D pour des tests
 *  des techniques HPC, on utilise le moyen d'une image pour visualiser le résultat
 *  Eva Dokladalova,  2019
 */

#include "function.h"

//------------------------------------------------------------------
// ONLY FOR PGN IMAGES type P2 !!!
//------------------------------------------------------------------
//open image file and access data, allocate an image with file data; 
float * readimg(char *filename, int *rs, int *cs, int *vs)
{
  float *im; // input data
  char *rd;
  int c,v;
  int rw; //row size
  int cl; //column size
  char buffer[BSIZE];
  FILE *fd = NULL;
  
  // open input file 
  fd = fopen(filename,"r");
  if (fd==NULL)
    {
      fprintf(stderr, "Input image reading failed)\n");
      return (0);
    }
 
  // verify file type BLACK ADN WHITE ASCII PGN = P2
  rd = fgets(buffer, BSIZE, fd);
  if (!rd)
    {
      fprintf(stderr, "Reading failed (%s)\n", filename);
      return 0;
    }
  if ((buffer[0] != 'P') || (buffer[1]!= '2'))
    {   fprintf(stderr,"%s : invalid image format\n", filename);
      
      return (0);
    }
  do
    { // comment in pgn file ?
      rd = fgets(buffer, BSIZE, fd); /* commentaire #*/
      if (!rd)
	{
	  fprintf(stderr, "%s: fgets returned without reading\n", filename);
	  return 0;
	}
      
    } while (!isdigit(buffer[0]));
  
  // scan image size rw = number of rows; cl = number of columns
  c = sscanf(buffer, "%d %d", &rw, &cl);
  rd = fgets(buffer, BSIZE, fd);
  // scan maximal pixel value, should be == 255
  c = sscanf(buffer, "%d", &v);
  //data allocation and initialization
  im = (float *) calloc (rw*cl,sizeof(float));
  if (im==NULL)
    {
      fprintf(stderr, "Data allocation (failed %d) bytes)\n", rw*cl);
      return (0);
    }
  int i;
  // data initialization
  int N = rw*cl*sizeof(uint8);
  for (i=0;i<N;i++)
    {
      fscanf(fd, "%d", &c);
      im[i]=(float) c;
    }
  // close file and return pointer to the iamge data
  fclose(fd);  
  *cs = cl;
  *rs = rw;
  *vs = v;
  return(im);
}

//------------------------------------------------------------------
// ONLY FOR PGN IMAGES type P2 !!!
//------------------------------------------------------------------
// write image in a file
//----------------------------------------------------------------
void writeimg(char *filename, float *im, int rs, int cs, int vs)
{
  int cl;
  int rw;
  int v;
  int N;
  FILE *fd_out;
  int i;
  
  cl = cs;
  rw = rs;
  v = vs;
  N = cl*rw;
 
  fd_out = fopen(filename,"w");
  if (!fd_out)
    {
      fprintf(stderr, "%s: cannot open file\n", filename);
      exit(0);
    } 
  fputs("P2\n", fd_out);
  fprintf(fd_out, "# comment\n");
  fprintf(fd_out, "%d %d\n", rw,cl);
  fprintf(fd_out, "%d\n", v);
  for (i = 0; i < N; i++)
    { 
      fprintf(fd_out, "%d \n", (int) (im[i]+0.5));
    }      
  fclose(fd_out);
}


