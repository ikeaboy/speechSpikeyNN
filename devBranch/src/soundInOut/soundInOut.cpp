/*
 * soundInOut.cpp
 *
 * Sound handling reading and writing sound files
 *
 *  Created on: 9 Jan 2013
 *      Author: antoine
 */




#include "soundInOut.h"




SoundWrapper::SoundWrapper(){

	cout<<"New wrapper...."<<endl;



}

void SoundWrapper::get_wav_header(){
	wav_header head;
	int r=0;

	cout<<"Opening this file header"<<file<<endl;
	in=fopen(file,"r");
	r=fread(header.chunkId, 1, 4, in);
	r=fread(&header.chunkSize, 4, 1, in);
	r=fread(header.format, 1, 4, in);
	r=fread(header.subChunkId, 1, 4, in);
	r=fread(&header.subChunkSize, 4, 1, in);
	r=fread(&header.audioFormat, 2, 1, in);
	r=fread(&header.numChannels, 2, 1, in);
	r=fread(&header.sampleRate, 4, 1, in);
	r=fread(&header.byteRate, 4, 1, in);
	r=fread(&header.blockAlign, 2, 1, in);
	r=fread(&header.bitsPerSample, 2, 1, in);
	r=fread(header.subChunkId, 1, 4, in);
	r=fread(&header.subChunkSize2, 4, 1, in);

	//readWav();
	fclose(in);

}

char * SoundWrapper::getFile()
{
    return (char *)file;
}

wav_header SoundWrapper::getHeader() const
{
    return header;
}

void SoundWrapper::setFile(char * file)
{
    strcpy(this->file, file);
    get_wav_header();
    readWav();

}

void SoundWrapper::setHeader(wav_header header)
{
    this->header = header;
}

void SoundWrapper::readWav(){


	float file_length; // in seconds




		if (header.audioFormat == 1)
			printf("PCM format\n");
		else
		    printf("Unknown format::%d\n",header.audioFormat);

	    if (header.numChannels == 1)        printf("Mono\n");
	    else if (header.numChannels == 2)        printf("Stereo\n");
	    printf("Sample Rate = %d\n", header.sampleRate);
	    printf("%d bytes per sample\n", header.bitsPerSample / 8);
	// how long the file is in seconds
	    file_length = (float)(header.subChunkSize2) / header.byteRate;
	    cout<<"chk:"<< header.subChunkSize2 <<" byterate:: "<< header.byteRate<<endl;

	    printf("file length::%f is %d mn %d s\n",file_length, (int)(file_length/60), (int)(file_length)%60);

}
