/*
 * speaker_pcm
 *
 * Plays 8-bit PCM audio on pin 11 using pulse-width modulation (PWM).
 * For Arduino with Atmega168.
 *
 * Uses two timers. The first changes the sample value 8000 times a second.
 * The second holds pin 11 high for 0-255 ticks out of a 256-tick cycle,
 * depending on sample value. The second timer repeats 62500 times per second
 * (16000000 / 256), much faster than the playback rate (8000 Hz), so
 * it almost sounds halfway decent, just really quiet on a PC speaker.
 *
 * Takes over Timer 1 (16-bit) for the 8000 Hz timer. This breaks PWM
 * (analogWrite()) for Arduino pins 9 and 10. Takes Timer 0 (8-bit)
 * for the pulse width modulation, breaking PWM for pin 5.
 *
 * References:
 *     http://www.uchobby.com/index.php/2007/11/11/arduino-sound-part-1/
 *     http://www.atmel.com/dyn/resources/prod_documents/doc2542.pdf
 *     http://www.evilmadscientist.com/article.php/avrdac
 *     http://gonium.net/md/2006/12/27/i-will-think-before-i-code/
 *     http://fly.cc.fer.hr/GDM/articles/sndmus/speaker2.html
 *     http://www.gamedev.net/reference/articles/article442.asp
 *
 * Michael Smith <michael@hurts.ca>
 */

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define SAMPLE_RATE 16000

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "PCM.h"

// This would require code changes to use anything other than pin 6 (OC0A)
#define PIN_SPEAKER 6

/*
 * The audio data needs to be unsigned, 8-bit, 8000 Hz, and small enough
 * to fit in flash. 10000-13000 samples is about the limit.
 *
 * sounddata.h should look like this:
 *     const int sounddata_length=10000;
 *     const unsigned char sounddata_data[] PROGMEM = { ..... };
 *
 * You can use wav2c from GBA CSS:
 *     http://thieumsweb.free.fr/english/gbacss.html
 * Then add "PROGMEM" in the right place. I hacked it up to dump the samples
 * as unsigned rather than signed, but it shouldn't matter.
 *
 * http://musicthing.blogspot.com/2005/05/tiny-music-makers-pt-4-mac-startup.html
 * mplayer -ao pcm macstartup.mp3
 * sox audiodump.wav -v 1.32 -c 1 -r 8000 -u -1 macstartup-8000.wav
 * sox macstartup-8000.wav macstartup-cut.wav trim 0 10000s
 * wav2c macstartup-cut.wav sounddata.h sounddata
 *
 * (starfox) nb. under sox 12.18 (distributed in CentOS 5), i needed to run
 * the following command to convert my wav file to the appropriate format:
 * sox audiodump.wav -c 1 -r 8000 -u -b macstartup-8000.wav
 */

unsigned char const *sounddata_data=0;
int sounddata_length=0;
volatile int sample;
byte lastSample;
char playing = 0;

// This is called at SAMPLE_RATE to load the next sample.
ISR(TIMER1_COMPA_vect) {
  if (sample >= sounddata_length) {
    if (sample == sounddata_length + lastSample) {
      stopPlayback();
    }
    else {
      // Ramp down to zero to reduce the click at the end of playback.
      OCR0A = sounddata_length + lastSample - sample;
    }
  }
  else {
    OCR0A = pgm_read_byte(&sounddata_data[sample]);
  }

  ++sample;
}

void startPlayback(unsigned char const *data, int length)
{
  sounddata_data = data;
  sounddata_length = length;

  // Set up Timer 0 to do pulse width modulation on the speaker
  // pin.

  // Set fast PWM mode (p.157), no prescaler (p.158)
  // Do non-inverting PWM on pin OC0A (p.155) on the Arduino this is pin 6.
  TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  TCCR0B = _BV(CS00);

  playing = 1;

  // Set up Timer 1 to send a sample every interrupt.

  cli();

  // Set CTC mode (Clear Timer on Compare Match) (p.133), no prescaler (p.134)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1A = 0;
  TCCR1B = _BV(WGM12) | _BV(CS10);

  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 = _BV(OCIE1A);

  // Disable interrupts for Timer0
  TIMSK0 = 0;

  lastSample = pgm_read_byte(&sounddata_data[sounddata_length-1]);
  sample = 0;
  sei();
}

void stopPlayback()
{
  // Disable playback per-sample interrupt.
  TIMSK1 = 0;

  // Disable the per-sample timer completely.
  TCCR1B = 0;

  // Disable the PWM timer.
  TCCR0B = 0;

  digitalWrite(PIN_SPEAKER, LOW);

  playing = 0;
}

char isPlaying()
{
  return playing;
}