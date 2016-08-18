#include <cox.h>

Timer tPrint;

static void printTask(void *) {
  System.ledToggle();
  printf("[%lu usec] Hi!\n", micros());
}

static void keyboard(SerialPort&) {
  System.ledToggle();
  printf("[%lu usec] Keyboard input\n", micros());
}

void setup() {
  System.ledOn();
  Serial.begin(115200);
  printf("\n*** [ATSAMR21-XPro] Basic functions ***\n");

  tPrint.onFired(printTask, NULL);
  tPrint.startPeriodic(1000);

  Serial.listen();
  Serial.onReceive(keyboard);
}
