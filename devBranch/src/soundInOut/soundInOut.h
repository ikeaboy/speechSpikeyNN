/*
 * soundInOut.h
 *
 *  Created on: 9 Jan 2013
 *      Author: antoine
 */

#ifndef SOUNDINOUT_H_
#define SOUNDINOUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <linux/soundcard.h>
#include <fcntl.h>
#include <fftw3.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
using namespace std;

typedef struct {
	char chunkId[4];
	int chunkSize;
	char format[4];
	char subChunkId[4];
	int subChunkSize;
	unsigned int audioFormat;
	unsigned int numChannels;
    	int sampleRate;
	int byteRate;
	unsigned int blockAlign;
	unsigned int bitsPerSample;
	char subChunkId2[4];
	int subChunkSize2;
	} wav_header;


class SoundWrapper {

public :

	wav_header  header;
	FILE * in;
	char file[25];
    char * getFile();
    wav_header getHeader() const;
    void setFile(char * file);
    void setHeader(wav_header header);
    SoundWrapper();
	void get_wav_header();

	void readWav();


};


#endif /* SOUNDINOUT_H_ */
