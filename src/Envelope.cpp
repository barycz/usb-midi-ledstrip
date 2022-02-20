
#include "Envelope.h"

namespace Envelope
{

ADSRGenerator::ADSRGenerator()
	: m_Value()
	, m_State(State::Release)
{

}

void ADSRGenerator::Update(TTime dt, ADSR adsr)
{
	switch (m_State)
	{
	case State::Attack:
		break;

	case State::Decay:
		break;

	case State::Release:
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
