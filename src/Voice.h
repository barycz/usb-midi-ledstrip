
#pragma once

#include "Envelope.h"

#include <array>

namespace Voice
{

using TKey = uint8_t; // note/pitch
using TVelocity = uint8_t;

class Voice
{
public:
	Voice();
	Voice(TKey key, TVelocity velocity);

	void Update(Envelope::TTime dt);
	TKey GetKey() const;
	Envelope::TValue GetValue() const;
	void Release();

private:
	Envelope::ADSRGenerator m_Envelope;
	TKey m_Key;
	TVelocity m_Velocity;
};

class Stack
{
public:
	static const auto Polyphony = 8u;

	void Update(Envelope::TTime dt);

	void OnNoteOn(TKey key, TVelocity velocity);
	void OnNoteOff(TKey key);

private:
	Voice& GetNextVoice();
	Voice* GetByKey(TKey key);

	std::array<Voice, Polyphony> m_Voices;
	unsigned m_NextVoice = 0;
};

}