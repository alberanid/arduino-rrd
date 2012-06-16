/*
 * RRD library - Arduino library to read values from analog or digital PIN and
 * store the data in a round-robin array.
 *
 * Basic usage:
 * #include <RRD.h>
 *
 * // An RRD pool, named "rrd", that will store 20 analog values
 * // reading from pin 5 every 100 ms.
 * RRD myRRD("rrd", 5, 100, 20);
 *
 * // Start collecting data (probably, you'll put this call in setup()).
 * myRRD.start();
 *
 * // Functions below will be placed somewhere in loop(), probably.
 *
 * // Check if it's time to read and store a value (done automacatically;
 * // return true if we've collected some data).
 * // It's save (even advisable) to call thi method as often as possibile:
 * // if it's not time to store the value, nothing will be done.
 * boolean changed = myRRD.check();
 *
 * // Function that can be used to know if we've already collected
 * // enough data to fill the pool (useful to start reacting only after
 * // we've read enough data, for example).
 * boolean isFull = myRRD.isFulll();
 *
 * // Return the last read value.
 * int value = myRRD.getLast();
 *
 * // Stop collecting data.
 * myRRD.stop();
 *
 * TODO / ideas:
 * - possibility to use micros instead of millis.
 * - destructor doesn't work.
 *
 *
 * Copyright (c) 2012 Davide Alberani <da@erlug.linux.it>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>
#include <stdlib.h>
#include "RRD.h"


#define DEBUG_RRD false

/* RRD constructor.
 * name_: name used to identify this RRD pool.
 * pin_: Pin used to read data.
 * interval_: collect a sample each given milliseconds (if possible at all).
 * length_: (optional) how many items will be contained in the RRD pool (default: RRD_ITEMS).
 * defaultValue_: (optional) value used to initialize the RRD pool (default: 0).
 * analog_: (optional) if true the input pin will be considered as an analog input, otherwise
 *          it will be initialized as a digital input (default: true).
 */
RRD::RRD(String name_, int pin_, int interval_, int length_, int defaultValue_, boolean analog_) {
  name = name_;
  pin = pin_;
  interval = interval_;
  _length = length_;
  defaultValue = defaultValue_;
  analog = analog_;
  items = (int*) malloc(sizeof(int) * _length);
  if (!analog) {
    pinMode(pin, INPUT);
  }
  reset();
}


/* RRD destructor. */
RRD::~RRD() {
  // FIXME: woooah!  It has the bad habit to explode.  Why-oh-why?
  /* if (items) {
    free((void *)items);
  } */
}


/* Reset internal state and initialize the pool. */
void RRD::reset() {
  running = false;
  _fullData = false;
  _index = 0;
  _time = millis();
  for (int i = 0; i < _length; i++) {
    items[i] = defaultValue;
  }
}


/* Begin collecting data.
 * If alsoReset is true, the RRD pool is re-initialized (default: false).
 */
void RRD::start(boolean alsoReset) {
  if (alsoReset) {
    reset();
  }
  _time = millis();
  running = true;
}


/* Stop collecting data. */
void RRD::stop() {
  running = false;
}


/* Pretty-print the RRD pool content. */
void RRD::print() {
  Serial.print(name + String("RRD: "));
  for (int i = 0; i < _length; i++) {
    Serial.print(items[i]);
    if (i < _length-1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}


/* Read a value from the pin.  Notice that the
 * value is NOT stored in the pool, yet.
 */
int RRD::read() {
  if (analog) {
    return analogRead(pin);
  } else {
    return digitalRead(pin);
  }
}


/* Check if it's time to read and store some data.
 * Return true if we've collected a datum.
 */
boolean RRD::check() {
  if (!running) {
    return false;
  }
  unsigned long ctime = millis();
  if ((ctime - _time) < interval) {
    return false;
  }
  items[_index] = read();
  _fullData = _fullData || ((_index + 1) == _length);
  _index = (_index + 1) % _length;
  _time = ctime;
  if (DEBUG_RRD) {
    print();
  }
  return true;
}


/* Return the index of last stored value. */
int RRD::getLastIndex() {
  return (_index + _length -1) % _length;
}


/* Return the last stored value. */
int RRD::getLast() {
  return items[getLastIndex()];
}


/* Return the average of the values in the pool.
 * If excludeLast is true, the last inserted value
 * is not considered (default: false).
 */
float RRD::average(boolean excludeLast) {
  long sum = 0;
  int _lastIdx = getLastIndex();
  for (int i = 0; i < _length; i++) {
    if (excludeLast && i == _lastIdx) {
      continue;
    }
    if (!_fullData && i == _lastIdx) {
      break;
    }
    sum += items[i];
  }
  int div = _length - ((int) excludeLast);
  return (float(sum) / div);
}


/* Return the length of the RRD pool. */
int RRD::getLength() {
  return _length;
}


/* Return true if the RRD pool was filled
 * with values least one time. */
boolean RRD::isFull() {
  return _fullData;
}


