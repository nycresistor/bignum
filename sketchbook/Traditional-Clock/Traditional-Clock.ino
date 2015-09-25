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

volatile unsigned long dsecs = 0;

unsigned long usec_per_decisec = 100000L;

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN,LOW);
  Serial.begin(15200);
  Timer3.initialize(usec_per_decisec);
  Timer3.attachInterrupt(next_decisec);
}

void next_decisec() {
  dsecs++;
  if (dsecs >= (24L * 60L * 60L * 10L)) {
    dsecs -= (24L * 60L * 60L * 10L);
  }
  unsigned char d[6];
  unsigned long secs = (dsecs / 10L) % 60L;
  unsigned long mins = (dsecs / (10L * 60L)) % 60L;
  unsigned int hrs = (dsecs / (10L * 60L * 60L)) % 24L;  
  d[5] = secs % 10;
  d[4] = secs / 10;
  d[3] = mins % 10;
  d[2] = mins / 10;
  d[1] = hrs % 10;
  d[0] = hrs / 10;
  
  for (int i = 0; i < 6; i++) {
    SPI.transfer(digits[d[i]]);
  }
  digitalWrite(LATCH_PIN,HIGH);
  delayMicroseconds(1);
  digitalWrite(LATCH_PIN,LOW);
}

int v = 0;

#define BUFSZ 32
unsigned char buf[BUFSZ];
unsigned int idx = 0;

long ctv(unsigned char c) { return (c - '0'); }

void loadBufVal() {
  unsigned long hr = (ctv(buf[0]) * 10) + ctv(buf[1]);
  unsigned long min = (ctv(buf[2]) * 10) + ctv(buf[3]);
  unsigned long sec = (ctv(buf[4]) * 10) + ctv(buf[5]);
  dsecs = (hr * 60L * 60L * 10L) + (min * 60L * 10L) + (sec * 10L);
  Serial.print(dsecs,DEC);
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

