/*
 * This is an example project to showcase DaRe Coding for LoRaWAN.
 * It transmits the frame counter little endian every minute with R=1/2, W=8.
 */

//#define DEBUG

/*!
 * Include DaRe sources
 */
#include "libraries/DaRe/DaRe.cpp"
#include "libraries/DaRe/DaReEncode.cpp"

/*!
 * Variables used for DaRe
 */
#define DATA_POINT_SIZE 4 //Constant size of the original data to be send in bytes
DaReEncode Encoder; //DaRe encoder object
DaRe::Payload EncodedPayload; //Encoded payload object
uint8_t DataPoint[DATA_POINT_SIZE]; //Input array for the encoder, containing the uncoded data
uint32_t FrameCounter; //Variable to keep track of the LoRaWAN frame counter
uint8_t i;

/*!
 * devaddr and session keys for LoRaWAN transmission
 */
//The last 4 bytes are the DevAddr, leave the first byte unchanged
uint8_t CMD_DEV_ADDR[5] = { 0x21, 0x00, 0x00, 0x00, 0x00 };

//The last 16 bytes of appskey and nwkskey are the key values, leave the first two bytes unchanged
uint8_t CMD_APPSKEY[18] = { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t CMD_NWKSKEY[18] = { 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*!
 * Application port foor LoRaWAN transmission
 */
#define PORT 0x03

/*!
 * More LoRaWAN stack variables
 */
uint8_t CMD_STOP[1] = { 0x30 };
uint8_t CMD_LORA_PRVNET[2] = { 0x25, 0xA0 };
uint8_t CMD_START[1] = { 0x31 };
uint8_t CMD_SEND_PREFIX[4] = { 0x50, 0x08, 0x00, PORT };
//{0x50,0x08,0x00,PORT} for without ACK
//{0x50,0x0C,0x00,PORT} for with ACK

#define SERIAL_BAUD 57600 // For debug communication
#define SERIAL1_BAUD 9600 // For communication with the LoRaBee

#define PACKET_TIME_OUT 45000

uint8_t sendBuffer[256];

// Define payload for LoRaWAN transmission to be twice the uncoded data size plus 1 byte
#define LORAWAN_APP_DATA_SIZE (DATA_POINT_SIZE*2+1)
uint8_t AppData[LORAWAN_APP_DATA_SIZE] = {};
bool YouMaySend = true;

void setup() {
#ifdef DEBUG
  Serial.begin(SERIAL_BAUD);
#endif
  Serial1.begin(SERIAL1_BAUD);

#ifdef DEBUG
  Serial.println();
  Serial.println("------------------------------------------------");
  Serial.println("Starting the Embit LoRaWAN demo with DaRe Coding");
  Serial.println("------------------------------------------------");
#endif

  //Step 1.
#ifdef DEBUG
  Serial.println("Sending the network stop command");
#endif
  sendPacket(CMD_STOP, sizeof(CMD_STOP));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  //Step 2.
#ifdef DEBUG
  Serial.println("Setting the network preferences to LoRaWAN private network");
#endif
  sendPacket(CMD_LORA_PRVNET, sizeof(CMD_LORA_PRVNET));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  //Step 3.
#ifdef DEBUG
  Serial.println("Setting the DevAddr");
#endif
  sendPacket(CMD_DEV_ADDR, sizeof(CMD_DEV_ADDR));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  //Step 4.
#ifdef DEBUG
  Serial.println("Setting the AppSKey");
#endif
  sendPacket(CMD_APPSKEY, sizeof(CMD_APPSKEY));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  //Step 5.
#ifdef DEBUG
  Serial.println("Setting the NwkSKey");
#endif
  sendPacket(CMD_NWKSKEY, sizeof(CMD_NWKSKEY));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  //Step 6.
#ifdef DEBUG
  Serial.println("Sending the network start command");
#endif
  sendPacket(CMD_START, sizeof(CMD_START));
  readPacket();
#ifdef DEBUG
  Serial.println();
#endif

  // initialize timer interrupt for periodical transmission
  timer_interrupt_setup();

  // initialize DaRe encoder
  Encoder.init(&EncodedPayload, DATA_POINT_SIZE, DaRe::R_1_3, DaRe::W_32);
  Encoder.set(DaRe::R_1_2, DaRe::W_8);
  FrameCounter = 1;
}

void loop() {
  if (YouMaySend) {    
    /*!
     * Put the frame counter value as payload
     */
    DataPoint[0] = (FrameCounter & 0xFF);
    DataPoint[1] = ((FrameCounter >> 8) & 0xFF);
    DataPoint[2] = ((FrameCounter >> 16) & 0xFF);
    DataPoint[3] = ((FrameCounter >> 24) & 0xFF);

    /*!
     * Encode the payload
     */
    Encoder.encode(&EncodedPayload, DataPoint, FrameCounter);

    /*!
     * Put the encoded payload in the app data
     */
    for (i = 0; i < EncodedPayload.payloadSize; i++) {
        AppData[i] = EncodedPayload.payload[i];
    }
#ifdef DEBUG
    Serial.println();
#endif

    /*!
     * Transmit the payload
     */
    sendPayload(AppData, LORAWAN_APP_DATA_SIZE);
    YouMaySend = false;
  }

  delay(100);
}

/*!
 * Functions for sending a payload (from Sodaq example)
 */
void sendPayload(uint8_t* data, uint16_t length) {
#ifdef DEBUG
  Serial.println(String("Sending payload: ") + (char*)data);
  Serial.print("As HEX: ");
  for (size_t i = 0; i < length; i++) {
    printHex(data[i]);
  }
  Serial.println();
#endif

  memcpy(sendBuffer, CMD_SEND_PREFIX, sizeof(CMD_SEND_PREFIX));
  memcpy(&sendBuffer[sizeof(CMD_SEND_PREFIX)], data, length);

  sendPacket(sendBuffer, sizeof(CMD_SEND_PREFIX) + length);
  readPacket();
}

void sendPacket(uint8_t* data, uint16_t length) {
#ifdef DEBUG
  Serial.print("Sending: ");
#endif
  uint16_t packetLength = length + 3;
  uint8_t* len = (uint8_t*)&packetLength;
  uint8_t CRC = len[1] + len[0];

  sendByte(len[1]);
  sendByte(len[0]);

  for (size_t i = 0; i < length; i++) {
    CRC += data[i];
    sendByte(data[i]);
  }

  sendByte(CRC);
#ifdef DEBUG
  Serial.println();
#endif
}

void sendByte(uint8_t data) {
  Serial1.write(data);
#ifdef DEBUG
  printHex(data);
#endif
}

/*!
 * Functions to receive a packet (from Sodaq example)
 */
void serialEvent1() {
  readPacket();
}

void readPacket() {
  uint32_t maxTS = millis() + PACKET_TIME_OUT;
  uint16_t length = 4;
  uint8_t firstByte = 0;

#ifdef DEBUG
  Serial.print("Receiving: ");
#endif

  size_t i = 0;
  while ((maxTS > millis()) && (i < length)) {
    while ((!Serial1.available()) && (maxTS > millis()));

    if (Serial1.available()) {
      uint8_t value = Serial1.read();
      if (i == 0) {
        firstByte = value;
      }
      else if (i == 1) {
        length = firstByte * 256 + value;
      }
#ifdef DEBUG
      printHex(value);
#endif
      i++;
    }
  }

  if (i < length) {
#ifdef DEBUG
    Serial.print("Timeout");
#endif
  } else {
    FrameCounter = FrameCounter + 1;
  }
#ifdef DEBUG
  Serial.println();
#endif
}

void printHex(uint8_t hex) {
  char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  Serial.print(hexTable[hex / 16]);
  Serial.print(hexTable[hex % 16]);
  Serial.print(' ');
}

