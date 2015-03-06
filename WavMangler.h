/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef WavMangler_h_
#define WavMangler_h_

#include "AudioStream.h"
#include "SD.h"


typedef struct {
  float playRate;
  uint32_t startPosition;  
  uint32_t endPosition;  
} WmSlice;

class WavMangler : public AudioStream
{
public:
	WavMangler(void) : AudioStream(0, NULL) { begin(); }
	void begin(void);
  	bool open(const char *filename);
        uint32_t get_sample_count();
        void slice(uint32_t from_sample, uint32_t to_sample);
	void close(void);
	bool isPlaying(void) { return playing; }
	uint32_t positionMillis(void);
	uint32_t lengthMillis(void);
	virtual void update(void);
private:
	File rawfile;
	uint32_t file_size;
	volatile uint32_t file_offset;
	volatile bool playing;      
	volatile uint32_t max_file_offset;

        int16_t m_input_data[AUDIO_BLOCK_SAMPLES];
        volatile int m_input_data_len;
        volatile float m_input_pos;        
        volatile float m_input_increment;
};

#endif

