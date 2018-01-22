/*
 * nullserial.h
 *
 *  Created on: 17 Jan 2018
 *      Author: peterharrison
 *
 *    Code adapted from Joost Yervante Damad
 *    http://joost.damad.be/2013/05/nullserial-devnull-serial-stream-for.html
 */

#ifndef NULLSERIAL_H_
#define NULLSERIAL_H_

#include "Arduino.h"

class NullSerial : public Stream {
private:

public:
  // public methods
  NullSerial() {}
  ~NullSerial() {}
  void begin(long speed) { (void)speed;}
  bool listen() {
    return true;
  }
  void end() {}
  bool isListening() {
    return true;
  }
  bool overflow() {
    return false;
  }
  int peek() {
    return -1;
  }

  virtual size_t write(unsigned char byte) {
  	(void)byte;
    return 0;
  }
  virtual int read() {
    return -1;
  }
  virtual int available() {
    return 0;
  }
  virtual void flush() {}

  size_t write(const char *str) {
  	(void)str;
    return 0;
  }
  size_t write(const uint8_t *buffer, size_t size) {
  	(void)buffer;
  	(void)size;
    return 0;
  }

  size_t print(const __FlashStringHelper *) {
    return 0;
  }
  size_t print(const String &) {
    return 0;
  }
  size_t print(const char[]) {
    return 0;
  }
  size_t print(char) {
    return 0;
  }
  size_t print(unsigned char, int = DEC) {
    return 0;
  }
  size_t print(int, int = DEC) {
    return 0;
  }
  size_t print(unsigned int, int = DEC) {
    return 0;
  }
  size_t print(long, int = DEC) {
    return 0;
  }
  size_t print(unsigned long, int = DEC) {
    return 0;
  }
  size_t print(double, int = 2) {
    return 0;
  }
  size_t print(const Printable&) {
    return 0;
  }

  size_t println(const __FlashStringHelper *f) {
  	(void)f;
    return 0;
  }
  size_t println(const String &s) {
  	(void) s;
    return 0;
  }
  size_t println(const char[]) {
    return 0;
  }
  size_t println(char) {
    return 0;
  }
  size_t println(unsigned char, int = DEC) {
    return 0;
  }
  size_t println(int, int = DEC) {
    return 0;
  }
  size_t println(unsigned int, int = DEC) {
    return 0;
  }
  size_t println(long, int = DEC) {
    return 0;
  }
  size_t println(unsigned long, int = DEC) {
    return 0;
  }
  size_t println(double, int = 2) {
    return 0;
  }
  size_t println(const Printable&) {
    return 0;
  }
  size_t println(void) {
    return 0;
  }

  // public only for easy access by interrupt handlers
  static inline void handle_interrupt();
};
#endif /* NULLSERIAL_H_ */
