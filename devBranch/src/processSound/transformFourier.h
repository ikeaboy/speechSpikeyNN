/*
 * transformFourier.h
 *
 *  Created on: 10 Jan 2013
 *      Author: antoine
 */

#ifndef TRANSFORMFOURIER_H_
#define TRANSFORMFOURIER_H_
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>


class TransformFourier{

public:
	fftw_plan transfPlan;
	fftw_complex * complex;

	double ** input;
	int size;
	int input_size;
	int sample_rate;
	int position;/**integer to point to the frame we are at in the processing */
    fftw_complex *getComplex() const;
    double **getInput() const;
    int getInputSize() const;
    int getPosition() const;
    int getSampleRate() const;
    int getSize() const;
    void setComplex(fftw_complex *complex);
    void setInput(double **input);
    void setInputSize(int inputSize);
    void setPosition(int position);
    void setSampleRate(int sampleRate);
    void setSize(int size);
	void perform();


};


#endif /* TRANSFORMFOURIER_H_ */
