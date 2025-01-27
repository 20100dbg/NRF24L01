#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

const byte address[] = { 0xF0F0F0F0AA , 0xF0F0F0F0BB};

unsigned long last_send = 0;
uint8_t radioNumber = 0;
uint8_t id = 0;

struct s_payload {
  uint8_t id;
  char message[20];
};

void send() {
  Serial.println("sending !");
  
  struct s_payload payload;
  payload.id = id++;
  strcpy(payload.message, "Salut !");

  radio.stopListening();
  delay(10);
  bool report = radio.write(&payload, sizeof(s_payload));
  delay(10);

  radio.startListening();
}

void setup() {

  delay(2000);

  Serial.begin(115200);
  while (!Serial) { }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  //radio.setChannel(0);
  radio.setDataRate(RF24_250KBPS); //RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  radio.setPALevel(RF24_PA_HIGH);  //RF24_PA_MIN RF24_PA_LOW RF24_PA_HIGH RF24_PA_MAX 

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(s_payload));
  
  //radio.setRetries(3,3);
  radio.setAutoAck(false);
  radio.disableCRC();

  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);

  radio.startListening();
  Serial.println("listening...");
}  // setup


void loop() {

  if (radioNumber && millis() - last_send > 3000) {
    send();
    last_send = millis();
  }

  delay(10);

  if (radio.available()) {

    struct s_payload payload_received;
    radio.read(&payload_received, sizeof(s_payload));

    Serial.print(F("Received "));
    Serial.print(payload_received.id);
    Serial.print(payload_received.message);
    Serial.println();
  }

  delay(10);

}  // loop