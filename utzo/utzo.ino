const int SP_FRONT = 3;
const int SP_BACK = 11;
const int SW_PORT = 8;
const int LED_PORT = 7;

const int FD = 200;
const int BD = FD;
const int ISI = -50;
const int BACK_START = FD + ISI;
const int TOTAL_END = BACK_START + BD;
const float SIN_COEF = 0.00157079633; // (2pi) / ((1 / f[Hz])[us])
const float ZERO_LEVEL = 127.5;
const int DELAY_MSEC = 125;

bool running_flg;
unsigned long start_time;

void setup() {
  running_flg = false;
  TCCR2B = TCCR2B & B11111000 | B00000001;  // 32KHz
  pinMode(SP_FRONT, OUTPUT);
  pinMode(SP_BACK, OUTPUT);
  pinMode(SW_PORT, INPUT_PULLUP);
  pinMode(LED_PORT, OUTPUT);

  Serial.begin(9600);
}

void startDevice()
{
  if (running_flg) return;

  running_flg = true;
  delay(DELAY_MSEC);
  start_time = micros();
}

void recvSerial()
{
  if (!(Serial.available() > 0)) return;
  char c = Serial.read();
  if (c != 'S') return;
  startDevice();
}

void checkButton()
{
  int state = digitalRead(SW_PORT);
  if (state == HIGH) return;
  startDevice();
}

void driveDevice(unsigned long us)
{
  unsigned long ms = us / 1000;
  bool flg = (ms < TOTAL_END);

  // Front
  if (ms < FD) {
    int v = (int)(ZERO_LEVEL * (sin(SIN_COEF * us) + 1.0));
    analogWrite(SP_FRONT, v);
  } else {
    analogWrite(SP_FRONT, 0);
  }

  // Back
  if ((BACK_START < ms) && flg) {
    int v = (int)(ZERO_LEVEL * (sin(SIN_COEF * (us - (BACK_START * 1000))) + 1.0));
    analogWrite(SP_BACK, v);
  } else {
    analogWrite(SP_BACK, 0);
  }
  
  digitalWrite(LED_PORT, flg ? HIGH : LOW);
  if (flg) return;
  running_flg = false;
}

void loop() {
  recvSerial();
  checkButton();
  if (!running_flg) return;
  driveDevice(micros() - start_time);
}

