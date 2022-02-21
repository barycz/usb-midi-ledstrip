#include "Voice.h"
#include "mbed.h"
#include "rtos/rtos.h"
#include "USBMIDI.h"

BusOut statusLeds(LED_RED, LED_GREEN, LED_BLUE);
USBMIDI midi;
SPI ledStrip(/* mosi */P0_27, /* miso */P0_20, /* sclk */P0_24);

const auto UpdateInterval = 10000u; // us
const auto NumLeds = 60;
const auto NumNoteKeys = 128; // 0-127 are the note pitches
const auto LedToKeyOffset = 3*12; // skip 3 lower octaves
const char startFrame[] = { 0x00, 0x00, 0x00, 0x00 };
const char endFrame[] = { 0xff, 0xff, 0xff, 0xff };
using TPixels = char[NumLeds * 4];
TPixels pixels;
auto noteCounter = 0;
Voice::Stack voiceStack;
auto midiClock = 0u;

void midiCallback(MIDIMessage msg)
{
	switch (msg.type())
	{
		case MIDIMessage::NoteOnType:
			statusLeds[0] = 0;
			voiceStack.OnNoteOn(msg.key(), msg.velocity());
			++noteCounter;
			break;
		case MIDIMessage::NoteOffType:
			statusLeds[0] = 1;
			voiceStack.OnNoteOff(msg.key());
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

void Gradient(TPixels& pixels)
{
	for (auto i = 0; i < NumLeds; i++)
	{
		auto* led = pixels + i * 4;
		led[0] = 0xe0 + 0x01;
		led[1] = 255 * i / (NumLeds - 1);
		led[2] = 255 - led[1];
		led[3] = 0;
	}
}

void VoicesToLeds(const Voice::Stack& voices, TPixels& pixels)
{
	for (auto& voice: voices)
	{
		const auto voiceValue = voice.GetValue();
		if (voiceValue == 0)
		{
			continue;
		}
		
		const auto ledId = voice.GetKey() - LedToKeyOffset;
		if (ledId < 0 || ledId >= NumLeds)
		{
			continue;
		}

		auto* led = pixels + ledId * 4;
		led[0] = 0xe0 + 0x10;
		led[1] = 0;
		led[2] = 0;
		led[3] = voiceValue * 2; // scale 128 midi velicities
	}
}

int main(void)
{
	midi.attach(midiCallback);
	statusLeds = -1;

	while(1)
	{
		voiceStack.Update(UpdateInterval);

		Gradient(pixels);
		VoicesToLeds(voiceStack, pixels);

		ledStrip.lock();
		ledStrip.write(startFrame, sizeof(startFrame), nullptr, 0);
		ledStrip.write(pixels, sizeof(pixels), nullptr, 0);
		ledStrip.write(endFrame, sizeof(endFrame), nullptr, 0);
		ledStrip.unlock();
	}
}
