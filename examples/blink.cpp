#include <TThread.h>

TThread t_blink;

void setup() {
  t_blink.setTrigType(tt_INTERVAL);
  t_blink.setInterval(500);
  t_blink.init(thread_blink);
}

void loop() { t_blink.run(false); }



bool LEDISON = false;
void thread_blink() { digitalWrite(LED_BUILTIN,(LEDISON ? LOW : HIGH)); LEDISON = !LEDISON; }