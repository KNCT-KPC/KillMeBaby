const int SP_FRONT = 3;
const int SP_BACK = 11;
const int SW_PORT = 8;
const int LED_PORT = 7;

const int VOLUME = 16;
const int FD = 200;
const int BD = FD;
const int ISI = -50;
const int BACK_START = FD + ISI;
const int TOTAL_END = BACK_START + BD;

bool running_flg;
unsigned long start_time;

void setup() {
  running_flg = false;
  TCCR2B = ((TCCR2B & B11111000) | B00000101);
  
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
  start_time = millis();
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

void driveDevice(unsigned long nowtime)
{
  bool flg = (nowtime < TOTAL_END);
  
  analogWrite(SP_FRONT, (nowtime < FD) ? VOLUME : 0);
  analogWrite(SP_BACK, ((BACK_START < nowtime) && flg) ? VOLUME : 0);
  digitalWrite(LED_PORT, flg ? HIGH : LOW);
  
  if (flg) return;
  running_flg = false;
}

void loop() {
  recvSerial();
  checkButton();
  if (!running_flg) return;
  driveDevice(millis() - start_time);
}

