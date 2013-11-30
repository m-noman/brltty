/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2013 by The BRLTTY Developers.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version. Please see the file LICENSE-GPL for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

#include "prologue.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "log.h"
#include "parameters.h"
#include "async_alarm.h"
#include "program.h"
#include "file.h"
#include "parse.h"
#include "prefs.h"
#include "charset.h"
#include "spk.h"
#include "brltty.h"

void
initializeSpeechSynthesizer (SpeechSynthesizer *spk) {
  spk->data = NULL;
}

static void setSpeechTrackingAlarm (void *data);
static AsyncHandle speechTrackingAlarm = NULL;

int speechTracking = 0;
int speechScreen = -1;
int speechLine = 0;
int speechIndex = -1;

ASYNC_ALARM_CALLBACK(handleSpeechTrackingAlarm) {
  SpeechSynthesizer *spk = parameters->data;

  asyncDiscardHandle(speechTrackingAlarm);
  speechTrackingAlarm = NULL;

  if (speechTracking) {
    if (scr.number == speechScreen) {
      speech->doTrack(spk);

      if (speech->isSpeaking(spk)) {
        if (ses->trackCursor) {
          int index = speech->getTrack(spk);

          if (index != speechIndex) trackSpeech(speechIndex = index);
        }

        setSpeechTrackingAlarm(parameters->data);
        return;
      }
    }

    speechTracking = 0;
  }
}

static void
setSpeechTrackingAlarm (void *data) {
  if (!speechTrackingAlarm) {
    asyncSetAlarmIn(&speechTrackingAlarm, SPEECH_TRACKING_POLL_INTERVAL, handleSpeechTrackingAlarm, data);
  }
}

void
muteSpeech (SpeechSynthesizer *spk, const char *reason) {
  speech->mute(spk);
  logMessage(LOG_CATEGORY(SPEECH_EVENTS), "mute: %s", reason);
}

void
sayUtf8Characters (
  SpeechSynthesizer *spk,
  const char *text, const unsigned char *attributes,
  size_t length, size_t count,
  int immediate
) {
  if (count) {
    if (immediate) {
      muteSpeech(spk, "say immediate");
    }

    logMessage(LOG_CATEGORY(SPEECH_EVENTS), "say: %s", text);
    speech->say(spk, (const unsigned char *)text, length, count, attributes);
    if (speechTracking) setSpeechTrackingAlarm(spk);
  }
}

void
sayString (SpeechSynthesizer *spk, const char *string, int immediate) {
  sayUtf8Characters(spk, string, NULL, strlen(string), getTextLength(string), immediate);
}

static void
sayStringSetting (SpeechSynthesizer *spk, const char *name, const char *string) {
  char statement[0X40];
  snprintf(statement, sizeof(statement), "%s %s", name, string);
  sayString(spk, statement, 1);
}

static void
sayIntegerSetting (SpeechSynthesizer *spk, const char *name, int integer) {
  char string[0X10];
  snprintf(string, sizeof(string), "%d", integer);
  sayStringSetting(spk, name, string);
}

static unsigned int
getIntegerSetting (unsigned char setting, unsigned char internal, unsigned int external) {
  return rescaleInteger(setting, internal, external);
}

int
setSpeechVolume (SpeechSynthesizer *spk, int setting, int say) {
  if (!speech->setVolume) return 0;
  logMessage(LOG_CATEGORY(SPEECH_EVENTS), "set volume: %d", setting);
  speech->setVolume(spk, setting);
  if (say) sayIntegerSetting(spk, gettext("volume"), setting);
  return 1;
}

unsigned int
getIntegerSpeechVolume (unsigned char setting, unsigned int normal) {
  return getIntegerSetting(setting, SPK_VOLUME_DEFAULT, normal);
}

#ifndef NO_FLOAT
float
getFloatSpeechVolume (unsigned char setting) {
  return (float)setting / (float)SPK_VOLUME_DEFAULT;
}
#endif /* NO_FLOAT */

int
setSpeechRate (SpeechSynthesizer *spk, int setting, int say) {
  if (!speech->setRate) return 0;
  logMessage(LOG_CATEGORY(SPEECH_EVENTS), "set rate: %d", setting);
  speech->setRate(spk, setting);
  if (say) sayIntegerSetting(spk, gettext("rate"), setting);
  return 1;
}

unsigned int
getIntegerSpeechRate (unsigned char setting, unsigned int normal) {
  return getIntegerSetting(setting, SPK_RATE_DEFAULT, normal);
}

#ifndef NO_FLOAT
float
getFloatSpeechRate (unsigned char setting) {
  static const float spkRateTable[] = {
    0.3333,
    0.3720,
    0.4152,
    0.4635,
    0.5173,
    0.5774,
    0.6444,
    0.7192,
    0.8027,
    0.8960,
    1.0000,
    1.1161,
    1.2457,
    1.3904,
    1.5518,
    1.7320,
    1.9332,
    2.1577,
    2.4082,
    2.6879,
    3.0000
  };

  return spkRateTable[setting];
}
#endif /* NO_FLOAT */

int
setSpeechPitch (SpeechSynthesizer *spk, int setting, int say) {
  if (!speech->setPitch) return 0;
  logMessage(LOG_CATEGORY(SPEECH_EVENTS), "set pitch: %d", setting);
  speech->setPitch(spk, setting);
  if (say) sayIntegerSetting(spk, gettext("pitch"), setting);
  return 1;
}

unsigned int
getIntegerSpeechPitch (unsigned char setting, unsigned int normal) {
  return getIntegerSetting(setting, SPK_PITCH_DEFAULT, normal);
}

#ifndef NO_FLOAT
float
getFloatSpeechPitch (unsigned char setting) {
  return (float)setting / (float)SPK_PITCH_DEFAULT;
}
#endif /* NO_FLOAT */

int
setSpeechPunctuation (SpeechSynthesizer *spk, SpeechPunctuation setting, int say) {
  if (!speech->setPunctuation) return 0;
  logMessage(LOG_CATEGORY(SPEECH_EVENTS), "set punctuation: %d", setting);
  speech->setPunctuation(spk, setting);
  return 1;
}
