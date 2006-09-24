/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#ifndef _BOX_H
#define _BOX_H

#include "actionevent.h"

typedef struct {

	    SDL_Surface		*background;
	    SDL_Rect		rectBack;
	    SDL_Rect		rectArea;
	    INTERFACEACTION	*action;

		} S_BOX;

S_BOX *		InitBox(Uint16, INTERFACEACTION **, Uint32);
void		FreeBox(S_BOX *);
Uint16		GetHeightText(const char *, ENUMFONT);

#endif
