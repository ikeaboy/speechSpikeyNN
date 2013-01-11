/*
 * transforFourier.cpp
 *
 *  Created on: 10 Jan 2013
 *      Author: antoine
 */

#include "transformFourier.h"

fftw_complex *TransformFourier::getComplex() const
{
    return complex;
}

double **TransformFourier::getInput() const
{
    return input;
}

int TransformFourier::getInputSize() const
{
    return input_size;
}

int TransformFourier::getPosition() const
{
    return position;
}

int TransformFourier::getSampleRate() const
{
    return sample_rate;
}

int TransformFourier::getSize() const
{
    return size;
}

void TransformFourier::setComplex(fftw_complex *complex)
{
    this->complex = complex;
}

void TransformFourier::setInput(double **input)
{
    this->input = input;
}

void TransformFourier::setInputSize(int inputSize)
{
    input_size = inputSize;
}


void TransformFourier::setPosition(int position)
{
    this->position = position;
}

void TransformFourier::setSampleRate(int sampleRate)
{
    sample_rate = sampleRate;
}

void TransformFourier::setSize(int size)
{
    this->size = size;
}

void TransformFourier::perform(){





}
