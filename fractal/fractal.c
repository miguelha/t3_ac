#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include "fractalfuncs.h"

#ifdef _OPENMP
    #include <omp.h>
#else
    #define omp_get_num_threads() 0
    #define omp_get_thread_num() 0
#endif

void Generate(struct IMG * img, int maxiter){
    int j=0;
    int scrsizex,scrsizey;
    scrsizex=img->cols;
    scrsizey=img->rows;

    #ifdef _OPENMP
    #pragma omp parallel
    {
        #pragma omp for collapse(2) schedule(static,5) // divide all rows of an image evenly, 5 by 5, between threads
        for(int j = 0; j < scrsizey; j++){
            for(int i = 0; i < scrsizex; i++){
                julia(img, i, j, maxiter);
            }
        }
    }
    #else
    for(int j = 0; j < scrsizey; j++){
        for(int i = 0; i < scrsizex; i++){
            julia(img, i, j, maxiter);
        }
    }
    #endif

}

void difuse(struct IMG * imgin, int nepocs, float alpha){
    struct IMG * temp,*imgnew;
    int i;
    char filename[80];
    
    imgnew=(struct IMG *) malloc(sizeof(struct IMG));
    imgnew->rows=imgin->rows;
    imgnew->cols=imgin->cols;
    imgnew->pixels=(PIXEL *)malloc(imgnew->cols*imgnew->rows*sizeof(PIXEL));
    for (i=1;i<=nepocs;i++){
	// apply diffusion for each color channel, NEVER mixing them...
	
	// YOUR CODE HERE
	
	
	sprintf(filename,"julia%04d.pgm",i);
	saveimg(imgnew,filename);
	temp=imgin;
	imgin=imgnew;
	imgnew=temp;
    }
}
    
int main(int argc, char ** argv){
    #ifdef _OPENMP
    double t1, t2;
    #else
    clock_t t1,t2;
    #endif
    int resx,resy;
    struct IMG * img;
    int nepocs=0;
    float alpha=0;

	
    if (argc==1){
	resx=640;
	resy=480;
    } else if ((argc==3)||(argc==5)){
	resx=atoi(argv[1]);
	resy=atoi(argv[2]);
	if(argc==5){
	    nepocs=atoi(argv[3]);
	    alpha=atof(argv[4]);
	    if (alpha<0.0 || alpha>1.0){
		printf("Alpha tem de estar entre 0 e 1\n");
		exit(1);
	    }
	}
    } else {
	printf("Erro no número de argumentos\n");
	printf("Se não usar argumentos a imagem de saida terá dimensões 640x480\n");
	printf("Senão devera especificar o numero de colunas seguido do numero de linhas\n");
	printf("Adicionalmente poderá especificar o numero de epocas de difusao e o factor de difusao,\\ caso contrario serao considerados como 0.");
	printf("\nExemplo: %s 320 240 \n",argv[0]);
	printf("\nExemplo: %s 320 240 100 0.5\n",argv[0]);
	exit(1);
    }
    img=(struct IMG *)malloc(sizeof(struct IMG));
    
    img->pixels=(PIXEL *)malloc(resx*resy*sizeof(PIXEL));
    img->cols=resx;
    img->rows=resy;
    
    #ifdef _OPENMP
    char filename[80];
    double elapsed_time;
    double total_elapsed_time = 0;

    for(int i = 0; i < 100; i++){
        t1=omp_get_wtime();
        Generate(img, i);
        t2=omp_get_wtime();

        elapsed_time = (t2-t1);
        total_elapsed_time += elapsed_time;

        printf("Julia Fractal gerado em %6.3f secs com %d iterações\n", elapsed_time, i+1);
        //	mandel(img,resx,resy);
        sprintf(filename, "julia/julia_%02d.pgm", i);
        saveimg(img, filename);
    }

    printf("\nTempo total: %6.3f secs (com OpenMP)\n", total_elapsed_time);
    #else
    char filename[80];
    double elapsed_time;
    double total_elapsed_time = 0;

    for(int i = 0; i < 100; i++){
        t1=clock();
        Generate(img, i);
        t2=clock();

        elapsed_time = (((double)(t2-t1))/CLOCKS_PER_SEC);
        total_elapsed_time += elapsed_time;

        printf("Julia Fractal gerado em %6.3f secs com %d iterações\n", elapsed_time, i+1);
        //	mandel(img,resx,resy);
        sprintf(filename, "julia/julia_%02d.pgm", i);
        saveimg(img, filename);
    }

    printf("\nTempo total: %6.3f secs (sem OpenMP)\n", total_elapsed_time);
    #endif



    if(nepocs>0)
    difuse(img,nepocs,alpha);
}
										  
