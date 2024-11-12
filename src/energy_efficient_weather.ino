#include <NimBLEDevice.h>
#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_SHT20.h>

// #######################################################
// # XIAO ESP32C3 -- ENSURE THE ESP IS SET FOR 80MHz !!! #
// #######################################################

/* Minimise energy consumption by slowing the CPU to the minimum speed which still supports BLE */
#if F_CPU != 80000000
#  error CPU speed should be set to 80MHz
#endif

#if !CONFIG_BT_NIMBLE_EXT_ADV
#  error Must enable extended advertising, see nimconfig.h file in your Documents -> Arduino -> libraries -> NimBLE-Arduino folder.
#endif

#define MILLIS_TO_MICROS(SEC) SEC ## 000ull
#define SLEEP_DURATION MILLIS_TO_MICROS(9700)
/* We broadcast this number of advertisements before performing another read of the sensors - to save just a little more energy */
#define BROADCASTS_PER_READ 6

NimBLEExtAdvertising *pAdvertising;
volatile bool isAdvertising = true;

RTC_DATA_ATTR float temperature = 0;
RTC_DATA_ATTR float humidity = 0;
RTC_DATA_ATTR uint8_t packetId = 0;
RTC_DATA_ATTR uint8_t broadcastNumber = BROADCASTS_PER_READ;

void performRead() {
  DFRobot_SHT20 sht20;
  broadcastNumber = 0;
  packetId++;
  sht20.initSHT20();
  delay(15);
  temperature=sht20.readTemperature();
  humidity=sht20.readHumidity();
  if (humidity > 99.98) {
	  humidity = 99.98;
  } else if (humidity < 0.02) {
	  humidity = 0.02;
  }
}

class AdvertisingCallbacks: public NimBLEExtAdvertisingCallbacks {
  void onStopped(NimBLEExtAdvertising* pAdv, int reason, uint8_t inst_id) {
    isAdvertising = false;
  }
};

void setup() {
  NimBLEDevice::init("");
  NimBLEExtAdvertisement extAdv(BLE_HCI_LE_PHY_CODED, BLE_HCI_LE_PHY_1M);
  uint8_t packet[25];
  int16_t val;

  if (++broadcastNumber >= BROADCASTS_PER_READ) {
    performRead();
  }

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setCallbacks(new AdvertisingCallbacks);
  extAdv.setConnectable(false);
  extAdv.setLegacyAdvertising(true);
  /* BTHome packet - see https://bthome.io/format/ */
  packet[0]=0x02;  // len=2
  packet[1]=0x01;  // flags
  packet[2]=0x06;
  packet[3]=0x08;  // len=8
  packet[4]=0x09;  // name
  packet[5]='W';
  packet[6]='e';
  packet[7]='a';
  packet[8]='t';
  packet[9]='h';
  packet[10]='e';
  packet[11]='r';
  packet[12]=0x0c;  // len=12
  packet[13]=0x16;  // 16-bit uuid
  packet[14]=0xd2;
  packet[15]=0xfc;
  packet[16]=0x40;	// bthome flags
  packet[17]=0x00;	// packet id
  packet[18]=packetId;
  packet[19]=0x02;	// temperature 0.01 C
  val = temperature * 100.0f;
  packet[20]=(val & 0xff);
  packet[21]=((val>>8)&0xff);
  packet[22]=0x03;	// humidity 0.01 pc
  val = humidity*100.0f;
  packet[23]=(val & 0xff);
  packet[24]=((val >> 8)&0xff);
  extAdv.setData(packet,25);
  if (pAdvertising->setInstanceData(0, extAdv)) {
    /* Send only one advertising packet */
    if (pAdvertising->start(0, 0, 1)) {
      do {
        yield();
      /* Exit out of this loop as soon as the packet is sent - saves energy! */
      } while (isAdvertising);
    }
  }
  NimBLEDevice::deinit();
  esp_deep_sleep(SLEEP_DURATION);
}

void loop() {
}
