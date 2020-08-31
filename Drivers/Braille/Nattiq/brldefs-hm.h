/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2020 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU Lesser General Public License, as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any
 * later version. Please see the file LICENSE-LGPL for details.
 *
 * Web Page: http://brltty.app/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

#ifndef BRLTTY_INCLUDED_NT_BRLDEFS
#define BRLTTY_INCLUDED_NT_BRLDEFS

typedef enum {
  /* braille keyboard keys */
  NT_KEY_Dot1  = 0,
  NT_KEY_Dot2  = 1,
  NT_KEY_Dot3  = 2,
  NT_KEY_Dot4  = 3,
  NT_KEY_Dot5  = 4,
  NT_KEY_Dot6  = 5,
  NT_KEY_Dot7  = 6,
  NT_KEY_Dot8  = 7,
  NT_KEY_Space = 8,

  /* Braille function keys */
  NT_KEY_F1 =  9,
  NT_KEY_F2 = 10,
  NT_KEY_F3 = 11,
  NT_KEY_F4 = 12,

  /* Braille panning keys */
  NT_KEY_Backward = 13,
  NT_KEY_Forward  = 14,


} NT_NavigationKey;

typedef enum {
  NT_GRP_NavigationKeys = 0,
  NT_GRP_RoutingKeys
} HM_KeyGroup;

#endif /* BRLTTY_INCLUDED_HM_BRLDEFS */ 
