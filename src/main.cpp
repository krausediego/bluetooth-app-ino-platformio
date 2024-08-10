#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *DataCharacteristic;

#define LED 2
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define STEP_DATA_CHAR_UUID "beefcafe-36e1-4688-b7f5-00000000000b"

int stepCount = 0;

void setup()
{
  pinMode(LED, OUTPUT);

  BLEDevice::init("Ble");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  DataCharacteristic = pService->createCharacteristic(
      STEP_DATA_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  DataCharacteristic->addDescriptor(new BLE2902());

  // Start BLE server and advertising
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x66);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE device is ready to be connected");
}

void loop()
{
  delay(5000);
  stepCount++;
  String stepData = String(stepCount);
  DataCharacteristic->setValue(stepData.c_str());
  DataCharacteristic->notify();
  delay(5000);
}