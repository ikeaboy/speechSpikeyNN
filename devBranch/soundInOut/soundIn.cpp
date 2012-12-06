/*
 * f0contour.c
 *
 *  Created on: 15-Oct-2009
 *  Updated on : 28-Jun-2010
 *      Author: antoine
 */

#include "f0contour.h"
#include <fcntl.h>

#define data 0
#define method 2

int padd=1;
long double sampling = 48000.;
int chk_size = 1024, step = 256;

/* Main program : */
int main(int argc, char * argv[])
{
/* variable creation and initialisation ... */
	/* saved data in files */
FILE * f_in = NULL;
FILE * fsound = NULL;
FILE * ffilter = NULL;
FILE * filteredsignal = NULL;
FILE * spectrum = NULL;
FILE * sinus = NULL;


int k=0;
short int * raw;
char file[25];
char energy_filename[25] = {'e','n','e','r','g','y','.','d','a','t'};
double freq = 0.;
int length = -1;
double * filter = NULL;
double * filtered_output = NULL;
int i = 0,size = 0,j = 0;
double * chunk_fft;
int winner = -1;
char outwav[25];
double correction = 0.;
double time = 0;
double * sin_signal;
double * res;
double * in = NULL;
double * powspec;
double * output_array;
fftw_complex * fftof_signal;
int * zeros;
double * frequencies;
struct stat st;
long double * noise = NULL;
long double * mean = NULL;

//correction=(double)sampling/((double)chk_size*padd); /* corrective factor to convert index in frequency */

	/* Looking for arguments ... */
	if(argc < 2)
	{
		printf("ERR : Missing arguments \n (Gimme a file to feed plz *_*) \n--- Path to wav file ----\n CIAO..\n");
		exit(0);
	}

	/* Checking the file format and converting to raw ... */
	if(strstr(argv[1], ".wav") != NULL)
	{
		printf("Opening Wav File: %s  \n",argv[1]);
		f_in = fopen(argv[1],"r");
		size = wavopen(f_in);
		memset(file, 0, 25);
		strcpy(file, argv[1]);
		printf("size  of file is = %d\n", size);
		raw = (short int *)malloc(sizeof(short int)*size);
		memset(outwav, 0, 25);

		/* Creating the name of the new file : wavNameOfTheOriginal.dat */
		strcpy(outwav, "data/wav");
		strcat(outwav, strtok(argv[1],"."));
		strcat(outwav, ".dat");
		/* copying the content of file to ... */
		printf("wav to file:%s \n",outwav);
		wavtoarray(file, &raw, outwav);
		fclose(f_in);
	}
	else
	{
		printf("Opening Raw file File: %s  \n",argv[1]);

		stat(argv[1], &st);
		size = st.st_size;
		printf("File size should be %d, then should be /4 %d\n ", size, size/8);
		f_in=fopen(argv[1],"r");
		memset(file,0,25);
		strcpy(file,argv[1]);
		size/=4;
		printf("size  of file is = %d\n",size);
		raw=(short int *)malloc(sizeof(short int)*size);
		memset(outwav,0,25);
		strcpy(outwav,"data/raw");
		strcat(outwav,strtok(argv[1],"."));
		strcat(outwav,".dat");
		printf("Raw  to file:%s \n",outwav);
		size = rawtoarray(file, &raw, outwav);
		fclose(f_in);
	}


	/* =============================================================================== */
	/* ========== Allocating memory for dynamic use ========== */

	chunk_fft = (double *)malloc(sizeof(double)*chk_size);
	output_array = (double *)malloc(size*sizeof(double));

	/* ============ Generating Sinusoidal signal ============ */
	sinus = fopen("data/sinus_signal.dat","w+");

	/* srand48(1);  Unused */
	freq = 500; /* Frequency*/
	length = 16384; /* length of the generated signal */
	sin_signal = (double *)malloc(length*sizeof(double));

	for( i = 0; i < length; i++)
	{
		sin_signal[i] = 5000.*sin(2*M_PI*(long double)i*freq/(sampling)); /* first part of the signal*/
	//	sin_signal[i] += 5000.*sin(2*M_PI*(long double)i*10.*freq/(sampling)); /* added sinus */
	//	sin_signal[i] += 5000.*sin(2*M_PI*(double)i*3.*freq/((double)sampling));
	//	sin_signal[i]+= 5000.*sin(2*M_PI*(double)i*3.9*freq/((double)sampling));
	//	printf("%d %f %f\n", i, sin_signal[i], freq);

		fprintf(sinus,"%d %f \n", i, sin_signal[i]); /* writing data in the file */
	}
	fclose(sinus);
	/* ================== Signal generated ================== */

	/**padded_raw=(short int *)malloc(3*size*sizeof(short int));
	if(padd>1)	zero_padd(raw,size,padd,padded_raw);*/
	//correction/=padd;

	/* Opening files to write "sinres" signal (unused) and filtered signal*/
	fsound = fopen("data/sinres.dat","w+");
	ffilter = fopen("data/filtered.dat","w+");

	/* memory allocation */
    filter = (double*)malloc(chk_size*factor*sizeof(double));
    zeros = (int *)malloc((int)(size)/step*sizeof(int));
    frequencies = (double *)malloc((double)(size)/step*sizeof(double));
    res = (double *)malloc(sizeof(double)*(int)(size)/step);
    noise = (long double*)malloc(sizeof(long double)*(int)(size)/step);
    mean = (long double*)malloc(sizeof(long double)*(int)(size)/step);
    in = (double *)malloc(sizeof(double)*(size));


	/* ====================================================== */

    f_in = fopen("data/doublewav.dat","w+");
    j = 0;

    for(i = 0 ; i < size ; i++)
    {
    	in[i] = (double)raw[i];
    	if((i+1)%(step) == 0)
    	{
				//if(isequal(noise[k],0.))printf("noise is NULL j=%d k=%d \n",j,k);
				k++;
				if(k == size/step) break;
				noise[k]=0.;

		}
		// computing volume
		noise[k]=noise[k]+((fabs(in[i])*0.1));
    }

/**
	FIR(in,size);
	for(i=0;i<size;i++){
	    	fprintf(f_in,"%d %f \n",i,fabs(in[i]-(double)raw[i]));
	}*/

	 fclose(f_in);
	/* ====================================================== */
    printf("min quef =%d max=%d\n",(int)(sampling/50.),(int)(sampling/800.));


    /* ================= Loop on data chunks ================= */
    for(i=0; i < size-1-step; i+=step)
    {
		 //printf("i=%d i+chk=%d \n",i,i+chk_size);
		 switch(data)
		 {
			 case 0: memcpy(chunk_fft, &(in[i]), sizeof(double)*chk_size);
			 break;

			 /* active data processing */
			 case 1: memcpy(chunk_fft, &(sin_signal[i]), chk_size*sizeof(short));
			 break;

			 default:
			 break;
		 }

		 mean[j]=average(chunk_fft,chk_size);

		 switch(method)
		 {
			 case 0: winner = dofft(&chunk_fft,chk_size);
					 correction = (double)sampling/(double)(chk_size*factor);
					 freq = (double)winner * correction;
			 break;	// Cepstrum analysis.....

			 case 1:
					 //cut_off(chunk_fft,chk_size,600,filter);
					 //for(j=0;j<chk_size;j++) fprintf(ffilter,"%d %f \n",i+j,filter[j]);
					// winner=d_fastcepstrum(filter,chk_size);
					 winner = fastcepstrum(chunk_fft,chk_size,mean[j]);
					 correction = 1./sampling;
					 freq = sampling/winner;
			break;

			default : /* "active method" */

			break;
		}

		 /* results of previous method */
		// frequencies[j] = zero_crossing_rate(chunk_fft, chk_size);
		//zeros[j] = zero_crossing(chunk_fft, chk_size);

		 if(winner<=0)
		 {
			 freq=0.;
		 }

		 res[j]=freq;
		 j++;

	}
	/* ================= End of the loop ==================== */

    /* ================ Loop 2 on data chunks =============== */
    /* the step is equal to chk_size */

    /* Array containing the result of the FFT ; needs to be "padding factor" (=4) times bigger than the chunk of input*/
    fftof_signal = (fftw_complex *)fftw_malloc(factor*chk_size*sizeof(fftw_complex));
    printf("memoire allouee : %f,\nchk_size : %d,\nsizeof(fftw_conmplex) : %d\n", (double)4*chk_size*sizeof(fftw_complex), chk_size, sizeof(fftw_complex));
    /*Array containing the power spectrum of the signal ; same size as the FFT */
    powspec = (double *)calloc(factor*chk_size, sizeof(double));
    /* Array containing the filtered time signal (in theory, the input without noise) ; same size as input */
    filtered_output = (double *)fftw_malloc(chk_size*sizeof(double));
    /* File containing FFT and PS data to plot in gnuplot */
    spectrum = fopen("data/spectrum.dat","w");
    filteredsignal = fopen("data/nao_speech.dat","w");

    for(i = 0*chk_size; i < size-1-chk_size; i+=chk_size) //size-1-chk_size
    {
    	/* ca a l'air de marcher sur 1 echantillon !*/
		/* Get data from input */
    	memcpy(chunk_fft, &(in[i]), sizeof(double)*chk_size);
    //	printf("chunk_size : %d, samples : %f, Iteration : %d / %d, k = %d \n", chk_size, (float)size/chk_size, i, size, k);
    	dofft_r2c(&chunk_fft, fftof_signal, chk_size);

    	/* cutting "bad frequencies" */
    //	multifonction_filter(fftof_signal, chk_size, 0, 2000, 1);
    //	multifonction_filter(fftof_signal, chk_size, 10000, 14000, 1);
    //	multifonction_filter(fftof_signal, chk_size, 22000, 24000, 1);

    	for (k = 0; k < factor*chk_size; k++)
        {
        //	printf("Here I am = %d - %d \n", i, k);
    		freq = k*sampling/(factor*chk_size);
        	powspec[k] += sqrt(fftof_signal[k][0]*fftof_signal[k][0] + fftof_signal[k][1]*fftof_signal[k][1]) / 50; //(double)chk_size / size *
        	fprintf(spectrum, "%d %f %f %f %f \n", k, freq, fftof_signal[k][0], fftof_signal[k][1], powspec[k]);
		}

    	dofft_c2r(&fftof_signal, filtered_output, chk_size);
        /* Recording the new signal (fft back) without excluded frequencies ; We want only a chk_size signal */
        for(k=0; k < chk_size; k++)
        {
       		//printf("Marqueur recording 2nd file %d\n", k);
    		time = (double)(i+k)*(chk_size)/sampling;
       		fprintf(filteredsignal, "%d %f %f \n", i+k, time, filtered_output[k]);
       		output_array[i+k] = filtered_output[k]; /* recording data to convert them as a raw and play with aplay */
		}
    }
    dump_binary(output_array, chk_size);


	/* ============= Other way to process datas ============= */
	/* We try to filter the input signal with fft forward and backward */
    /* ====================================================== */

    /* Data have been copied in chunk_fft, but we need the whole signal, so let's use sin_signal as input */
    /* Preparing the output file : */

    /* Size of the signal is "length" */
	/* Computing energy of signal helps us to separate noise and speech ... see that later ... */
//    energysize = energy_computing(sin_signal, energy_filename, length);
 //   printf("Energy_size: %d\n", energysize);
	/* --- Energy Computed --- */

										/* Size allocation must be modified to take care about padding rate */

//    fftof_signal = (fftw_complex *)fftw_malloc(4*length*sizeof(fftw_complex)); /* padding factor = 4 */

    /* Doing FFT on signal */
/*
		dofft_r2c(&sin_signal, fftof_signal, length);

		multifonction_filter(fftof_signal, length, 0, 3980, 1);

		dofft_c2r(&fftof_signal, sin_signal, length);
*/
    /* Doing FFT backward on signal */

    printf("num of sample = %d  expected = %d\n", j, (int)(size)/chk_size);
	 //running_average(res,j);
/*	for(i=0;i<(int)(size)/step;i++)
	{
		median(&res[i],10);
	}
	*/

	//running_average(res,j);
//	printf(" zeros crossing limit %f %d  \n",(double)(chk_size/6.),(int)(chk_size/6.));

/*	for(i=0;i<j;i++)
	{
		if(zeros[i]<(double)(chk_size/6.))
		fprintf(fsound, "%d %f %f %d %d %Lf\n", i*step, (float)(i*step)/sampling, res[i], i, zeros[i], noise[i]);
	}
*/

	fclose(ffilter);
	fclose(fsound);
    fclose(filteredsignal);
    fclose(spectrum);

    free(powspec);
	free(filtered_output);
	free(fftof_signal);
	free(in);
	free(noise);
	free(mean);
	free(zeros);
	free(frequencies);
	free(res);
	free(chunk_fft);
	free(filter);
	free(sin_signal);
	free(output_array);

	return 0;
}
