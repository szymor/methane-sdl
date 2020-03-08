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
// The GP2X main document header file
//------------------------------------------------------------------------------

#ifndef _doc_h
#define _doc_h 1

#include "game.h"
#include "target.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#if !defined(AMIGA)
#include <linux/soundcard.h>
#endif



#define GP2X_SCREEN_W 320
#define GP2X_SCREEN_H 240

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (14)
#define GP2X_BUTTON_B               (15)
#define GP2X_BUTTON_X               (12)
#define GP2X_BUTTON_Y               (13)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

#ifdef METHANE_MIKMOD
class CMikModDrv;
#endif

class CMethDoc
{

public:
	CMethDoc();
	~CMethDoc();

	void InitGame(void);
	void StartGame(void);
	void MainLoop(void *screen_ptr);
	void RedrawMainView( int pal_change_flag );
	void PlayModule(int id);
	void StopModule(void);
	void PlaySample(int id, int pos, int rate);
	void RemoveSoundDriver(void);
	void InitSoundDriver(void);
	void UpdateModule(int id);
	void SaveScores(void);
	void LoadScores(void);

private:
	void DrawScreen( void *screen_ptr );
private:
#ifdef METHANE_MIKMOD
	CMikModDrv	*m_pMikModDrv;
#endif

public:
	CGameTarget	m_GameTarget;

};

#endif

void gp2x_set_volume(int leftVolume, int rightVolume)
{
#if !defined(AMIGA)
    unsigned long soundDev = open("/dev/mixer", O_RDWR);
    if (soundDev)
    {
        int vol =(((leftVolume*0x50)/100)<<8)|((rightVolume*0x50)/100);
        ioctl(soundDev, SOUND_MIXER_WRITE_PCM, &vol);
        close(soundDev);
    }
#endif
}

#define SYS_CLK_FREQ 7372800

void hw_set_cpu(unsigned int MHZ)
{
#ifdef GP2X
  unsigned int v;
  unsigned int mdiv,pdiv=3,scale=0;
  MHZ*=1000000;
  mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;

  mdiv=((mdiv-8)<<8) & 0xff00;
  pdiv=((pdiv-2)<<2) & 0xfc;
  scale&=3;
  v = mdiv | pdiv | scale;
  
  unsigned int l = memregs32[0x808>>2];// Get interupt flags
  memregs32[0x808>>2] = 0xFF8FFFE7;   //Turn off interrupts
  memregs16[0x910>>1]=v;              //Set frequentie
  while(memregs16[0x0902>>1] & 1);    //Wait for the frequentie to be ajused
  memregs32[0x808>>2] = l;            //Turn on interrupts
#endif
}
