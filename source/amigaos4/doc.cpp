/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 * Program WebSite: http://methane.sourceforge.net/index.html              *
 * Project Email: rombust@postmaster.co.uk                                 *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The GP2X main document file
//------------------------------------------------------------------------------
#ifdef GP2X_WIN32_DEBUG
#include <windows.h>
#endif

#include <stdlib.h>

#include "SDL.h"
#include "SDL_framerate.h"
#include "doc.h"

#ifdef METHANE_MIKMOD
#include "../mikmod/audiodrv.h"
#endif
int VOL;


//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------
extern void main_code(void);

//------------------------------------------------------------------------------
// The Game Instance
//------------------------------------------------------------------------------
CMethDoc Game;
FPSmanager FpsManager;
SDL_Surface * SdlScreen = 0;


static char TheScreen[SCR_WIDTH * SCR_HEIGHT];
static int SampleChannel = 5;	// Used by CMethDoc::PlaySample

//------------------------------------------------------------------------------
// The HighScore table filename
//------------------------------------------------------------------------------
static char HighScoreFileName[] = "Methane.HiScores";
#define HighScoreLoadBufferSize (MAX_HISCORES * 64)


//------------------------------------------------------------------------------
// The Main Function
//------------------------------------------------------------------------------

#ifdef GP2X_WIN32_DEBUG
int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
#else
int main (void)
#endif
{
	// init
	if (SDL_Init(SDL_INIT_JOYSTICK|SDL_INIT_VIDEO) < 0) {
		fprintf (stderr, "Can't init SDL : %s", SDL_GetError());
		return 1;
	}
	atexit (SDL_Quit);
	SDL_JoystickOpen (0);

    SDL_EnableKeyRepeat (
		SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// run
	main_code();					// The main routine


	return (0) ;
}

//------------------------------------------------------------------------------
//! \brief The program main code
//------------------------------------------------------------------------------
void main_code(void)
{
VOL=64;
gp2x_set_volume(VOL, VOL);
hw_set_cpu (75);
	JOYSTICK *jptr1;
//	JOYSTICK *jptr2;

	SdlScreen = SDL_SetVideoMode (
		GP2X_SCREEN_W, GP2X_SCREEN_H, 8, SDL_SWSURFACE|SDL_FULLSCREEN);
	if (!SdlScreen)
	{
		fprintf (stderr, "Couldn't set video mode : %s\n", SDL_GetError());
		return;
	}
	SDL_ShowCursor (SDL_DISABLE);

	printf("The GNU General Public License V2 applies to this game.\n\n");
	printf("See: http://methane.sourceforge.net\n");
	printf("Instructions:\n\n");
	printf("Press X to start game (You can't enter player names).\n");
	printf("Press A to fire gas from the gun.\n");
	printf("Press X to jump.\n");
	printf("Hold A to suck a trapped baddie into the gun.\n");
	printf("Release A to throw the trapped baddie from the gun.\n");
	printf("Throw baddies at the wall to destroy them.\n\n");
	printf("START = Quit (and save high scores)\n");
	printf("SELECT = Change player graphic during game\n");
	//printf("F1 = Next Level (DISABLED)\n");


	Game.InitSoundDriver();
	Game.InitGame ();
	Game.LoadScores();
	Game.StartGame();
	jptr1 = &Game.m_GameTarget.m_Joy1;
//	jptr2 = &Game.m_GameTarget.m_Joy2;

	SDL_initFramerate (&FpsManager);
	int run = 1;
	while(run)
	{
#define GP2X_WIN32_DEBUG

#ifdef GP2X_WIN32_DEBUG
		SDL_PumpEvents();
		Uint8 * key = SDL_GetKeyState (NULL);

		if (key[SDLK_ESCAPE]) break;
		if (key[SDLK_TAB])
		{
			Game.m_GameTarget.m_Game.TogglePuffBlow();
		}
//		jptr1->key = key;
//		jptr2->key = key;

		if (key[SDLK_RIGHT]) {
			jptr1->right = 1;
		}
		else {
			jptr1->right = 0;
		}

		jptr1->left  = key[SDLK_LEFT] > 0;
		jptr1->up    = key[SDLK_UP] > 0;
		jptr1->down  = key[SDLK_DOWN] > 0;
		jptr1->fire  = (key[SDLK_LCTRL]) || (key[SDLK_RCTRL]);
		jptr1->key = 13;	// Fake key press (required for high score table)
#else
		// event loop
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_JOYBUTTONDOWN:
					switch(event.jbutton.button)
					{
						//case GP2X_BUTTON_UP:    jptr1->up    = 1; break;
						case GP2X_BUTTON_LEFT:  jptr1->left  = 1; break;
						case GP2X_BUTTON_RIGHT: jptr1->right = 1; break;
						case GP2X_BUTTON_DOWN:  jptr1->down  = 1; break;
						case GP2X_BUTTON_UPLEFT:   /*jptr1->up    =*/ jptr1->left  = 1; break;
						case GP2X_BUTTON_UPRIGHT:  /*jptr1->up    =*/ jptr1->right = 1; break;
						case GP2X_BUTTON_DOWNLEFT: jptr1->down  = jptr1->left  = 1; break;
						case GP2X_BUTTON_DOWNRIGHT:jptr1->down  = jptr1->right = 1; break;
						case GP2X_BUTTON_A: jptr1->up  = 1; break;
						case GP2X_BUTTON_B: break;
						case GP2X_BUTTON_X: jptr1->fire  = 1; break;
						case GP2X_BUTTON_Y:  break;
						case GP2X_BUTTON_L: case GP2X_BUTTON_R:  run =0; break;
						case GP2X_BUTTON_SELECT: Game.m_GameTarget.m_Game.TogglePuffBlow(); break;
						case GP2X_BUTTON_VOLUP: VOL+=5;if (VOL>=129) VOL=128;gp2x_set_volume(VOL, VOL);break;
						case GP2X_BUTTON_VOLDOWN: VOL-=5;if (VOL<=0) VOL=0;gp2x_set_volume(VOL, VOL);break;
						default : break;
					}
					break;
				case SDL_JOYBUTTONUP:
					switch(event.jbutton.button)
					{
						//case GP2X_BUTTON_UP:    jptr1->up    = 0; break;
						case GP2X_BUTTON_LEFT:  jptr1->left  = 0; break;
						case GP2X_BUTTON_RIGHT: jptr1->right = 0; break;
						case GP2X_BUTTON_DOWN:  jptr1->down  = 0; break;
						case GP2X_BUTTON_UPLEFT:   /*jptr1->up    =*/ jptr1->left  = 0; break;
						case GP2X_BUTTON_UPRIGHT:  /*jptr1->up    =*/ jptr1->right = 0; break;
						case GP2X_BUTTON_DOWNLEFT: jptr1->down  = jptr1->left  = 0; break;
						case GP2X_BUTTON_DOWNRIGHT:jptr1->down  = jptr1->right = 0; break;
						case GP2X_BUTTON_A: jptr1->up  = 0;break;
						case GP2X_BUTTON_B:  break;
						case GP2X_BUTTON_X:  jptr1->fire  = 0;break;
						case GP2X_BUTTON_Y:  break;
						default : break;
					}
					break;
				default:
					break;
			}
		}

		jptr1->key = 13;	// Fake key press (required to pass getPlayerName screen)
#endif

		// (CHEAT MODE DISABLED) --> jptr1->next_level = 0;
		Game.MainLoop (0);
		SDL_framerateDelay (&FpsManager);
	}
	Game.SaveScores ();

	SDL_FreeSurface (SdlScreen);
}

//------------------------------------------------------------------------------
//! \brief Initialise Document
//------------------------------------------------------------------------------
CMethDoc::CMethDoc()
{
#ifdef METHANE_MIKMOD
	SMB_NEW(m_pMikModDrv,CMikModDrv);
#endif
	m_GameTarget.Init(this);
}

//------------------------------------------------------------------------------
//! \brief Destroy Document
//------------------------------------------------------------------------------
CMethDoc::~CMethDoc()
{
#ifdef METHANE_MIKMOD
	if (m_pMikModDrv)
	{
		delete(m_pMikModDrv);
		m_pMikModDrv = 0;
	}
#endif
}

//------------------------------------------------------------------------------
//! \brief Initialise the game
//------------------------------------------------------------------------------
void CMethDoc::InitGame(void)
{
	m_GameTarget.InitGame (TheScreen);
	m_GameTarget.PrepareSoundDriver ();
}

//------------------------------------------------------------------------------
//! \brief Initialise the sound driver
//------------------------------------------------------------------------------
void CMethDoc::InitSoundDriver(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->InitDriver();
#endif
}

//------------------------------------------------------------------------------
//! \brief Remove the sound driver
//------------------------------------------------------------------------------
void CMethDoc::RemoveSoundDriver(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->RemoveDriver();
#endif
}

//------------------------------------------------------------------------------
//! \brief Start the game
//------------------------------------------------------------------------------
void CMethDoc::StartGame(void)
{
	m_GameTarget.StartGame();
}

//------------------------------------------------------------------------------
//! \brief Redraw the game main view
//!
//! 	\param pal_change_flag : 0 = Palette not changed
//------------------------------------------------------------------------------
void CMethDoc::RedrawMainView( int pal_change_flag )
{
	// Function not used
}

//------------------------------------------------------------------------------
//! \brief Draw the screen
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::DrawScreen( void *screen_ptr )
{
	METHANE_RGB *pptr;
	int cnt;
    SDL_Color colors [PALETTE_SIZE];

    if (SDL_MUSTLOCK (SdlScreen))
	{
		SDL_LockSurface (SdlScreen);
	}

	// Set the game palette
	pptr = m_GameTarget.m_rgbPalette;
	for (cnt=0; cnt < PALETTE_SIZE; cnt++, pptr++)
	{
		colors[cnt].r = pptr->red;
		colors[cnt].g = pptr->green;
		colors[cnt].b = pptr->blue;
	}
	SDL_SetPalette (SdlScreen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, PALETTE_SIZE);

	// Copy the pixels
	char * dptr = (char *) SdlScreen->pixels;
	char * sptr = TheScreen + 4 * GP2X_SCREEN_W;
	for (int y = 0; y < GP2X_SCREEN_H; y++) {
		for (int x = 0; x < GP2X_SCREEN_W; x++) {
			*dptr++ = *sptr++;
		}
		dptr += (SdlScreen->pitch - GP2X_SCREEN_W);
	}

    if (SDL_MUSTLOCK (SdlScreen))
	{
		SDL_UnlockSurface (SdlScreen);
	}

	// Show the new screen
	SDL_Flip (SdlScreen);
}

//------------------------------------------------------------------------------
//! \brief The Game Main Loop
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::MainLoop( void *screen_ptr )
{
	m_GameTarget.MainLoop();
	DrawScreen( screen_ptr );
#ifdef METHANE_MIKMOD
	m_pMikModDrv->Update();
#endif
}

//------------------------------------------------------------------------------
//! \brief Play a sample (called from the game)
//!
//! 	\param id = SND_xxx id
//!	\param pos = Sample Position to use 0 to 255
//!	\param rate = The rate
//------------------------------------------------------------------------------
void CMethDoc::PlaySample(int id, int pos, int rate)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->PlaySample(id, pos, rate);
#endif
}

//------------------------------------------------------------------------------
//! \brief Stop the module (called from the game)
//------------------------------------------------------------------------------
void CMethDoc::StopModule(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->StopModule();
#endif
}

//------------------------------------------------------------------------------
//! \brief Play a module (called from the game)
//!
//! 	\param id = SMOD_xxx id
//------------------------------------------------------------------------------
void CMethDoc::PlayModule(int id)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->PlayModule(id);
#endif
}

//------------------------------------------------------------------------------
//! \brief Update the current module (ie restart the module if it has stopped) (called from the game)
//!
//! 	\param id = SMOD_xxx id (The module that should be playing)
//------------------------------------------------------------------------------
void CMethDoc::UpdateModule(int id)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->UpdateModule(id);
#endif
}

//------------------------------------------------------------------------------
//! \brief Load the high scores
//------------------------------------------------------------------------------
void CMethDoc::LoadScores(void)
{
	FILE *fptr;
	char *mptr;
	char *tptr;
	char let;
	int cnt;

	fptr = fopen(HighScoreFileName, "r");
	if (!fptr) return;	// No scores available

	// Allocate file memory, which is cleared to zero
	mptr = (char *) calloc(1, HighScoreLoadBufferSize);
	if (!mptr)		// No memory
	{
		fclose(fptr);
		return;
	}
	fread( mptr, 1, HighScoreLoadBufferSize-2, fptr);	// Get the file

	// (Note: mptr is zero terminated)
	tptr = mptr;
	for (cnt=0; cnt<MAX_HISCORES; cnt++)	// For each highscore
	{

		if (!tptr[0]) break;

		m_GameTarget.m_Game.InsertHiScore( atoi(&tptr[4]), tptr );

		do	// Find next name
		{
			let = *(tptr++);
		}while (!( (let=='$') || (!let) ));
		if (!let) break;	// Unexpected EOF
	}

	free(mptr);

	fclose(fptr);

}

//------------------------------------------------------------------------------
//! \brief Save the high scores
//------------------------------------------------------------------------------
void CMethDoc::SaveScores(void)
{
	FILE *fptr;
	int cnt;
	HISCORES *hs;

	fptr = fopen(HighScoreFileName, "w");
	if (!fptr) return;	// Cannot write scores
	for (cnt=0, hs=m_GameTarget.m_Game.m_HiScores; cnt<MAX_HISCORES; cnt++, hs++)
	{
		fprintf(fptr, "%c%c%c%c%d$", hs->name[0], hs->name[1], hs->name[2], hs->name[3], hs->score);
	}
	fclose(fptr);

}

