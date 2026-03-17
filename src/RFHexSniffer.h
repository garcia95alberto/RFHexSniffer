#ifndef RF_HEX_SNIFFER_H
#define RF_HEX_SNIFFER_H

#include <Arduino.h>

class RFHexSniffer {
public:
  RFHexSniffer();

  bool begin(uint8_t pin, bool invert = false);

  void update();
  bool available();
  void clear();

  uint8_t getByteCount() const;
  uint8_t readBytes(uint8_t* out, uint8_t maxLen);
  void readHex(char* out, size_t outSize);

  void setShortLongThreshold(uint16_t usThreshold);
  void setFrameGap(uint32_t usGap);
  void setBitOrderMSB(bool msbFirst);

private:
  static void isrRouter();
  void handleEdge();
  void decodeFrame(uint16_t capturedCount);

  static RFHexSniffer* _instance;

  uint8_t _pin;
  bool _invert;
  bool _msbFirst;

  volatile uint32_t _lastEdgeUs;
  volatile uint16_t _pulseWidths[512];
  volatile uint16_t _pulseCount;
  volatile bool _frameReady;

  uint16_t _shortLongThresholdUs;
  uint32_t _frameGapUs;

  uint8_t _bytes[64];
  uint8_t _byteCount;
};

#endif
