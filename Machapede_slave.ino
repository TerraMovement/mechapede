#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00002";
const byte base_address[6] = "tivmf";

#define MOT_A1_PIN 10
#define MOT_A2_PIN 9
#define MOT_B1_PIN 6
#define MOT_B2_PIN 5

int arr[2];

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Set all the motor control inputs to OUTPUT
  pinMode(MOT_A1_PIN, OUTPUT);
  pinMode(MOT_A2_PIN, OUTPUT);
  pinMode(MOT_B1_PIN, OUTPUT);
  pinMode(MOT_B2_PIN, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(MOT_A1_PIN, LOW);
  digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW);
  digitalWrite(MOT_B2_PIN, LOW);
}

void loop() {
  if (radio.available()) {
    radio.read(&arr, sizeof(arr));

    Serial.println(arr[0]);
    Serial.println(arr[1]);

    // Map the joystick input range (0-1023) to the desired output range (-255 to 255)
    int mapped_value_x = map(arr[0], -14, 1009, -255, 255);
    int mapped_value_y = map(arr[1], -14, 1009, -255, 255);

    Serial.println(mapped_value_x);
    Serial.println(mapped_value_y);

    // Calculate PWM values for each motor based on joystick input
    int pwm_left = mapped_value_x + mapped_value_y;
    int pwm_right = mapped_value_x - mapped_value_y;

    spin_and_wait(pwm_left, pwm_right, 0);
  }
}

void spin_and_wait(int pwm_A, int pwm_B, int duration) {
  // Ensure the PWM values are within the valid range
  pwm_A = constrain(pwm_A, -255, 255);
  pwm_B = constrain(pwm_B, -255, 255);

  set_motor_currents(pwm_A, pwm_B);
  delay(duration);
}

void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN) {
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, 0);
    analogWrite(IN2_PIN, -pwm);
  } else if (pwm > 0) { // forward
    analogWrite(IN1_PIN, pwm);
    analogWrite(IN2_PIN, 0);
  } else { // stop
    analogWrite(IN1_PIN, 0);
    analogWrite(IN2_PIN, 0);
  }
}

void set_motor_currents(int pwm_A, int pwm_B) {
  set_motor_pwm(pwm_A, MOT_A1_PIN, MOT_A2_PIN);
  set_motor_pwm(pwm_B, MOT_B1_PIN, MOT_B2_PIN);

  // Print a status message to the console.
  Serial.print("Set motor A PWM = ");
  Serial.print(pwm_A);
  Serial.print(" motor B PWM = ");
  Serial.println(pwm_B);
}
