/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskjær				<frhag10@student.sdu.dk>
#						Kent Stark Olsen				<keols09@student.sdu.dk>
#						Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#						Leon Bonde Larsen				<lelar09@student.sdu.dk>
#						Rudi Hansen						<rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File:     BufferedSoundIO.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-10-30
********************************************************************************
# Description
#
# The purpose of this class is to create a link between the hardware and the
# implementation of the software part of the physical layer.
*******************************************************************************/
//	Own include
#include "BufferedSoundIO.h"

/****************************************************************************************************************/
/* Constructors and destructor																					*/
/****************************************************************************************************************/
BufferedSoundIO::BufferedSoundIO(void)
{
	if (Pa_Initialize() == paNoError)
	{
		this -> outputStreamError = paNoError;
		this -> inputStreamError = paNoError;

		setupOutputStream(	paFloat32, 2, 512, 44100);
		setupInputStream(	paFloat32, 2, 512, 44100);
	}

	calculateGoertzelCoefficients();
	initializeHack();
}

BufferedSoundIO::BufferedSoundIO(unsigned long bufferSize, unsigned long sampleRate)
{
	if (Pa_Initialize() == paNoError)
	{
		this -> outputStreamError = paNoError;
		this -> inputStreamError = paNoError;

		setupOutputStream(	paFloat32, 2, bufferSize, sampleRate);
		setupInputStream(	paFloat32, 2, bufferSize, sampleRate);
	}

	calculateGoertzelCoefficients();
	initializeHack();
}

BufferedSoundIO::BufferedSoundIO(	PaSampleFormat sampleFormat, unsigned int numberOfChannels,
									unsigned long bufferSize, unsigned long sampleRate)
{
	if (Pa_Initialize() == paNoError)
	{
		this -> outputStreamError = paNoError;
		this -> inputStreamError = paNoError;

		setupOutputStream(	sampleFormat, numberOfChannels, bufferSize, sampleRate);
		setupInputStream(	sampleFormat, numberOfChannels, bufferSize, sampleRate);
	}

	calculateGoertzelCoefficients();
	initializeHack();
}

BufferedSoundIO::BufferedSoundIO(	PaSampleFormat outputSampleFormat, unsigned int outputNumberOfChannels,
									unsigned long outputBufferSize, unsigned long outputSampleRate,
									PaSampleFormat inputSampleFormat, unsigned int inputNumberOfChannels,
									unsigned long inputBufferSize, unsigned long inputSampleRate)
{
	if (Pa_Initialize() == paNoError)
	{
		this -> outputStreamError = paNoError;
		this -> inputStreamError = paNoError;

		setupOutputStream(	outputSampleFormat, outputNumberOfChannels, outputBufferSize, outputSampleRate);
		setupInputStream(	inputSampleFormat, inputNumberOfChannels, inputBufferSize, inputSampleRate);
	}

	calculateGoertzelCoefficients();
	initializeHack();
}

BufferedSoundIO::~BufferedSoundIO(void) 
{
	//	Terminate PortAudio if possible.
	Pa_Terminate();

	//	Delete allocated memory on heap
	delete outputBuffer;
	delete inputBuffer;
	delete frameBuffer;
}

/****************************************************************************************************************/
/* Temporary hack																								*/
/****************************************************************************************************************/
void BufferedSoundIO::initializeHack (void)
{
	this -> frameBuffer = new boost::circular_buffer<Frame>(100);
}

void BufferedSoundIO::dtmfSequenceToFrameBuffer (const boost::circular_buffer<unsigned int> *buffer)
{
	unsigned int i, j, previousCode;
	unsigned char frameParameters[NUMBER_OF_NIBBLES_PER_FRAME];

#ifdef DEBUG
	std::cout << "\nPhysical layer detected a frame!\n";
#endif

	for (i = 0, j = 0; i < (buffer -> size()); i++)
	{
		//	Wrap around if j get out of range
		j = j % NUMBER_OF_NIBBLES_PER_FRAME;

		switch (buffer -> at(i))
		{
		default:
			//	If i equals 0
			if (!i)
			{
				frameParameters[j++] = (unsigned char)(buffer -> at(i));
			}
			else
			{
				if (previousCode != (buffer -> at(i)))
				{
					frameParameters[j++] = (unsigned char)(buffer -> at(i));
				}
			}
			break;
		case 16:	//	Indicate beginning of frame (Discard it)
		case 17:	//	Indicate end of frame (Discard it)
		case 18:	//	Indicate double tone (Discard it)
		case 19:	//	Reserved (Discard it)
			break;
		}

		previousCode = (buffer -> at(i));
	}
	
	Frame newFrame(	frameParameters[5], 
					frameParameters[4], 
					frameParameters[3], 
					frameParameters[2], 
					frameParameters[1], 
					frameParameters[0]);
	
	this -> frameBuffer -> push_back(newFrame);

	return;
}

void BufferedSoundIO::pullFrames (void *buffer)
{
	if (this -> frameBuffer -> empty()) return;
	
	boost::circular_buffer<Frame> *data = (boost::circular_buffer<Frame>*)buffer;
	
	unsigned int remainingPlaceholders = (data -> capacity()) - (data -> size());

	if (remainingPlaceholders > (this -> frameBuffer -> size())) remainingPlaceholders = (this -> frameBuffer -> size());
	
	for (unsigned int i = 0; i < remainingPlaceholders; i++)
	{
		data -> push_back(this -> frameBuffer -> at(0));
		this -> frameBuffer -> pop_front();
	}

	return;
}

unsigned int BufferedSoundIO::frameBufferSize (void)
{
	return (this -> frameBuffer -> size());
}

/****************************************************************************************************************/
/* Private functions																							*/
/****************************************************************************************************************/
void BufferedSoundIO::calculateGoertzelCoefficients (void)
{
	unsigned int i;
	//  Low tones
	for (i = 0; i < BufferedSoundIO_numberOfLowTones; i++)
		goertzelCoefficientsLowTones[i] =	2.0f * 
											cos(	2.0f * PI * 
													(float)(unsigned int)(0.5f + (float)BufferedSoundIO_lowTones[i] *
													(float)(this -> outputBufferSize) /
													(float)(this -> inputSampleRate)
												) /	
												(float)(this -> outputBufferSize));
	//	High tones
	for (i = 0; i < BufferedSoundIO_numberOfHighTones; i++)
		goertzelCoefficientsHighTones[i] =	2.0f * 
											cos(	2.0f * PI * 
													(float)(unsigned int)(0.5f + (float)BufferedSoundIO_highTones[i] *
													(float)(this -> outputBufferSize) /
													(float)(this -> inputSampleRate)
												) /	
											(float)(this -> outputBufferSize));
}

/****************************************************************************************************************/
/* Private callbacks																							*/
/****************************************************************************************************************/
int BufferedSoundIO::outputStreamCallback(	const void *inputBuffer,
											void *outputBuffer,
											unsigned long framesPerBuffer,
											const PaStreamCallbackTimeInfo* timeInfo,
											PaStreamCallbackFlags statusFlags)
{
	//	Cast data passed through stream to our structure
	float *out = (float*)outputBuffer;
    (void) inputBuffer; //	Prevent unused variable warning.
	
	//	If buffer is empty, generate silence
	if (this -> outputBuffer -> empty())
	{
		this -> isOutputStreamRunning_ = false;

		//	Calulate buffer (Silence)
		for(unsigned int i = 0; i < framesPerBuffer; i++)
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}
	//	else generate tone of next element in the list
	else
	{
		this -> isOutputStreamRunning_ = true;

		//  Get frequencies
		unsigned int code = (this -> outputBuffer -> at(0));												//	Code between 0 and 15 (both included) (possible 19 if extra frequency are added)
		this -> outputBuffer -> pop_front();																//	Remove code from ring buffer
		unsigned int lowFrequency = BufferedSoundIO_lowTones[code / BufferedSoundIO_numberOfHighTones];		//	Calculate index of low frequency
		unsigned int highFrequency = BufferedSoundIO_highTones[code % BufferedSoundIO_numberOfHighTones];	//	Calculate index of high frequency
		
		//	Calulate buffer
		for(unsigned int i = 0; i < framesPerBuffer; i++)
		{
			*out++ =	(1.0f / 2.0f) * 
						((float)sin(2.0f * (float)PI * ((float)lowFrequency / (float)(this -> outputSampleRate)) * (float)i) +
						 (float)sin(2.0f * (float)PI * ((float)highFrequency / (float)(this -> outputSampleRate)) * (float)i));
			*out++ =	(1.0f / 2.0f) * 
						((float)sin(2.0f * (float)PI * ((float)lowFrequency / (float)(this -> outputSampleRate)) * (float)i) +
						 (float)sin(2.0f * (float)PI * ((float)highFrequency / (float)(this -> outputSampleRate)) * (float)i));
		}
	}

	return paContinue;
}

int BufferedSoundIO::inputStreamCallback(	const void *inputBuffer,
											void *outputBuffer,
											unsigned long framesPerBuffer,
											const PaStreamCallbackTimeInfo* timeInfo,
											PaStreamCallbackFlags statusFlags)
{
	//	Cast data passed through stream to our structure
	float *in = (float*)inputBuffer;
	(void) outputBuffer; //	Prevent unused variable warning.

	//	General variables
	unsigned int i, j;

	//	Goertzel variables
	float input, frequencyOutputMagnitude;
	float	outputLowTones[BufferedSoundIO_numberOfLowTones],
			w1LowTones[BufferedSoundIO_numberOfLowTones], w2LowTones[BufferedSoundIO_numberOfLowTones];
	float	outputHighTones[BufferedSoundIO_numberOfHighTones],
			w1HighTones[BufferedSoundIO_numberOfHighTones], w2HighTones[BufferedSoundIO_numberOfHighTones];
	
	//	Make sure all goertzel values are 0.0f
	//  Low tones
	for (i = 0; i < BufferedSoundIO_numberOfLowTones; i++)
		outputLowTones[i] = w1LowTones[i] = w2LowTones[i] = 0.0f;
	//	High tones
	for (i = 0; i < BufferedSoundIO_numberOfHighTones; i++)
		outputHighTones[i] = w1HighTones[i] = w2HighTones[i] = 0.0f;

	//	Goertzel algorithm
	for (i = 0; i < framesPerBuffer; i++)
	{
		input = *in++;
		*in++;
		//	Low tones
		for (j = 0; j < BufferedSoundIO_numberOfLowTones; j++)
		{
			outputLowTones[j] = input + goertzelCoefficientsLowTones[j] * w1LowTones[j] - w2LowTones[j];
			w2LowTones[j] = w1LowTones[j];
			w1LowTones[j] = outputLowTones[j];
		}
		//	High tones
		for (j = 0; j < BufferedSoundIO_numberOfHighTones; j++)
		{
			outputHighTones[j] = input + goertzelCoefficientsHighTones[j] * w1HighTones[j] - w2HighTones[j];
			w2HighTones[j] = w1HighTones[j];
			w1HighTones[j] = outputHighTones[j];
		}
	}

	//	Decisions on how to interpret the incomming signals goes here
	//	Decision making variables
	bool isSignalFaulty(false), isLowToneDetected(false), isHighToneDetected(false), isSignalValid;
	unsigned int code, indexOfLowTone, indexOfHighTone;

	//	Calculate low tones
	for (i = 0; i < BufferedSoundIO_numberOfLowTones; i++)
	{
		frequencyOutputMagnitude = sqrt(pow(w1LowTones[i], 2) + 
										pow(w2LowTones[i], 2) -
										w1LowTones[i] * 
										w2LowTones[i] * 
										goertzelCoefficientsLowTones[i]);	

		if (frequencyOutputMagnitude > HIGH_TONES_THRESHOLD)
		{
			if (!isLowToneDetected)
			{
				isLowToneDetected = true;
				indexOfLowTone = i;
			}
			else isSignalFaulty = true;
		}
	}
	//	Calculate high tones
	for (i = 0; i < BufferedSoundIO_numberOfHighTones; i++)
	{
		frequencyOutputMagnitude = sqrt(pow(w1HighTones[i], 2) + 
									 	pow(w2HighTones[i], 2) -
										w1HighTones[i] * 
										w2HighTones[i] * 
										goertzelCoefficientsHighTones[i]);	

		if (frequencyOutputMagnitude > HIGH_TONES_THRESHOLD)
		{
			if (!isHighToneDetected)
			{
				isHighToneDetected = true;
				indexOfHighTone = i;
			}
			else isSignalFaulty = true;
		}
	}

	//	Calculate if signal is valid for interpretation
	isSignalValid = isLowToneDetected && isHighToneDetected && (!isSignalFaulty);

	//	If signal is valid
	if (isSignalValid)
	{
		//	Generate code from found frequencies 
		code = indexOfLowTone * 4 + indexOfHighTone;
		
		//	Final check to ensure code is ok
		if ((code >= 0) && (code <= 19))
		{
			switch (code)
			{
			case 16:	//	Start of frame
				this -> inputBuffer -> clear();
				this -> inputBuffer -> push_back(code);
				break;
			default:	//	Every other valid signal
				this -> inputBuffer -> push_back(code);
				break;
			case 17:	//	End of frame
				if (this -> inputBuffer -> size())
				{
					this -> inputBuffer -> push_back(code);

					//	TODO: Make a callback to physical layer telling it to pullSamples
					//	This is a temporary hack to make it work.
					this -> dtmfSequenceToFrameBuffer(this -> inputBuffer);
					
					this -> inputBuffer -> clear();
				}
				break;
			}
		}
	}

 	return paContinue;
}

/****************************************************************************************************************/
/* Public methods																								*/
/****************************************************************************************************************/
void BufferedSoundIO::setupOutputStream(PaSampleFormat sampleFormat, unsigned int numberOfChannels,
										unsigned long bufferSize, unsigned long sampleRate)
{
	//	Setting up output variables
	this -> outputSampleFormat		= sampleFormat;
	this -> outputNumberOfChannels	= numberOfChannels;
	this -> outputBufferSize		= bufferSize;
	this -> outputSampleRate		= sampleRate;
	
	//	Set up output buffer (Dynamic)
	this -> outputBuffer = new boost::circular_buffer<unsigned int>();
	
	//	Set up output stream
	this -> outputStreamError = Pa_OpenDefaultStream(	&(this -> outputStream),		//	Passing address of portAudioStream pointer
														0,								//	No input
														this -> outputNumberOfChannels,	//	Stereo output
														this -> outputSampleFormat,		//	32 bit floating point output
														this -> outputSampleRate,		//	Sample rate
														this -> outputBufferSize,		//	Frames per buffer, i.e. the number
																						//	of sample frames that PortAudio will
																						//	request from the callback. Many apps
																						//  may want to use
																						//	paFramesPerBufferUnspecified, which
																						//	tells PortAudio to pick the best,
																						//	possibly changing, buffer size.
														this -> paOutputStreamCallback,	//	This is your callback function.
														this);							//	This is a pointer that will be passed
																						//	to your callback
}

void BufferedSoundIO::startOutputStream(void)
{
	//	If an error have been raised during interaction with output stream,
	//  PortAudio playback will not be started.
	if (this -> outputStreamError == paNoError)
	{
		//	Start stream
		this -> outputStreamError = Pa_StartStream(this -> outputStream);
	}
}

void BufferedSoundIO::stopOutputStream(void)
{
	//	If an error have been raised during interaction with output stream,
	//  PortAudio playback will not be started.
	if (this -> outputStreamError == paNoError)
	{
		//	Stop stream
		this -> outputStreamError = Pa_StopStream(this -> outputStream);
	}
}

bool BufferedSoundIO::isOutputStreamRunning(void)
{
	return this -> isOutputStreamRunning_;
}

int BufferedSoundIO::pushSamples(void *sampleBuffer)
{
	//	Cast sampleBuffer to proper type
	boost::circular_buffer<unsigned int> *data = (boost::circular_buffer<unsigned int>*)sampleBuffer;

	//	Resize buffer if needed
	if ((this -> outputBuffer -> capacity()) != (data -> size())) this -> outputBuffer -> resize(data -> size());

	//	Clear buffer
	this -> outputBuffer-> clear();

	//  Copy data
	for (unsigned int i = 0; i < (data -> size()); i++)	this -> outputBuffer -> push_back(data -> at(i));
	
 	return 0;
}

unsigned int BufferedSoundIO::outputSequenceBufferSize (void)
{
	return (this -> outputBuffer -> size());
}

void BufferedSoundIO::setupInputStream(	PaSampleFormat sampleFormat, unsigned int numberOfChannels, 
										unsigned long bufferSize, unsigned long sampleRate)
{
	//	Setting up input variables
	this -> inputSampleFormat		= sampleFormat;
	this -> inputNumberOfChannels	= numberOfChannels;
	this -> inputBufferSize			= bufferSize;
	this -> inputSampleRate			= sampleRate;

	//	Set up input buffer (Static)
	this -> inputBuffer = new boost::circular_buffer<unsigned int>(	(this -> outputBufferSize) /	//	2 * MAX_NIPPLES_PER_FRAME size 
																	(this -> inputBufferSize) *		//	of input buffer prevents asynchronous
																	MAX_NIBBLES_PER_FRAME + 1);		//  data sets to be misinterpretated	

	//	Set up input stream
	this -> inputStreamError = Pa_OpenDefaultStream(	&(this -> inputStream),			//	Passing address of portAudioStream pointer
														this -> inputNumberOfChannels,	//	Stereo input
														0,								//	No output
														this -> inputSampleFormat,		//	32 bit floating point output
														this -> inputSampleRate,		//	Sample rate
														this -> inputBufferSize,		//	Frames per buffer, i.e. the number
																						//	of sample frames that PortAudio will
																						//	request from the callback. Many apps
																						//  may want to use
																						//	paFramesPerBufferUnspecified, which
																						//	tells PortAudio to pick the best,
																						//	possibly changing, buffer size.
														this -> paInputStreamCallback,	//	This is your callback function.
														this);							//	This is a pointer that will be passed
																						//	to your callback
}

void BufferedSoundIO::startInputStream(void)
{
	//	If an error have been raised during interaction with output stream,
	//  PortAudio playback will not be started.
	if (this -> inputStreamError == paNoError)
	{
		//	Start stream
		this -> inputStreamError = Pa_StartStream(this -> inputStream);
	}
}

void BufferedSoundIO::stopInputStream(void)
{
	//	If an error have been raised during interaction with output stream,
	//  PortAudio playback will not be started.
	if (this -> inputStreamError == paNoError)
	{
		//	Stop stream
		this -> inputStreamError = Pa_StopStream(this -> inputStream);
	}
}