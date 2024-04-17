#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int SW_pin = 4; // Connected to joystick switch
const int X_pin = 0;  // Analog, connected to joystick X
const int Y_pin = 1;  // Analog, connected to joystick Y

RF24 radio(7, 8); // CE, CSN

const byte address_1[6] = "00001";
const byte address_2[6] = "00002";
const byte address_3[6] = "00003";
const byte base_address[6] = "tivmf";

int arr[2];
int addressIndex = 1; // Start with the first address

void setup() {
  radio.begin();
  setRadioAddress(); // Set initial radio address
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  Serial.begin(9600);
}

void loop() {
  if (digitalRead(SW_pin) == LOW) { // Check if the switch is pressed
    changeRadioAddress(); // Change the radio address
    delay(500); // Add a small delay for debounce
  }

  Serial.print("Switch: ");
  Serial.println(digitalRead(SW_pin));

  Serial.print("Xh: ");
  Serial.println(analogRead(X_pin));

  arr[0] = analogRead(X_pin);

  Serial.print("Yh: ");
  Serial.println(analogRead(Y_pin));

  arr[1] = analogRead(Y_pin);

  radio.write(&arr, sizeof(arr));

  Serial.println(addressIndex);

  delay(1000);
}

void setRadioAddress() {
  switch (addressIndex) {
    case 1:
      radio.openWritingPipe(address_1);
      break;
    case 2:
      radio.openWritingPipe(address_2);
      break;
    case 3:
      radio.openWritingPipe(address_3);
      break;
    case 4:
      radio.openWritingPipe(base_address);
      break;
    default:
      break;
  }
}

void changeRadioAddress() {
  addressIndex++; // Increment the address index
  if (addressIndex > 4) // If it exceeds the number of addresses, reset to 1
    addressIndex = 1;
  
  setRadioAddress(); // Set the new radio address
}
