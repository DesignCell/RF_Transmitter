#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

// ADC Interval Timer
uint32_t Time_Previous = 0;
uint32_t Time_Current = 0;

// Define Analog Inputs
#define Joy_1X A0 // Joystick 1 X Axis
#define Joy_1Y A1 // Joystick 1 Y Axis
#define Joy_2X A2 // Joystick 2 X Axis
#define Joy_2Y A3 // Joystick 2 Y Axis
#define Joy_2XT A4 // Joystick 2 X Trim
#define Joy_2YT A5 // Joystick 2 X Trim

// Define Digital Inputs
#define Joy_1B 2 // Button Joy1
#define Joy_2B 3 // Button Joy2

//Create RF Data Package. Max size of the Structure is 32 uint8_t (bytes)
struct Data_Package
{
	uint8_t JoyValue_1X;
	uint8_t JoyValue_1Y;
	uint8_t JoyValue_2X;
	uint8_t JoyValue_2XT;
	uint8_t JoyValue_2Y;
	uint8_t JoyValue_2YT;
	uint8_t JoyValue_1B;
	uint8_t JoyValue_2B;
	uint8_t ADC_Timer;
};

Data_Package rf_data; // Creates a variable with the above structure

void setup()
{
	radio.begin();
	radio.openWritingPipe(address); //Must match Receiver
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW); //Brodcast energy, higher energies need capacitor
	radio.stopListening();		   //Stop = Transmitter

	// Set Analog PinModes
	pinMode(Joy_1X, INPUT); // Joystick 1 X Axis
	pinMode(Joy_1Y, INPUT); // Joystick 1 Y Axis
	pinMode(Joy_2X, INPUT); // Joystick 2 X Axis
	pinMode(Joy_2Y, INPUT); // Joystick 2 Y Axis
	pinMode(Joy_2XT, INPUT); // Joystick 2 X Trim
	pinMode(Joy_2YT, INPUT); // Joystick 2 Y Trim
	//Set Digital PinModes
	pinMode(Joy_1B, INPUT_PULLUP); // Button Joy1
	pinMode(Joy_2B, INPUT_PULLUP); // Button Joy2
}

void loop()
{

	// ADC Start Timer
	Time_Previous = micros();

	rf_data.JoyValue_1X = map(analogRead(Joy_1X), 0, 1023, 0, 255);
	rf_data.JoyValue_1Y = map(analogRead(Joy_1Y), 0, 1023, 255, 0);
	rf_data.JoyValue_2X = map(analogRead(Joy_2X), 0, 1023, 0, 255);
	rf_data.JoyValue_2Y = map(analogRead(Joy_2Y), 0, 1023, 255, 0);

	rf_data.JoyValue_1B = digitalRead(Joy_1B);
	rf_data.JoyValue_2B = digitalRead(Joy_2B);

	// Slower frequency
	rf_data.JoyValue_2XT = map(analogRead(Joy_2XT), 0, 1023, 0, 255);
	rf_data.JoyValue_2YT = map(analogRead(Joy_2YT), 0, 1023, 0, 255);

	// ADC End Timer
	Time_Current = micros();

	rf_data.ADC_Timer = constrain((Time_Current - Time_Previous)/10, 0, 255);

	radio.write(&rf_data, sizeof(rf_data));
}