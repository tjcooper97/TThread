#include <TThread.h>

#define t_cpuusage    0
#define t_cpuusagerpt 1
#define t_blink       2
#define threadCount   3

TThread t[threadCount];

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  t[t_cpuusage].setTrigType(tt_INTERVAL);
  t[t_cpuusage].setInterval(250);
  t[t_cpuusage].init(thread_cpuusage);

  t[t_cpuusagerpt].setTrigType(tt_BOOLR);
  t[t_cpuusagerpt].init(thread_cpuusage_reporting);

  pinMode(LED_BUILTIN,OUTPUT);
  t[t_blink].setTrigType(tt_INTERVAL);
  t[t_blink].setInterval(500);
  t[t_blink].init(thread_blink);
}

void loop() { for (byte i = 0; i < threadCount; i++) { t[i].run(false); }; }



String formatdNumber(double v, byte l, String d) { String r = String(v); while (r.length() < l) { r = d + r; }; return r; }



void thread_cpuusage() { for (byte c = 0; c < threadCount; c++) { t[c].calcUsage(); }; t[t_cpuusagerpt].setTriggered(true); delay(random(30,40)); }

void thread_cpuusage_reporting() {
  Serial.print("CPU Usage at [" + String(millis()) + "]  |  ");
  double total = 0;
  for (byte r = 0; r < threadCount; r++) { Serial.print(String(r) + ": " + formatdNumber(t[r].getCPUUsage(false),5," ") + "%  |  "); total += t[r].getCPUUsage(false); };
  Serial.println("Total: " + String(total) + "%");
  delay(random(20,30));
}


bool LEDISON = false;
void thread_blink() { digitalWrite(LED_BUILTIN,(LEDISON ? LOW : HIGH)); LEDISON = !LEDISON; delay(random(10,20)); }