#include "basic.h"
#include "escene.h"

shurt	etr_log= ESP_BABBLE;
static	shurt	etr_wim, etrl_progress, etrl_lock, etr_dot;
static	short	etrl_step;

   /****
      Windowing interface routines

      visT_major(s)  -- declare major phase
      visT_minor(s, n) -- name minor phase (and length n)
      visT_step(n)  -- update amount completed 
      visT_din    -- completed minor section
      visT_done   -- finished major section

      visT_tic -- check OS and window system for interupts

      visW_message -- message for the status window
      visW_warn   -- warns of error or exception
	 return  0: continue,  1: halt
      visW_err    -- warns of fatal error/exception
      */

void visT_major(char *s) {}
void visT_minor(char *s, int n) {}
void visT_step(int n) {}
void visT_tick() {}
void visT_din() {}
void visT_done() {}
void visT_tic() {}
void visW_message(char *s) {}
void visW_outp(char *s) {}
int visW_warn(char *s, char *t) { return 0; } 
void visW_err(char *s, char *t) {}
void visW_bad(char *s, char *t) {}
int visW_prompt_for_new(char *s, char *t) { return -1; } 
int visW_prompt_for_replace(char *s, char *t) { return -1; } 
int visW_pFile(int scope, int clontet, char *s, char *t) { return 1; } 

