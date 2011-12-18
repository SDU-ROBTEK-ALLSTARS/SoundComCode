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
# File:     BufferedSoundIO.h
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
# implementation of the physical layer.
*******************************************************************************/
#pragma once

#include "../portaudio.h"
#include "../buffers/frame.h"
#include <boost/circular_buffer.hpp>
#include <math.h>
#include <iostream>	//	For debugging purpose

#define PI							(3.14159265f)
#define LOW_TONES_THRESHOLD			(5.0f)
#define HIGH_TONES_THRESHOLD		(5.0f)
#define	MAX_NIBBLES_PER_FRAME		(13)
#define	NUMBER_OF_NIBBLES_PER_FRAME	(6)

//	Frequencies of interest
const unsigned int BufferedSoundIO_lowTones[] = {697, 770, 852, 941, 350};
const unsigned int BufferedSoundIO_highTones[] = {1209, 1336, 1477, 1633};
const unsigned int BufferedSoundIO_numberOfLowTones = sizeof(BufferedSoundIO_lowTones) / sizeof(unsigned int);
const unsigned int BufferedSoundIO_numberOfHighTones = sizeof(BufferedSoundIO_highTones) / sizeof(unsigned int);

class BufferedSoundIO
{
private:
	//	Temporary hack
	boost::circular_buffer<Frame> *frameBuffer;
	void initializeHack (void);
	void dtmfSequenceToFrameBuffer (const boost::circular_buffer<unsigned int> *buffer);
	
	//	Goertzel variables
	float goertzelCoefficientsLowTones[BufferedSoundIO_numberOfLowTones];
	float goertzelCoefficientsHighTones[BufferedSoundIO_numberOfHighTones];

	//	Private functions
	void calculateGoertzelCoefficients (void);
	
	//	Output stream variables
	PaStream *outputStream;
	PaError outputStreamError;
	boost::circular_buffer<unsigned int> *outputBuffer;
	
	PaSampleFormat outputSampleFormat;
	unsigned int outputNumberOfChannels;
	unsigned long outputBufferSize;
	unsigned long outputSampleRate;

	bool isOutputStreamRunning_;
	
	//	Input stream variables
	PaStream *inputStream;
	PaError inputStreamError;
	boost::circular_buffer<unsigned int> *inputBuffer;
		
	PaSampleFormat inputSampleFormat;
	unsigned int inputNumberOfChannels;
	unsigned long inputBufferSize;
	unsigned long inputSampleRate;

	//	Output stream callbacks
	static int paOutputStreamCallback(	const void *inputBuffer,
										void *outputBuffer,
										unsigned long framesPerBuffer,
										const PaStreamCallbackTimeInfo* timeInfo,
										PaStreamCallbackFlags statusFlags,
										void *userData)
										{ return ((BufferedSoundIO*)userData) -> outputStreamCallback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags); }

	int outputStreamCallback(	const void *inputBuffer,
								void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags);

	//	Input stream callbacks
	static int paInputStreamCallback(	const void *inputBuffer,
										void *outputBuffer,
										unsigned long framesPerBuffer,
										const PaStreamCallbackTimeInfo* timeInfo,
										PaStreamCallbackFlags statusFlags,
										void *userData)
										{ return ((BufferedSoundIO*)userData) -> inputStreamCallback(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags); }

	int inputStreamCallback(	const void *inputBuffer,
								void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags);

public:
	BufferedSoundIO(void);

	BufferedSoundIO(unsigned long bufferSize, unsigned long sampleRate);

	BufferedSoundIO(PaSampleFormat sampleFormat, unsigned int numberOfChannels,
					unsigned long bufferSize, unsigned long sampleRate);

	BufferedSoundIO(PaSampleFormat outputSampleFormat,	unsigned int outputNumberOfChannels,
					unsigned long outputBufferSize,		unsigned long outputSampleRate,
					PaSampleFormat inputSampleFormat,	unsigned int inputNumberOfChannels,
					unsigned long inputBufferSize,		unsigned long inputSampleRate);

	~BufferedSoundIO(void);

	//	Output stream definitions
	//	Set up the outputstream
	void setupOutputStream(	PaSampleFormat sampleFormat, 
							unsigned int numberOfChannels,
							unsigned long bufferSize,
							unsigned long sampleRate);		//	Default: pa_float32 (from -1.0 to 1.0), stereo
	
	void startOutputStream(void);							//	This starts the the playback from buffer
	void stopOutputStream(void);							//	This should be called implicitly when outputbuffer is empty
	bool isOutputStreamRunning(void);						//	Returns boolean which tell if the playback is running
	int pushSamples(void *sampleBuffer);					//	Return value indicate success
	unsigned int outputSequenceBufferSize (void);
	
	//	Input stream definitions
	//	Set up the input stream
	void setupInputStream(	PaSampleFormat sampleFormat, 
							unsigned int numberOfChannels,
							unsigned long bufferSize,
							unsigned long sampleRate);

	void startInputStream(void);							//	This start the recording of sound
	void stopInputStream(void);								//	This stop the recording of sound
	
	////////////////////////////////Temporary hack publics//////////////////////////////
	void pullFrames (void *buffer);
	unsigned int frameBufferSize (void);
};
