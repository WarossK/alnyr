#pragma once

#include <functional>
#include <optional>

namespace alnyr
{
	struct alnyrState
	{
		virtual ~alnyrState() {}
		virtual void operator() () = 0;
	};

	class alnyrStateManager
	{
	private:
		alnyrState* now_state_;

	public:
		void StateRun()
		{
			(*now_state_)();
		}

		template<class State, class...StateArgs> void StateChange(StateArgs...state_args)
		{
			delete now_state_;
			now_state_ = new State(state_args...);
		}
	};
}