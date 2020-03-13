int pMin = 0;  //the lowest value that comes out of the potentiometer
int pMax = 920; //the highest value that comes out of the potentiometer.
int pVal = 2;  //we will use this value to store the readings of the potentiometer
int lastpotvalue = 2;

unsigned long debounceDelay = 40;

int toggleState = HIGH;
int lastToggleState = HIGH;
int reading = 0;
unsigned long toggleDebounceTime = 0;

int initialized = 0;
bool clockRun = HIGH;

int manState = HIGH;
int lastManState = HIGH;
int manReading = 0;
unsigned long manDebounceTime = 0;


void setup()
{
  pinMode(PD2, OUTPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  Serial.begin(9600);

  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 32000;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  if(initialized == 1)
    digitalWrite(PD2, (clockRun) ? digitalRead(PD2) ^ 1 : LOW);  
}

void loop()
{
  if(initialized == 0)
  {
    delay(2000);
    initialized = 1;
  }
  pVal = analogRead(A0);
  manReading = digitalRead(9);
  reading = digitalRead(10);
  if(reading != lastToggleState && reading == LOW) 
  {
    toggleDebounceTime = millis();
  }

  if((millis() - toggleDebounceTime) > debounceDelay)
  {
    if(reading != toggleState)
    {
      toggleState = reading;
      if(toggleState == LOW)
      {
        clockRun = !clockRun;
      }
    }
  }
  lastToggleState = reading;

  if(manReading != lastManState && manReading == LOW) 
  {
    manDebounceTime = millis();
  }

  if((millis() - manDebounceTime) > debounceDelay)
  {
    if(manReading != manState)
    {
      manState = manReading;
      if(manState == LOW)
      {
        //just in case
        clockRun == LOW;

        //fire manual pulse
        digitalWrite(PD2, HIGH);
        delay(100);
        digitalWrite(PD2, LOW);
      }
    }
  }
  lastManState = manReading;

  
  pVal = map(pVal, pMin, pMax, 2, 100); 
  if(lastpotvalue != pVal && clockRun == HIGH)
  {
    OCR1A = 62500 / pVal;
  }
  lastpotvalue = pVal;
}
