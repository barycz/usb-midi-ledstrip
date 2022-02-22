
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
	const auto scaledVelocity = 64 + m_Velocity / 2;
	const Envelope::ADSRGenerator::ADSR adsr = {
		50000,
		50000,
		1000000,
		scaledVelocity * EnvelopeValueScale,
		scaledVelocity * EnvelopeValueScale / 5,
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
	auto& voice = GetNextVoice(key);
	voice = Voice(key, velocity);
}

void Stack::OnNoteOff(TKey key)
{
	auto* voice = GetVoiceByKey(key);
	if (voice)
	{
		voice->Release();
	}
}

Voice* Stack::GetVoiceByKey(TKey key)
{
	for (auto& voice: m_Voices)
	{
		if (voice.GetKey() == key)
		{
			return &voice;
		}
	}

	return nullptr;
}

Voice& Stack::GetNextVoice(TKey key)
{
	auto* recycledVoice = GetVoiceByKey(key);
	if (recycledVoice)
	{
		return *recycledVoice;
	}

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
