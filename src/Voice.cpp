
#include "Voice.h"

namespace Voice {

Voice::Voice()
	: m_Envelope(Envelope::ADSRGenerator::State::Release)
	, m_Key()
	, m_Velocity()
{

}

Voice::Voice(TKey key, TVelocity velocity)
	: m_Envelope(Envelope::ADSRGenerator::State::Attack)
	, m_Key(key)
	, m_Velocity(velocity)
{

}

void Voice::Update(Envelope::TTime dt)
{
	const Envelope::ADSRGenerator::ADSR adsr = {
		100000,
		100000,
		1000000,
		m_Velocity * EnvelopeValueScale,
		m_Velocity * EnvelopeValueScale / 5,
	};

	m_Envelope.Update(dt, adsr);
}

Envelope::TValue Voice::GetValue() const
{
	return m_Envelope.GetValue() / EnvelopeValueScale;
}

TKey Voice::GetKey() const
{
	return m_Key;
}

void Voice::Release()
{
	m_Envelope.Release();
}

void Stack::Update(Envelope::TTime dt)
{
	for (auto& voice: m_Voices)
	{
		voice.Update(dt);
	}
}

void Stack::OnNoteOn(TKey key, TVelocity velocity)
{
	auto& voice = GetNextVoice();
	voice = Voice(key, velocity);
}

void Stack::OnNoteOff(TKey key)
{
	for (auto& voice: m_Voices)
	{
		// release *all* matching voices
		if (voice.GetKey() == key)
		{
			voice.Release();
		}
	}
}

Voice& Stack::GetNextVoice()
{
	auto& ret = m_Voices[m_NextVoice];
	m_NextVoice = (m_NextVoice + 1) % m_Voices.size();
	return ret;
}

const Voice* Stack::begin() const
{
	return m_Voices.begin();
}

const Voice* Stack::end() const
{
	return m_Voices.end();
}

Voice* Stack::begin()
{
	return m_Voices.begin();
}

Voice* Stack::end()
{
	return m_Voices.end();
}

}
