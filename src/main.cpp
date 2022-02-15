#include "mbed.h"
#include "rtos/rtos.h"
#include "USBMIDI.h"

BusOut statusLeds(LED_RED, LED_GREEN, LED_BLUE);
USBMIDI midi;
SPI ledStrip(/* mosi */P0_27, /* miso */P0_20, /* sclk */P0_24);

const auto NumLeds = 60;
const char startFrame[] = { 0x00, 0x00, 0x00, 0x00 };
const char endFrame[] = { 0xff, 0xff, 0xff, 0xff };
char pixels[NumLeds * 4];
auto noteCounter = 0;
bool noteHistory[NumLeds];
auto midiClock = 0;

void midiCallback(MIDIMessage msg)
{
	switch (msg.type())
	{
		case MIDIMessage::NoteOnType:
			statusLeds[0] = 0;
			noteHistory[0] = true;
			++noteCounter;
			break;
		case MIDIMessage::NoteOffType:
			statusLeds[0] = 1;
			break;
		case MIDIMessage::SysExType:
			if (msg.data[1] == 0xf8)
			{
				++midiClock;
			}
			break;
		default:;
	}
}

void updateHistory()
{
	for (auto i = NumLeds - 1; i > 0; --i)
	{
		noteHistory[i] = noteHistory[i - 1];
	}
	noteHistory[0] = false;
}

int main(void)
{
	midi.attach(midiCallback);
	ledStrip.frequency(512000);
	statusLeds = -1;
	memset(noteHistory, 0, sizeof(noteHistory));

	while(1)
	{
		if (midiClock >= 6) // 24 per quater note
		{
			updateHistory();
			midiClock = 0;
		}

		memset(pixels, 0, sizeof(pixels));
		for (auto i = 0; i < NumLeds; i++)
		{
			auto* led = pixels + i * 4;
			if (noteHistory[i])
			{
				led[0] = 0xe0 + 0x10;
				led[1] = 0;
				led[2] = 0;
				led[3] = 255;
			}
			else
			{
				led[0] = 0xe0 + 0x01;
				led[1] = 255 * i / (NumLeds - 1);
				led[2] = 255 - led[1];
			}
		}

		ledStrip.lock();
		ledStrip.write(startFrame, sizeof(startFrame), nullptr, 0);
		ledStrip.write(pixels, sizeof(pixels), nullptr, 0);
		ledStrip.write(endFrame, sizeof(endFrame), nullptr, 0);
		ledStrip.unlock();
		//wait_ms(16);
	}
}
