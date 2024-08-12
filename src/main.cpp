#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED 2
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define STEP_DATA_CHAR_UUID "beefcafe-36e1-4688-b7f5-00000000000b"

// Valor específico que deve acionar o LED 1 vez.
const std::string TARGET_VALUE_UP = "UP";

// Valor específico que deve acionar o LED 1 vez.
const std::string TARGET_VALUE_DOWN = "DOWN";

BLECharacteristic *DataCharacteristic;

// Função global para piscar o LED
void blinkUpLED()
{
  digitalWrite(LED, HIGH);
  delay(100); // Tempo em milissegundos
  digitalWrite(LED, LOW);
  delay(100); // Tempo em milissegundos
}

void blinkDownLED()
{
  digitalWrite(LED, HIGH);
  delay(100); // Tempo em milissegundos
  digitalWrite(LED, LOW);
  delay(100); // Tempo em milissegundos
  digitalWrite(LED, HIGH);
  delay(100); // Tempo em milissegundos
  digitalWrite(LED, LOW);
  delay(100); // Tempo em milissegundos
}

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    // Verifica se os dados foram escritos na característica
    std::string value = pCharacteristic->getValue();

    // Se o valor recebido for igual ao TARGET_VALUE, pisca o LED
    if (value == TARGET_VALUE_UP)
    {
      Serial.println("Recebido valor correspondente: " + String(value.c_str()));
      blinkUpLED(); // Chama a função global para piscar o LED
    }
    else if (value == TARGET_VALUE_DOWN)
    {
      Serial.println("Recebido valor correspondente: " + String(value.c_str()));
      blinkDownLED();
    }
  }
};

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Garantir que o LED esteja apagado no início

  BLEDevice::init("Ble");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  DataCharacteristic = pService->createCharacteristic(
      STEP_DATA_CHAR_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  DataCharacteristic->addDescriptor(new BLE2902());

  // Define o callback para a característica
  DataCharacteristic->setCallbacks(new MyCallbacks());

  // Inicia o serviço e a publicidade
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x66); // Define preferências de intervalo de scan
  BLEDevice::startAdvertising();
  Serial.println("BLE device is ready to be connected");
}

void loop()
{
  // Envia notificações periodicamente
  delay(5000);
  static int stepCount = 0;
  stepCount++;
  String stepData = String(stepCount);
  DataCharacteristic->setValue(stepData.c_str());
  DataCharacteristic->notify();
}
