#include "mbed.h"
#include "USBSerial.h"
 
//Virtual serial port over USB
USBSerial serial;
//DigitalOut test(LED_RED);
SPI ledStrip(/* mosi */P0_27, /* miso */P0_20, /* sclk */P0_24);

const auto NumLeds = 60;
const char startFrame[] = { 0x00, 0x00, 0x00, 0x00 };
const char endFrame[] = { 0xff, 0xff, 0xff, 0xff };
char pixels[NumLeds * 4];

int main(void) {
	//test = 1;
	//ledStrip.format(8, 0);
	ledStrip.frequency(512000);
	memset(pixels, 0, sizeof(pixels));
	for (auto i = 0; i < NumLeds; i++)
	{
		auto* led = pixels + i * 4;
		led[0] = 0xe0 + 0x01;
		led[1] = 255 * i / (NumLeds - 1);
		led[2] = 255 - led[1];
	}
	

	while(1)
	{
		//test = !test;
		ledStrip.lock();
		ledStrip.write(startFrame, sizeof(startFrame), nullptr, 0);
		ledStrip.write(pixels, sizeof(pixels), nullptr, 0);
		ledStrip.write(endFrame, sizeof(endFrame), nullptr, 0);
		ledStrip.unlock();
		//serial.printf("I am a virtual serial port 13.2.2022\r\n");
		//wait(1);
	}
}
