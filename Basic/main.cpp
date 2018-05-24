#include <cox.h>

Timer tPrint;

static void printTask(void *) {
  System.ledToggle();

  struct timeval t;
  gettimeofday(&t, NULL);
  printf("[%lu.%06lu] Hi!\n", (uint32_t) t.tv_sec, t.tv_usec);
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
  tPrint.startPeriodic(100);

  Serial.listen();
  Serial.onReceive(keyboard);
}
