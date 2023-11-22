#if !PICO_ON_DEVICE
/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2012 Mike Chambers
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

#ifndef FAKE86_RENDER_H_INCLUDED
#define FAKE86_RENDER_H_INCLUDED

#include <SDL2/SDL.h>

extern SDL_PixelFormat	*sdl_pixfmt;
extern SDL_Window	*sdl_win;

#ifdef _WIN32
//extern CRITICAL_SECTION screenmutex;
#endif

extern uint8_t	renderbenchmark;
extern uint8_t	scrmodechange;
extern uint32_t	framedelay;
extern uint64_t	totalframes;
extern uint8_t	noscale, nosmooth;

extern int      sdl_error       ( const char *msg   );
extern void     sdl_shutdown    ( void );
extern int	initscreen	( const char *ver   );
extern void	setwindowtitle	( const char *extra );
extern void	doscrmodechange	( void );

#endif
#endif