#include <SPI.h>
#include <TimerThree.h>

// PINOUTS:
// pin 4 (B7): LE - Latch enable
// pin 1 (B1): CLK - Clock
// pin 2 (B2): DI - Data In

unsigned char digits[16] = {
  0x3f, 0x06, 0x5b, 0x4f,
  0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x67, 0x77, 0x7c,
  0x39, 0x5e, 0x79, 0x71 };

const unsigned char LATCH_PIN = 4;

volatile unsigned long millibeats = 0;

unsigned long usec_per_mbeat = 86400;

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN,LOW);
  Serial.begin(15200);
  Timer3.initialize(usec_per_mbeat);
  Timer3.attachInterrupt(next_beat);
}

void next_beat() {
  millibeats++;
  if (millibeats >= 1000L*1000) { millibeats = 0; }
  if ((millibeats % 10) != 0) return;
  unsigned long cb = millibeats / 10;
  for (int i = 0; i < 5; i++) {
    SPI.transfer(digits[cb % 10]);
    cb /= 10;
  }
  digitalWrite(LATCH_PIN,HIGH);
  delayMicroseconds(1);
  digitalWrite(LATCH_PIN,LOW);
  //Serial.println(millibeats,DEC);
}

int v = 0;

#define BUFSZ 32
unsigned char buf[BUFSZ];
unsigned int idx = 0;

void loadBufVal() {
  unsigned long beats = 0;
  if (idx > 6) idx = 6;
  for (int i = 0; i < idx; i++) {
    beats *= 10;
    beats += (unsigned char)(buf[i] - '0');
  }
  unsigned long mult = 1000L;
  while (idx >= 4) {
    idx--;
    mult /= 10L;
  }
  millibeats = beats * mult;
  Serial.print(millibeats,DEC);
  Serial.print(" ");
}

void loop() {
  while (Serial.available()) {
    int nc = Serial.read();
    if (nc == -1) continue;
    if (nc == '\n') {
      loadBufVal();
      Serial.write("set time\n");
      idx = 0;
    } else {
      buf[idx++] = nc;
      if (idx >= BUFSZ) {idx--;}
    }
  }
};

