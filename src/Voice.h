
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
	// scale up values sent to the envelop to achieve better precision
	static const auto EnvelopeValueScale = 100u;

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

	const Voice* begin() const;
	const Voice* end() const;
	Voice* begin();
	Voice* end();

private:
	Voice& GetNextVoice();

	std::array<Voice, Polyphony> m_Voices;
	unsigned m_NextVoice = 0;
};

}