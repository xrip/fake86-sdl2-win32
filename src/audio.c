/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2013 Mike Chambers
            (C)2020      Gabor Lenart "LGB"

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* audio.c: functions to mix the audio channels, and handle SDL's audio interface. */

#include "config.h"
#if !PICO_ON_DEVICE
#include <SDL2/SDL.h>
#endif
//#ifdef _WIN32
//#include <windows.h>
//#include <process.h>
//#else
////#include <pthread.h>
//#endif
#include <stdint.h>
#include <stdio.h>
//#include <memory.h>
#include <string.h>

#include "audio.h"

#include "blaster.h"
#include "adlib.h"
#include "sndsource.h"
#include "speaker.h"
#include "timing.h"
#include "parsecl.h"

uint8_t doaudio = 1;

static struct wav_hdr_s wav_hdr;
static FILE *wav_file = NULL;

#if !PICO_ON_DEVICE
static SDL_AudioSpec wanted;
#endif
static int8_t audbuf[96000];
static int32_t audbufptr, usebuffersize;
int32_t usesamplerate = AUDIO_DEFAULT_SAMPLE_RATE;
int32_t latency = AUDIO_DEFAULT_LATENCY;

static uint64_t doublesamplecount;


uint8_t audiobufferfilled(void)
{
	if (audbufptr >= usebuffersize) return 1;
	return 0;
}


void tickaudio(void) {
	int16_t sample;
	if (audbufptr >= usebuffersize)
		return;
	sample = adlibgensample() >> 4;
	if (usessource)
		sample += getssourcebyte();
	sample += getBlasterSample();
	if (speakerenabled)
		sample += (speakergensample() >> 1);
	if (audbufptr < sizeof(audbuf))
		audbuf[audbufptr++] = (uint8_t)((uint16_t)sample+128);
}


// FIXME: it was int8_t I modified to uint8_t ...
static void fill_audio ( void *udata, uint8_t *stream, int len )
{
	memcpy(stream, audbuf, len);
	memmove(audbuf, &audbuf[len], usebuffersize - len);
	audbufptr -= len;
	if (audbufptr < 0)
		audbufptr = 0;
}


void initaudio ( void )
{
	printf ("Initializing audio stream... ");
	if (usesamplerate < 4000)
		usesamplerate = 4000;
	else if (usesamplerate > 96000)
		usesamplerate = 96000;
	if (latency < 10)
		latency = 10;
	else if (latency > 1000)
		latency = 1000;
	audbufptr = usebuffersize = (usesamplerate / 1000) * latency;
	gensamplerate = usesamplerate;
	doublesamplecount = (uint32_t)((double)usesamplerate * (double)0.01);
#if !PICO_ON_DEVICE
	wanted.freq = usesamplerate;
	wanted.format = AUDIO_U8;
	wanted.channels = 1;
	wanted.samples = (uint16_t)usebuffersize >> 1;
	wanted.callback = fill_audio;
	wanted.userdata = NULL;
	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		printf("Error: %s\n", SDL_GetError());
		return;
	} else {
		printf("OK! (%lu Hz, %lu ms, %lu sample latency)\n", (long unsigned int)usesamplerate, (long unsigned int)latency, (long unsigned int)usebuffersize);
	}
	memset(audbuf, 128, sizeof(audbuf));
	audbufptr = usebuffersize;
	SDL_PauseAudio(0);
#endif
	return;
}


void killaudio ( void )
{
#if !PICO_ON_DEVICE
	SDL_PauseAudio(1);
#endif
}
