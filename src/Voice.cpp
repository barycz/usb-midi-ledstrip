
#include "Voice.h"

namespace Voice {

Voice::Voice()
	: m_Key()
	, m_Velocity()
{

}

Voice::Voice(TKey key, TVelocity velocity)
	: m_Key(key)
	, m_Velocity(velocity)
{

}

void Voice::Update(Envelope::TTime dt)
{
	static const Envelope::ADSRGenerator::ADSR adsr = {0};

	m_Envelope.Update(dt, adsr);
}

Envelope::TValue Voice::GetValue() const
{
	return m_Envelope.GetValue();
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
	auto* const voice = GetByKey(key);

	// might happen from time to time (like when we miss note on message)
	if (voice == nullptr)
	{
		return;
	}

	voice->Release();
}

Voice* Stack::GetByKey(TKey key)
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

Voice& Stack::GetNextVoice()
{
	auto& ret = m_Voices[m_NextVoice];
	m_NextVoice = (m_NextVoice + 1) % m_Voices.size();
	return ret;
}

}
