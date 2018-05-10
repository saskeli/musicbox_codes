
#define TAU 6.2831853
#define C_FREQ 100000
#define WL 400000
#define t1 0.1666666666666
#define t2 0.0083333333333
#define t3 0.0001984126984

typedef int (*valueCalc)(float);

volatile long stepA = 8;
volatile long stepB = 8;
volatile long posA = 0;
volatile long posB = 0;


volatile short AM_IN = 0;
volatile short FM_IN = 0;

volatile valueCalc aCalc = &square;
volatile valueCalc bCalc = &square;

IntervalTimer tim;
IntervalTimer tom;


void setup() {                
  analogWriteFrequency(20, 375000);
  pinMode(20, OUTPUT); // LFO B output
  pinMode(23, INPUT); // Freq A input
  pinMode(21, INPUT); // Freq B input
  pinMode(18, INPUT); // LFO A waveform
  pinMode(17, INPUT); // LFO B waveform
  pinMode(12, INPUT); // Internal AM
  pinMode(11, INPUT); // Internal FM
  tim.priority(3);
  tom.priority(4);
  tom.begin(readstuff, 10000);
  tim.begin(refresh, 1000000/C_FREQ);
}

void loop() {
}

void readstuff() {
  stepA = 2 * (analogRead(A9) / 17) + 2;
  stepB = 2 * (analogRead(A7) / 17) + 2;
  int readVal = analogRead(A4);
  aCalc = readVal < 818 ? (readVal < 614 ? (readVal < 409 ? (readVal < 205 ? &square : &sine) : &sawU) : &sawD) : &triangle;
  readVal = analogRead(A3);
  bCalc = readVal < 818 ? (readVal < 614 ? (readVal < 409 ? (readVal < 205 ? &square : &sine) : &sawU) : &sawD) : &triangle;
  AM_IN = digitalReadFast(12);
  FM_IN = digitalReadFast(11);
}

void refresh() {
  posA += stepA;
  posA %= WL;
  int aVal = aCalc(posA / (float) WL);
  analogWrite(A6, aVal);
  posB += stepB + (FM_IN ? stepB * aVal / 255 : 0);
  posB %= WL;
  int bVal = bCalc(posB / (float) WL);
  if (AM_IN) bVal = (int) (bVal * (aVal + 255)/512);
  analogWrite(A14, bVal);
}

int square(float frac) {
  return frac < 0.5 ? 255 : 0;
}

float msin(float x) {
  float ret = x;
  float nx = -x * x * x;
  ret += nx * t1;
  nx = -nx * x * x;
  ret += nx * t2;
  nx = -nx * x * x;
  return ret + nx*t3;
}

int sine(float frac) {
  float x = frac * TAU;
  return (int) (((x > PI ? -1 * msin(x - PI) : msin(x)) + 1) * 127);
}

int sawU(float frac) {
  return (int) (frac * 255);
}

int sawD(float frac) {
  return (int) ((1 - frac) * 255);
}

int triangle(float frac) {
  return (int) (frac < 0.5 ? 512 * frac : (1 - frac) * 512);
}


