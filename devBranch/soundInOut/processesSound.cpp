/*
 * processes.c
 *
 *  Created on: 15-Oct-2009
 *      Author: antoine
 */
//#define SAVE_ALL
#include "sub_processes.h"
int on=0;
int factor = 4;/** Padding ration*/
int sub_cpt=0;
double PI=3.14159265358979323846;


typedef struct {
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
    char subChunkId[4];
    uint32_t subChunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	char subChunkId2[4];
	uint32_t subChunkSize2;
	} wav_header;


wav_header get_wav_header(FILE *in){

wav_header head;
int r=0;
	r=fread(head.chunkId, 1, 4, in);
	r=fread(&head.chunkSize, 4, 1, in);
	r=fread(head.format, 1, 4, in);
	r=fread(head.subChunkId, 1, 4, in);
	r=fread(&head.subChunkSize, 4, 1, in);
	r=fread(&head.audioFormat, 2, 1, in);
	r=fread(&head.numChannels, 2, 1, in);
	r=fread(&head.sampleRate, 4, 1, in);
	r=fread(&head.byteRate, 4, 1, in);
	r=fread(&head.blockAlign, 2, 1, in);
	r=fread(&head.bitsPerSample, 2, 1, in);
	r=fread(head.subChunkId, 1, 4, in);
	r=fread(&head.subChunkSize2, 4, 1, in);
    return head;

}
/**
void clear_header(wav_header *w){
	w->chunkId[4] = 0;
    w->format[4] = 0;
    w->subChunkId[4] = 0;
    w->subChunkId2[4] = 0;
}
*/
void pheader(wav_header *w){
int file_length; // in seconds
	if (w->audioFormat == 1)
		printf("PCM format\n");
	else
	        printf("Unknown format\n");

    if (w->numChannels == 1)        printf("Mono\n");
    else if (w->numChannels == 2)        printf("Stereo\n");
    printf("Sample Rate = %d\n", w->sampleRate);
    printf("%d bytes per sample\n", w->bitsPerSample / 8);
// how long the file is in seconds
    file_length = w->subChunkSize2 / w->byteRate;
    printf("file length is %d mn %d s\n", file_length/60, file_length%60);
}


int wavtoarray(char * file_in, short int ** array, char * file_out)
{
int i=0,r;
short int sample;
FILE * in = NULL;
FILE * out = NULL;
short * temparr;
	printf("wav to array thing filein = %s out=%s \n",file_in,file_out);
	in = fopen(file_in,"r");
	out = fopen(file_out,"w+");
	if(out == NULL) perror("output file  is NULL\n");

	get_wav_header(in);
	temparr=*array;

	while(feof(in) == 0){
		r=fread(&sample, 2, 1, in);
		//printf("r=%d i=%d sample=%d\n",r,i,sample);
		r=fprintf(out,"%d %d \n",i,sample);
		//printf("r=%d\n",r);
		temparr[i]=sample;
		//printf("array goes: %d\n",*array[i]);
		i++;

	} printf("done! nmsample=%d\n",i);
	fclose(in);
	fclose(out);
	return i;//length of the array
}

int rawtoarray(char * file_in,short int ** array,char * file_out){
int i=0,r;
short int sample;
FILE * in=NULL;
FILE * out=NULL;
short * temparr;

	printf("Raw to array filein=%s out=%s \n",file_in,file_out);
	in=fopen(file_in,"r");
	out=fopen(file_out,"w+");
	temparr=*array;

	while(feof(in) == 0){
		r=fread(&sample, 2, 1, in);
		temparr[i]=sample;
		r=fprintf(out,"%d %d \n",i,sample);
		i++;
		r=fread(&sample, 2, 1, in);
		r=fread(&sample, 2, 1, in);
		r=fread(&sample, 2, 1, in);

	} printf("done! nmsample=%d\n",i);
	fclose(in);
	fclose(out);
	return i;//length of the array
}

int wavopen(FILE * in){
wav_header w;

		w = get_wav_header(in);
		//clear_header(&w);
		pheader(&w);
		return w.subChunkSize2/2;

}

void cut_off(double * array,int _size,int cutoff,double * final){
int i=0,j=0;
double *  in=NULL;
//fftw_complex * out=NULL;
double * out=NULL;
double * temparr;
int stepcut=(int)((double)(cutoff*_size)/(double)sampling);
int size=_size*factor;
fftw_plan audio_plan;
fftw_plan back;

	in=(double *)fftw_malloc(size*sizeof(double));
	temparr=array;

	for(i=0,j=0;i<size;i++,j++){
		if(i<_size)	in[i]=hamming(i,_size)*(double)temparr[i];
		else in[i]=0;
	}

	//out=(fftw_complex *)fftw_malloc(size*sizeof(fftw_complex));
	out= (double *)fftw_malloc(size*sizeof(double));
	//audio_plan=fftw_plan_dft_r2c_1d(size, in, out, FFTW_MEASURE);

	audio_plan=fftw_plan_r2r_1d(size, in, out, FFTW_R2HC, FFTW_MEASURE);
	fftw_execute(audio_plan);

	/**out[0][0]=0.;
	out[0][1]=0.;*/
	for(i=stepcut+1;i<(size/2)-1;i++){
			//out[i][1]=out[i][0]=0.;
			out[i]=0.;
	}

	//back=fftw_plan_dft_c2r_1d(size,out,final,FFTW_MEASURE);
	back=fftw_plan_r2r_1d(size, out,final, FFTW_HC2R, FFTW_MEASURE);
	fftw_execute(back);
	//printf("Done....\n");
	fftw_destroy_plan(audio_plan);
	fftw_destroy_plan(back);
	fftw_free(in);
	fftw_free(out);

	return;
}

int dofft(double ** array,int _size){
int i=0,j=0;
double *  in;
double * out=NULL;
double * temparr;
int size=_size*factor,winner=-1;
fftw_plan audio_plan;

	in=fftw_malloc(size*sizeof(double));

	temparr=*array;
	//cut_off(temparr,chk_size,800,in);

	for(i=0,j=0;i<size;i++,j++){
		if(i<_size)	in[i]=hann(i,_size)*temparr[i];
		else in[i]=0;

	}
	out=fftw_malloc(size*sizeof(double));
	audio_plan=fftw_plan_r2r_1d(size, in, out, FFTW_R2HC, FFTW_MEASURE);

	//audio_plan=fftw_plan_dft_r2c_1d ( size, in, out, FFTW_ESTIMATE );
	fftw_execute(audio_plan);
	//printf("Done....\n");
	winner=freq_winner(out, size);
	fftw_destroy_plan(audio_plan);
	free(in);
	free(out);
	return winner;
}

/* ==================================================================================== */
int dofft_r2c(double ** array, fftw_complex * output, int _size)
{
	/* The fonction computes fft forward of the signal "array" given in argument. The result is registered in a file.
	 * */

int i=0;
double * in;			/* array with the real input */
//fftw_complex * out;		/* array with the complex output*/

double * temparr;
int size = _size*factor; /* Padding on the signal */
int winner = -1;
fftw_plan audio_plan;	/* the plan to compute fft */
FILE * results;			/* results file */

double correction = (double)sampling/(double)(_size*factor); /* corrective factor that convert index into frequency */
double freq = -1;
double powes=0.;
	/* Memory allocation */
	in = (double *)fftw_malloc(size*sizeof(double));
	results = fopen("data/fft_computed.dat","w");

	if(in == NULL || results == NULL)
	{
		printf("Allocating memory or opening file error\n");
		exit(1);
	}
	temparr = *array;

	for(i=0; i<size; i++)
	{
		/* Windowing by Hann window -- NOT ACTIVE (rectangular windowing) !*/
		if(i < _size)
		{
			//in[i] = tukey(i, _size, 0.8)*temparr[i];
			in[i] = hamming(i, _size)*temparr[i];
			//in[i] = hamming(int i,int size)*temparr[i];
			//in[i] = temparr[i];
		}
		else
		{
			in[i] = 0;
		}
	}

	/* processing the signal */
/*	out = (fftw_complex *)fftw_malloc(size*sizeof(fftw_complex));
	if(out == NULL)
	{
		printf("Allocating memory error\n");
		exit(1);
	}*/
	audio_plan = fftw_plan_dft_r2c_1d(size, in, output, FFTW_ESTIMATE );
	fftw_execute(audio_plan);

	/* writing results */
	for(i = 0; i < size; i++) /* i is "frequency index" */
	{
		freq = (double)i * correction;
		powes = sqrt(output[i][0]*output[i][0]+ output[i][1]*output[i][1]);
		fprintf(results, "%d %f %f %f %f\n", i, freq, output[i][0], output[i][1], powes);
	}

	fclose(results);
	fftw_destroy_plan(audio_plan);
	fftw_free(in);
	return winner;
}

int dofft_c2r(fftw_complex **array, double * output, int _size)
{
int i=0;
fftw_complex * in;
double * out;
int size=_size*factor;
int winner = -1;
fftw_plan audio_plan;
FILE * results;
double correction = (double)(_size*factor)/(double)sampling; /* convert freq index into time index */
double time = -1;
	//printf("Marqueur C2R : %d \n", _size);

	in = *array;
	out = (double *)fftw_malloc(size*sizeof(double));
	if(in == NULL || out == NULL)
	{
		printf("Allocating memory error\n");
		exit(1);
	}

	results = fopen("data/fft_backward.dat","w");

	audio_plan=fftw_plan_dft_c2r_1d( size, in, out, FFTW_ESTIMATE );
	fftw_execute(audio_plan);
	for(i = 0; i < _size; i++) /* i is now "time index" ... */
	{
		time = (double)i * correction;
		output[i] = out[i]/(_size*factor*hamming(i, _size)); // *hann(i, _size) Dividing output by hann windows helps us to get a better signal
		fprintf(results, "%d %f %f %f\n", i, time, out[i], output[i]);
	}

	fclose(results);

	fftw_destroy_plan(audio_plan);
	fftw_free(out);
//	printf("size : %d\n", _size);
	return winner;
}

int freq_winner(double * in,int size)
{
int i=0;
int winner=-1;
double max=0.;
double temp=0.;
double sample=0.;
int start=(int)((double)(size*40)/(sampling));
int topfreq=(int)((double)(size*1000)/(sampling));
FILE * fceps=NULL;
char name[100];
char num[9];


		memset(name,0,100);
		memset(num,0,9);
		strcpy(name,"Ceps");
		sprintf(num,"%d",sub_cpt);
		strcat(name,num);
		strcat(name,".dat");
		fceps=fopen(name,"w+");
		//in[i]=log10(fabs(in[i]*in[i]/(size*size)));
			for(i=start;i<topfreq;i++)
			{
			sample=in[i];
			temp=sqrt(sample * sample);
			fprintf(fceps,"%Lf %f \n",(double)(i)*(sampling/size),temp);
			if(max<temp){
					max=temp;
					winner=i;
			}
		}
		sub_cpt++;
		return winner;
}

int fastcepstrum(double *array,int size,long double mean)
{
int i=0;
double *  in;
fftw_complex * out;
//double * out;
double * powceps;
double * res;
double max=-99999999999.,tp=0.;
double * temparr;
int winner=-1;
fftw_plan audio_plan;
fftw_plan b_audio_plan;
int start_que=(int)(sampling/1000.);
int max_quef=(int)(sampling/50.);
int f_size=size*factor;
int max_que=max_quef<f_size?max_quef:f_size;


//	printf("fast_cepstrum...\n");
	in=fftw_malloc(f_size*sizeof(double));
	temparr=array;
	/**ZERO PADDING + Hamming window*/
	for(i=0;i<f_size;i++){
		if(i<size)	in[i]=hamming(i,size)*(temparr[i]-mean);
		else in[i]=0.;

	}
	//running_average(in,size);
	/*complex*/
	out=fftw_malloc(f_size*sizeof(fftw_complex));
	powceps=fftw_malloc(f_size*sizeof(double));
	res=fftw_malloc(f_size*sizeof(double));
	audio_plan=fftw_plan_dft_r2c_1d ( f_size, in, out, FFTW_MEASURE);
	fftw_execute(audio_plan);
	for(i=0;i<f_size/2;i++){
		tp=((out[i][1]*out[i][1])+(out[i][0]*out[i][0]));
		powceps[i]=powceps[f_size-i-1]=log((tp));
		if(isnan(powceps[i])>0){
			powceps[i]=0.;
		}
	}
	i=0;
	b_audio_plan=fftw_plan_r2r_1d(f_size, powceps,res, FFTW_R2HC,FFTW_MEASURE);
	fftw_execute(b_audio_plan);
	max=-99999.;
	for(i=start_que;i<max_que;i++){
		/* if ceps complex.....*/
		tp=fabs(res[i])/f_size;
		if(tp> max){
			max=tp;
			winner=i;
		}
	}
	fftw_free(out);
	fftw_free(in);
	fftw_free(powceps);
	fftw_free(res);
	fftw_destroy_plan(b_audio_plan);
	fftw_destroy_plan(audio_plan);
	//temparr=NULL;
	sub_cpt++;
	return winner;
}


double hamming(int i,int size){
double res=0.;

	res=0.54-0.46*cos(2.*M_PI*(double)i/(double)size);
	return res;
}

double hann(int i, int size){
double res=0.;

		res=0.5*(1. - cos(2.*M_PI*(double)i/(double)size));
		return res;

}

double tukey(int i, int size, float alpha)
{
	/* Tukey Window */
double res=0.;

	if ( fabs(i) > 0 || fabs(i) < alpha*size/2 )
	{
		res = 1.;
	}
	if ( fabs(i) > alpha*size/2 || fabs(i) < size / 2)
	{
		res = 0.5*(1 + cos(PI * ( fabs(i) - size / 2 ) / ((1 - alpha)*size/2) ) );
	}

	else
	{
		printf("Incorrect value %d\n", i);
		exit(1);
	}

		res = 0.5*(1. -cos(2.*M_PI*(double)i/(double)size));
		return res;

}

void zero_padd(short int * in,int size,int factor, short int * res){
short int * arr;
int i=0;
	arr=res;
	for(i=size;i<size*factor;i++){
			arr[i]=0;
	}

}

double triangle(int i, int size){
double res=0.;
float ind=(float)i;
float half=(float)(size)/2.;

	if(ind < half) res=((ind/(size))+0.5);

	else res=1. - ((ind)/size) +0.50;

	return res;

}

void bbleSort(double * tab,int size){

double temp;
int i=0;
double perm=1;

	while(perm>0){
		perm=0.;
		for(i=0;i<size;i++){

			if(tab[i]>tab[i+1]){

				temp=tab[i];
				tab[i]=tab[i+1];
				tab[i+1]=temp;
				perm=1.;
			}

		}
	};
}
void FIR(double * _in,int size)
{
double * in=NULL;
int l=34,i=0,j=0;
double a[l],temp=0;


a[0] =	0.026200252257397708;
a[1] =	0.06372465473413907;
a[2] =	-0.0353563720337165;
a[3] =	-0.01322374334537073;
a[4] =	-0.024614096199843033;
a[5] =	-0.00732751231122021;
a[6] =	0.013610129793657236;
a[7] =	0.031228459799191897;
a[8] =	0.029187973443352097;
a[9] =	0.004165347706746643;
a[10] =	-0.032483125156792254;
a[11] =	-0.05679235104118708;
a[12] =	-0.044605111563562894;
a[13] =	0.014372453415774079;
a[14] =	0.10746581726430607;
a[15] =	0.20292892146744287;
a[16] =	0.26304931777311685;
a[17] =	0.26304931777311685;
a[18] =	0.20292892146744287;
a[19] =	0.10746581726430607;
a[20] =	0.014372453415774079;
a[21] =	-0.044605111563562894;
a[22] =	-0.05679235104118708;
a[23] =	-0.032483125156792254;
a[24] =	0.004165347706746643;
a[25] =	0.029187973443352097;
a[26] =	0.031228459799191897;
a[27] =	0.013610129793657236;
a[28] =	-0.00732751231122021;
a[29] =	-0.024614096199843033;
a[30] =	-0.01322374334537073;
a[31] =	-0.0353563720337165;
a[32] =	0.06372465473413907;
a[33] =	0.026200252257397708;

	//in=(double *)malloc(sizeof(double)*size);
	in=_in;
	for(i=size-1;i>=l-1;i--)
	{
		//if(i<l-1) in[i]=_in[i];
		for(j=0;j<l;j++)
		{
			temp+=a[j]*_in[i-j];
		}
		in[i]=temp;
		temp=0.;
	}
	//memcpy(_in,in,size*sizeof(double));
}

void FIR_Nao_fan(double * _in,int size)
{
/*	try to filter Nao fan noise */
double * in = NULL;
int l = 36,i = 0,j = 0;
double a[l],temp = 0;

a[0] =	-1.0092412438819158;
a[1] =	0.3548958898234653;
a[2] =	0.08347814159540413;
a[3] =	0.04443473387965116;
a[4] =	-0.03157139757626504;
a[5] =	0.03254131606468866;
a[6] =	-0.023897887658299033;
a[7] =	0.00889450694502377;
a[8] =	-0.03908411074868484;
a[9] =	0.031767023309761104;
a[10] =	-0.06629729883644277;
a[11] =	0.047112492960693496;
a[12] =	-0.06401513233920984;
a[13] =	0.06271904446238481;
a[14] =	-0.08800312903720815;
a[15] =	0.1413572659144514;
a[16] =	-0.19988069459707725;
a[17] =	0.606513414196412;
a[18] =	0.606513414196412;
a[19] =	-0.19988069459707725;
a[20] =	0.1413572659144514;
a[21] =	-0.08800312903720815;
a[22] =	0.06271904446238481;
a[23] =	-0.06401513233920984;
a[24] =	0.047112492960693496;
a[25] =	-0.06629729883644277;
a[26] =	0.031767023309761104;
a[27] =	-0.03908411074868484;
a[28] =	0.00889450694502377;
a[29] =	-0.023897887658299033;
a[30] =	0.03254131606468866;
a[31] =	-0.03157139757626504;
a[32] =	0.04443473387965116;
a[33] =	0.08347814159540413;
a[34] =	0.3548958898234653;
a[35] =	-1.0092412438819158;

	printf("Size : %d\n", size);
	in = _in;
	for(i = size-1; i >= l-1; i--)
	{
		for(j=0;j<l;j++)
		{
			temp += a[j]*_in[i-j];
		}
		in[i] = temp;
		temp = 0.;
	}
}

fftw_complex * multifonction_filter(fftw_complex * signal, int _size, float lowfreq, float highfreq, int reverse)
{
	/* this filter puts every value of the frequency spectrum that is out of [low freq, high freq] frequency range to zero
	 * If reverse = 1, we change the band pass filter into a band reject filter ... */
int i=-1;
double correction = (double)sampling/(double)(_size*factor);
double powes = -1;
FILE * outputfile;
int size=_size*factor;

outputfile = fopen("data/filtered.dat", "w");

	if (signal == NULL)
	{
		printf("No input\n");
		exit(1);
	}

	for (i = 0; i < size; i++)
	{
		//printf("Iteration : %d\n", i);
		if(reverse == 1)
		{
			if ((correction*i >= lowfreq) && (correction*i <= highfreq))
			{
				signal[i][0] = 0;
				signal[i][1] = 0;
			}
		}

		else
		{
				if ((correction*i <= lowfreq) || (correction*i >= highfreq))
				{
					signal[i][0] = 0;
					signal[i][1] = 0;
				}
		}
		powes = sqrt(signal[i][0]*signal[i][0]+ signal[i][1]*signal[i][1]);
		fprintf(outputfile, "%d %f %f %f %f \n", i, correction*i, signal[i][0], signal[i][1], powes);
	}

	fclose(outputfile);
	return signal;
}


void median(double * arr,int ind){
//int i = 0;
double * temp;
double tosort[ind];
int length=ind;
int med=(ind%2==0?ind/2:(ind/2)+1);

	temp=arr;

	memcpy(tosort,temp,length*sizeof(double));
	bbleSort(tosort,ind);
	temp[0]=tosort[med];

}

void running_average(double * in, int size)
{
int i=0;
double * temp;
int window=5;
double sum=0.;
double t_sum=0.;

	temp=in;
	for(i=0;i<size-window;i++){

		if(i>=window){
			t_sum=temp[i-window];
			temp[i-window]=sum/window;
			sum-=t_sum;
		}
		sum+=temp[i];
	}
}

int zero_crossing(double * arr_, int size, long double mean)
{
int i = 0;
int c = 0;
double * arr=NULL;
//printf("zeros crossing\n");
	arr = &arr_[0];
//	printf("Mean : %Lf\n", mean);
	mean = 0.49;
	for(i=0;i<size-1;i++)
	{
		if( (arr[i]-mean)*(arr[i+1]-mean) < 0) c++;
	}
	return c;
}

long double zero_crossing_rate(double * arr_, int sample_size)
{
	/* this function computes zero-crossing rate and returns the fundamental frequency of the signal */
	int i = 0;
	long double rate = 0;
	long double fund_freq = 0;

	double * arr=NULL;

	sample_size = (double)sample_size; /* casting to avoid division issues */

	printf("zero_crossing rate computing ...\n");

		arr = &arr_[0];
		for(i = 0; i < sample_size-1; i++)
		{
			rate += fabs(sign(arr[i+1]) - sign(arr[i]));
		}
		printf("rate (sum) : %Lf\n", rate);
		if(rate == 0)
		{
			printf("No zero crossing ... check the offset of the signal\n");
			exit(1);
		}
		rate = rate / 2 -1; /* number of intervals between two zeros */
		printf("rate (division by 2 minus 1): %Lf\n", rate);
		rate = sample_size / rate; /* average "point period" */
		printf("rate (sample_size divided by rate): %Lf, sampling = %Lf\n", rate, sampling);
		rate = rate / sampling; /* average period */
		printf("rate (rate divided by sampling freq): %Lf\n", rate);
		fund_freq = 1 / rate; /* approximation of fundamental frequency */
		printf("fund_freq approximation : %Lf \n -------------- \n", fund_freq);
		return fund_freq;
}

long double average(double * arr_,int size){
int i=0;
long double c=0.;
double * arr=NULL;
	//	printf("Average\n");
		arr = &arr_[0];
		for(i=0;i<size;i++)
		{
			if( isdiff(arr[i], 0.) )
				{
					c += arr[i];
				}
		}
		c/=size;
		return c;
}





int cepstrum(short *in,int size){
int i,j,k;
double  ceps[size];
double magnitude=0.,tempceps=0.,max=0.;
double ind=0.;
int start_que=30;
int max_que=320;
int winner=-1;


	printf("Cepstrum \n");
	for(i=start_que;i<max_que;i++){

		for(j=0;j<size;j++){
			magnitude=0.;
			for(k=0;k<size;k++){
				ind=(double)in[k]* (double)hamming(k,size);
				magnitude+=ind*cos(2.*M_PI*(double)(j)*(double)(k)/(double)(size));
			}
			tempceps+=log(fabs(magnitude))*cos(2.*M_PI*(double)(j)*(double)(i)/(double)(size));
		}
		if(tempceps==0) printf("Error somewhere....\n");

		ceps[i]=fabs(tempceps)/(double)(size);

		tempceps=0.;

		if(ceps[i]>max){
			max=ceps[i];
			winner=i;
		}
	}
	printf("Done winner=%d max=%f\n",winner,max);
	return winner;
}

int isdiff(float x, float y)
{
  return (fabs(x-y)>1e-30);
}


/* ========================================================================================== */

int energy_computing(double * input, char * output_filename, int inputsize)
{
	/* Give to the fonction an array of value, it comnputes energy then record it in a file, for plotting */
	/* Variables initialisation */
	long double* global_energy;
	int i = 0, j = 0;
	int numberof_frame = -1;
	long double energy_1frame = 0;
	int frame_size = 128;
	FILE * energyof_signal ;
	numberof_frame = inputsize / frame_size;
    energyof_signal = fopen(output_filename,"w+");

	/* Checking input size value */
	printf("--------------\ntaille fichier = %d \n", inputsize);

	/* allocating "intern" memory */
	global_energy = (long double*)malloc(numberof_frame*sizeof(long double)); //= 2048/256 = 8 for the sinus
	/* verifying allocation and input */
	if (input == NULL || global_energy == NULL)
	{
		printf("Error in allocation : missing input or allocation fail !\n");
		exit(1);
	}

	/* Ok for data, now let's compute ! */
	printf("Energy computing ... \n");

	for( i = 0 ; i < numberof_frame  ; i++)
	{
		energy_1frame = 0;

		for (j = i*frame_size ; j < (i+1)*frame_size -1 ; j++)
		{
		/*	compute each value : */
			energy_1frame += input[j]*input[j];
		}

		energy_1frame = energy_1frame / frame_size; /* Normalisation */
		global_energy[i] = energy_1frame;
		fprintf(energyof_signal ,"%d %Lf\n", i, global_energy[i]); /* writing in the file following the scheme : index energy */
		printf("%d %Lf\n", i, global_energy[i]); /* writing in the file following the scheme : index energy */

	}

	printf("Nbe de frame : %d, Energy computed ! \n--------------\n", numberof_frame);
	fclose(energyof_signal);
	free(global_energy);
	return numberof_frame;
}

long double * separate_noise_speech(FILE * energy, int energysize)
{
	/* Split samples between noise and speech and return the index of speech */
	/* input file is read-write mode */
	long double* input_energy;
	long double* speech;
	long double* words_index;
	long double energy_avg = 0;
	int n = -1, i = 0, j = 0, k = 0, words_index_size = -1;
	long double threshold = 10000000; /* arbitrary value */
	FILE * words;

	words = fopen("data/words.dat","w+");

	/*printf("size of energy input file : %d \n", energysize);*/

	input_energy = (long double*)malloc(energysize*sizeof(long double));

	/* Let's go to the beginning of the file */
	rewind(energy);

	/* analysing the whole file to get relevant information */
	while(!feof(energy))
	{
		fscanf(energy, "%d %Lf \n",&n, &input_energy[i]);
		energy_avg += input_energy[i];
		i++;
	}

	if ( n != 0)
	{
		/* Average energy on the whole signal */
		threshold = 0.5 * energy_avg / n;
		energy_avg = 0;
	}


	speech = (long double *)calloc(n, sizeof(long double));
	words_index = (long double *)calloc(n, sizeof(long double));

	/*printf("end of file n = %d, threshold = %Lf \n", n, threshold);*/



	/* Now, let's use datas : */
	/* if energy of a frame is higher than the threshold, we could consider it's speech. */

	for (j = 0; j < n; j++)
	{
		if(input_energy[j] > threshold)
		{
			speech[j] = input_energy[j];

			/* if the previous "speech" == 0, it means that's noise, so we can record this index (new word/speech starting)
			 * We check the 7 previous samples, equivalent to 7*256/8000 = 224 ms. The average length of */
			if( speech[j-1] == 0 && speech[j-2] == 0 && speech[j-3] == 0 && speech[j-4] == 0 && speech[j-5] == 0 && speech[j-6] == 0 && speech[j-7] == 0 )
			{
				words_index[k] = j;
				k++;
			}

		}
		else
		{
			speech[j] = 0;
			/* if the previous "speech" != 0, it means that's speech, so we can record this index (new noise starting) */
			/*if( speech[j-1] != 0 && speech[j-2] != 0 && speech[j-3] != 0 )
			{
				words_index[k] = j;
				k++;
			}*/
		}
	//	printf("index : %d, niveau de parole : %Lf \n", j, speech[j]);
		fprintf(words,"%d %Lf \n", j, speech[j]);
	}
	words_index_size = k;
	words_index = (long double *)realloc(words_index, words_index_size*sizeof(long double));


	for( k = 0; k < words_index_size; k++)
	{
		printf("%Lf \n", words_index[k]);
	}

	return words_index;
	/* Let's close files and free (il a tout compris) memory */
	fclose(words);
	free(words_index);
	free(input_energy);
	free(speech);
	printf("--------------\n");
}

long double * locate_locutor(FILE * multichannel_input)
{
	/* get multi-channel sound .dat file and compute angle and distance from locutor */
	/* We're working on Nao, so let's suppose that data are recorded in the following scheme :
	 * timestep time(s) left right front rear */
	long double right = -1;
	long double left = -1;
	long double front = -1;
	long double rear = -1;
	long double results[2];
	long double Xaxis = -1;
	long double Yaxis = -1;
	long double norm = -1;
	long double angle = -1;
	int timestep;
	int time;


	while(!feof(multichannel_input))
	{
		/* First, reading the file : */
		fscanf(multichannel_input, "%d %d %Lf %Lf %Lf %Lf \n", &timestep, &time, &right, &left, &front, &rear);
		//printf("timestep : %d, time : %d, right : %Lf, left : %Lf, front : %Lf, rear : %Lf \n", timestep, time, right, left, front, rear);

		/* Get the max on both axis helps know in which quadrant the locutor is */
		Xaxis = maxof_norm(right, left);
		Yaxis = maxof_norm(front, rear);



		/* now let's compute angle and norm to get relevant output parameters */
		norm = sqrt(Xaxis*Xaxis + Yaxis*Yaxis);
		angle = 180/PI*atan(Yaxis/Xaxis)+is_neg(Xaxis)*180; /* angle from X axis (Right) trigonometrical, in degrees */
		results[0] = norm;
		results[1] = angle;
		printf("IN : Norm : %Lf, Angle : %Lf \n\n", norm, angle);
	}


return results;
}


long double maxof_norm(long double value1, long double value2)
{
	if (fabs(value1) > fabs(value2))
	{
	return value1;
	}
	else
	{
		return value2;
	}
}

int is_neg(long double value)
{
	if (value < 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int sign(double value)
{
	/* Gives the sign of the value*/
	if (is_neg(value))
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

int dump_binary(double * input, int size)
{
double * arr;
FILE * binary_output;
int i = -1;

	/* this function converts array to 4-channels raw readable with aplay */

	arr = (double * )malloc(size*sizeof(double));
	binary_output = fopen("data/output_sound.raw","wb");
	for(i = 0; i < size; i++)
	{
		arr[i] = input[i];

		fwrite(&arr[i], sizeof(double), 1, binary_output);
		fwrite(&arr[i], sizeof(double), 1, binary_output);
		fwrite(&arr[i], sizeof(double), 1, binary_output);
		fwrite(&arr[i], sizeof(double), 1, binary_output);
	}

	fclose(binary_output);
	free(arr);

	return 1;
}


