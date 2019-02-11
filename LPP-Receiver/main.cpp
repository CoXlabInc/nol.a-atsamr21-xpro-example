// -*- indent-tabs-mode:nil; -*-

#include "cox.h"

static void received(IEEE802_15_4Mac &radio, const IEEE802_15_4Frame *frame);
static void receivedProbe(uint16_t panId,
                          const uint8_t *eui64,
                          uint16_t shortId,
                          int16_t rssi,
                          const uint8_t *payload,
                          uint8_t payloadLen,
                          uint32_t channel);

static void ledOnTask(void *);
static void ledOffTask(void *);

uint16_t node_id = 2;
uint8_t node_ext_id[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0, 0};

LPPMac *Lpp;
Timer ledTimer;

void setup(void) {
  Serial.begin(115200);
  printf("\n*** [ATSAMR21-XPro] LPP Receiver ***\n");

  AT86RF233.begin();

  node_ext_id[6] = highByte(node_id);
  node_ext_id[7] = lowByte(node_id);

  Lpp = new LPPMac();
  Lpp->begin(AT86RF233, 0x1234, 0xFFFF, node_ext_id);
  Lpp->setProbePeriod(3000);
  Lpp->setListenTimeout(3300);
  Lpp->setTxTimeout(632);
  Lpp->setRxTimeout(465);
  Lpp->setRxWaitTimeout(30);
  //Lpp->setRadioAlwaysOn(true);

  Lpp->onReceive(received);
  Lpp->onReceiveProbe(receivedProbe);

  ledOnTask(NULL);
}

static void ledOnTask(void *) {
  ledTimer.onFired(ledOffTask, NULL);
  ledTimer.startOneShot(10);
  System.ledOn(0);
}

static void ledOffTask(void *) {
  ledTimer.onFired(ledOnTask, NULL);
  ledTimer.startOneShot(990);
  System.ledOff(0);
}

static void received(IEEE802_15_4Mac &radio, const IEEE802_15_4Frame *frame) {
  IEEE802_15_4Address srcAddr = frame->getSrcAddr();
  if (srcAddr.len == 2) {
    printf("RX : %x, ", srcAddr.id.s16);
  } else if (srcAddr.len == 8) {
    printf(
      "RX : %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x, ",
      srcAddr.id.s64[0],
      srcAddr.id.s64[1],
      srcAddr.id.s64[2],
      srcAddr.id.s64[3],
      srcAddr.id.s64[4],
      srcAddr.id.s64[5],
      srcAddr.id.s64[6],
      srcAddr.id.s64[7]
    );
  }
  printf("RSSI(%d dBm), LQI(%d) (length:%u)",
         frame->power,
         frame->meta.DSSS.corr,
         frame->getPayloadLength());

  for (uint16_t i = 0; i < frame->getPayloadLength(); i++)
    printf(" %02X", frame->getPayloadAt(i));
  printf("\n");
}

static void receivedProbe(uint16_t panId,
                          const uint8_t *eui64,
                          uint16_t shortId,
                          int16_t rssi,
                          const uint8_t *payload,
                          uint8_t payloadLen,
                          uint32_t channel) {
  printf("* Probe received from PAN:0x%04X", panId);

  if (eui64) {
    printf(", Node EUI64:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
            eui64[0], eui64[1], eui64[2], eui64[3],
            eui64[4], eui64[5], eui64[6], eui64[7]);
  }

  if (shortId != 0xFFFF) {
    printf(", ID:%x", shortId);
  }

  printf(", RSSI:%d", rssi);

  if (payloadLen > 0) {
    uint8_t i;

    printf(", length:%u, ", payloadLen);
    for (i = 0; i < payloadLen; i++) {
      printf("%02X ", payload[i]);
    }
  }

  printf("\n");
}
