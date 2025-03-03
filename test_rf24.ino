#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

const uint64_t local_address = 0xF9F9F9F9BB;
const uint64_t dest_address = 0xF9F9F9F9AA;

unsigned long last_send = 0;
uint8_t radioNumber = 0;
uint8_t id = 0;

struct s_payload {
  uint8_t id;
  char message[20];
};

void send() {  
  struct s_payload payload;
  payload.id = id++;
  strcpy(payload.message, "Salut !");

  radio.stopListening();
  //delay(10);
  //radio.openWritingPipe(dest_address);
  delay(10);
  
  bool report = radio.write(&payload, sizeof(s_payload)); //+1
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

  radio.setChannel(0); //0-127 
  radio.setDataRate(RF24_250KBPS); //RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  radio.setPALevel(RF24_PA_HIGH);  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, RF24_PA_MAX =0dBm

  radio.setPayloadSize(sizeof(s_payload));
  //radio.enableDynamicPayloads();

  //radio.setRetries(3,3);
  radio.setAutoAck(false);
  radio.disableCRC();
  radio.powerUp(); //disable low power mode

  radio.openWritingPipe(local_address);
  radio.openReadingPipe(1, dest_address);

  radio.startListening();
  Serial.println("listening...");

}  // setup


void loop() {

  if (radio.available()) {

    struct s_payload payload_received;
    radio.read(&payload_received, sizeof(s_payload));

    Serial.print(F("Received "));
    Serial.print(payload_received.id);
    Serial.print(payload_received.message);
    Serial.println();
  }

  delay(100);

  if (radioNumber && millis() - last_send > 3000) {
    Serial.println("sending !");
    send();
    last_send = millis();
  }

}  // loop