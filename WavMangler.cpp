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

#include "WavMangler.h"
#include "spi_interrupt.h"


void WavMangler::begin(void)
{
	playing = false;
	file_offset = 0;
	file_size = 0;
}


bool WavMangler::open(const char *filename)
{
	close();
	AudioStartUsingSPI();
	__disable_irq();
	rawfile = SD.open(filename);
	__enable_irq();
	if (!rawfile) {
		//Serial.println("unable to open file");
		return false;
	}
	file_size = rawfile.size();
	file_offset = 0;
        max_file_offset = file_size/2;
        m_input_pos = 0;
        m_input_data_len = 0;
        m_input_increment = 1;
	//Serial.println("able to open file");
	playing = false;
	return true;
}

uint32_t WavMangler::get_sample_count() {  
  return file_size / 2;
}

void WavMangler::slice(uint32_t from_sample, uint32_t to_sample) {  
  __disable_irq();
  file_offset = from_sample * 2;
  max_file_offset = to_sample * 2;
  rawfile.seek(file_offset);
  __enable_irq();  
  playing = true;
}

void WavMangler::close(void)
{
	__disable_irq();
	if (playing) {
		playing = false;
		__enable_irq();
		rawfile.close();
		AudioStopUsingSPI();
	} else {
		__enable_irq();
	}
}


void WavMangler::update(void)
{
	unsigned int i, n;
	audio_block_t *block;

	// only update if we're playing
	if (!playing) return;

	// allocate the audio blocks to transmit
	block = allocate();
	if (block == NULL) return;

          // loop through all the locations in the output buffer
          for(i=0; i<AUDIO_BLOCK_SAMPLES; ++i) {
          
              // read the next chunk of the file if needed
              if((int)m_input_pos >= m_input_data_len) {
                if(!rawfile.available()) {
                  break;
                }
                m_input_pos -= m_input_data_len;
		n = rawfile.read(m_input_data, AUDIO_BLOCK_SAMPLES*2);
		file_offset += n; // skip the number of bytes
                m_input_data_len = n/2;
              }
              
              block->data[i] = m_input_data[(int)m_input_pos] & m_bit_filter;
              m_input_pos += m_input_increment;
          }

          // zero out remainder of buffer (16 bit values)
	  for (;i<AUDIO_BLOCK_SAMPLES; i++) {
	    block->data[i] = 0; 
	  }
      	transmit(block);
      
        if(file_offset >= max_file_offset) {
          file_offset = 0; // position of the first sample
          rawfile.seek(file_offset);
        }
	release(block);
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

uint32_t WavMangler::positionMillis(void)
{
	return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t WavMangler::lengthMillis(void)
{
	return ((uint64_t)file_size * B2M) >> 32;
}

void WavMangler::set_play_rate(float r) {
  if(r < 0.001) r = 0.001;
  if(r > 3) r = 3;
  m_input_increment = r;
}
void WavMangler::set_bit_filter(int bits) {
  unsigned int f = ;
  while(bits-- > 0) {
    f >>=1;
    f |= 0xc000;
  }
  m_bit_filter = f;
}


