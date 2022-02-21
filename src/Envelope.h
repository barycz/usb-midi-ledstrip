
#pragma once

namespace Envelope
{

using TTime = unsigned;
using TValue = unsigned;

class ADSRGenerator
{
public:
	enum class State
	{
		Attack, Decay, Release
	};

	struct ADSR
	{
		TTime Attack;
		TTime Decay;
		TTime Release;

		TValue Peak;
		TValue Sustain;
	};

	ADSRGenerator(State state = State::Release);

	void Update(TTime dt, ADSR adsr);
	void Release();
	TValue GetValue() const;

private:
	TValue m_Value;
	State m_State;
};

}