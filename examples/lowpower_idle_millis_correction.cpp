// This code will only compile for an Arduino Mega or Uno because we're using LowPower.h




#include <TThread.h>
#include <LowPower.h>

TThread t_blink;


bool LEDISON = false;


void setup() {
  Serial.begin(115200);
  Serial.println(); Serial.println(F("TThread example to show how to handle the pausing of TIMER0 when sleeping")); Serial.flush();

  pinMode(LED_BUILTIN,OUTPUT);

  t_blink.setTrigType(tt_INTERVAL);
  t_blink.setInterval(1250);
  t_blink.init(thread_blink);
}

void loop() { t_blink.run(false); sleepme(SLEEP_500MS); }



void thread_blink() { 
  LEDISON = !LEDISON; 
  digitalWrite(LED_BUILTIN,(LEDISON ? HIGH : LOW)); 
  Serial.println(""); Serial.print(millis()); Serial.print(F(" - Set LED ")); Serial.println(LEDISON ? "On" : "Off");
}









void sleepme(period_t period) {
  if (period == SLEEP_FOREVER) { return; };
  
  Serial.println("");
  uint32_t sleepmillis = periodtomillis(period);
  Serial.print(millis()); Serial.print(F(" - Sleep start for ")); Serial.print(sleepmillis); Serial.println(F(" milliseconds"));
  Serial.flush();
    // LowPower.idle(period, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); // uno rev 3
    LowPower.idle(period, ADC_OFF, TIMER5_OFF, TIMER4_OFF, TIMER3_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART3_OFF, USART2_OFF, USART1_OFF, USART0_OFF, TWI_OFF); // mega 2560
  Serial.print(millis()); Serial.println(F(" - Sleep end"));

  Serial.print(F("  >Blink thread last run was ")); Serial.println(t_blink.getLastRunMillis());
    t_blink.setLastRunMillis(t_blink.getLastRunMillis() - sleepmillis);
    t_blink.setUsageStartMillis(t_blink.getUsageStartMillis() - sleepmillis);
  Serial.print(F("  >Blink thread last run now ")); Serial.println(t_blink.getLastRunMillis());
  Serial.print(F("  >Blink elapsed millis      ")); Serial.println(t_blink.getMillisSinceLastRun());
}























uint32_t periodtomillis(period_t period) {
  if      (period == SLEEP_15MS)  { return 15; }
  else if (period == SLEEP_30MS)  { return 30; }
  else if (period == SLEEP_60MS)  { return 60; }
  else if (period == SLEEP_120MS) { return 120; }
  else if (period == SLEEP_250MS) { return 250; }
  else if (period == SLEEP_500MS) { return 500; }
  else if (period == SLEEP_1S)    { return 1000; }
  else if (period == SLEEP_2S)    { return 2000; }
  else if (period == SLEEP_4S)    { return 4000; }
  else if (period == SLEEP_8S)    { return 8000; };
  return 0;
}