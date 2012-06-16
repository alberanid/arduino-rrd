/*
 * RRD library - Arduino library to read values from analog or digital PIN and
 * store the data in a round-robin array - header file.
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

#ifndef RRD_h
#define RRD_h

#include <Arduino.h>

// If not otherwise specified, the RRD pool will have so many items.
#define RRD_ITEMS 10

class RRD {
  public:
    RRD(String name_, int pin_, int interval_, int length_=RRD_ITEMS, int defaultValue_=0, boolean analog_=true);
    ~RRD();
    void start(boolean alsoReset=false);
    void stop();
    void reset();
    void print();
    int read();
    boolean check();
    int getLastIndex();
    int getLast();
    boolean isFull();
    int getLength();
    float average(boolean excludeLast=false);
    String name;
    int pin;
    int defaultValue;
    int interval;
    int analog;
    boolean running;
    int *items;

  private:
    unsigned long _time;
    int _index;
    int _length;
    boolean _fullData;
};

#endif
