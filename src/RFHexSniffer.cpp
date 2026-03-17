#include "RFHexSniffer.h"

RFHexSniffer* RFHexSniffer::_instance = nullptr;

RFHexSniffer::RFHexSniffer()
  : _pin(255),
    _invert(false),
    _msbFirst(true),
    _lastEdgeUs(0),
    _pulseCount(0),
    _frameReady(false),
    _shortLongThresholdUs(700),
    _frameGapUs(5000),
    _byteCount(0) {
}

bool RFHexSniffer::begin(uint8_t pin, bool invert) {
  _pin = pin;
  _invert = invert;
  _instance = this;

  pinMode(_pin, INPUT);
  _lastEdgeUs = micros();
  _pulseCount = 0;
  _frameReady = false;
  _byteCount = 0;

  attachInterrupt(digitalPinToInterrupt(_pin), RFHexSniffer::isrRouter, CHANGE);
  return true;
}

void RFHexSniffer::setShortLongThreshold(uint16_t usThreshold) {
  _shortLongThresholdUs = usThreshold;
}

void RFHexSniffer::setFrameGap(uint32_t usGap) {
  _frameGapUs = usGap;
}

void RFHexSniffer::setBitOrderMSB(bool msbFirst) {
  _msbFirst = msbFirst;
}

void RFHexSniffer::isrRouter() {
  if (_instance) {
    _instance->handleEdge();
  }
}

void RFHexSniffer::handleEdge() {
  uint32_t now = micros();
  uint32_t dt = now - _lastEdgeUs;
  _lastEdgeUs = now;

  if (dt > _frameGapUs) {
    if (_pulseCount > 8) {
      _frameReady = true;
    }
    return;
  }

  if (_pulseCount < 512) {
    _pulseWidths[_pulseCount++] = (uint16_t)dt;
  }
}

void RFHexSniffer::update() {
  if (!_frameReady) {
    return;
  }

  uint16_t capturedCount = 0;
  uint16_t localPulses[512];

  noInterrupts();
  capturedCount = _pulseCount;

  for (uint16_t i = 0; i < capturedCount && i < 512; i++) {
    localPulses[i] = _pulseWidths[i];
  }

  _pulseCount = 0;
  _frameReady = false;
  interrupts();

  if (capturedCount == 0) {
    return;
  }

  noInterrupts();
  for (uint16_t i = 0; i < capturedCount && i < 512; i++) {
    _pulseWidths[i] = localPulses[i];
  }
  interrupts();

  decodeFrame(capturedCount);
}

bool RFHexSniffer::available() {
  return _byteCount > 0;
}

void RFHexSniffer::clear() {
  _byteCount = 0;
}

uint8_t RFHexSniffer::getByteCount() const {
  return _byteCount;
}

uint8_t RFHexSniffer::readBytes(uint8_t* out, uint8_t maxLen) {
  uint8_t n = (_byteCount < maxLen) ? _byteCount : maxLen;

  for (uint8_t i = 0; i < n; i++) {
    out[i] = _bytes[i];
  }

  return n;
}

void RFHexSniffer::readHex(char* out, size_t outSize) {
  if (outSize == 0) {
    return;
  }

  const char* hex = "0123456789ABCDEF";
  size_t pos = 0;
  out[0] = '\0';

  for (uint8_t i = 0; i < _byteCount; i++) {
    if (pos + 2 >= outSize) {
      break;
    }

    out[pos++] = hex[(_bytes[i] >> 4) & 0x0F];
    out[pos++] = hex[_bytes[i] & 0x0F];
  }

  if (pos < outSize) {
    out[pos] = '\0';
  } else {
    out[outSize - 1] = '\0';
  }
}

void RFHexSniffer::decodeFrame(uint16_t capturedCount) {
  _byteCount = 0;

  uint8_t currentByte = 0;
  uint8_t bitIndex = 0;

  for (uint16_t i = 0; i < capturedCount; i++) {
    uint8_t bit = (_pulseWidths[i] >= _shortLongThresholdUs) ? 1 : 0;

    if (_msbFirst) {
      currentByte = (currentByte << 1) | bit;
    } else {
      currentByte |= (bit << bitIndex);
    }

    bitIndex++;

    if (bitIndex >= 8) {
      if (_byteCount < sizeof(_bytes)) {
        _bytes[_byteCount++] = currentByte;
      }
      currentByte = 0;
      bitIndex = 0;
    }
  }
}
