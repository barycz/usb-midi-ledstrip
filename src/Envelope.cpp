
#include "Envelope.h"

namespace Envelope
{

ADSRGenerator::ADSRGenerator(State state)
	: m_Value()
	, m_State(state)
{

}

void ADSRGenerator::Update(TTime dt, ADSR adsr)
{
	switch (m_State)
	{
	case State::Attack:
		m_Value += adsr.Peak * dt / adsr.Attack;
		if (m_Value >= adsr.Peak)
		{
			m_Value = adsr.Peak;
			m_State = State::Decay;
		}
		break;

	case State::Decay:
		m_Value -= (adsr.Peak - adsr.Sustain) * dt / adsr.Decay;
		if (m_Value < adsr.Sustain || m_Value > adsr.Peak)
		{
			m_Value = adsr.Sustain;
		}
		break;

	case State::Release:
		m_Value -= adsr.Sustain * dt / adsr.Release;
		if (m_Value > adsr.Peak)
		{
			m_Value = 0;
		}
		break;
	}
}

void ADSRGenerator::Release()
{
	m_State = State::Release;
}

TValue ADSRGenerator::GetValue() const
{
	return m_Value;
}

}
