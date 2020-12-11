#pragma once

#include "solutions/sol.hpp"

namespace console {
	enum class RunState {
		NotRun,
		Waiting,
		Running,
		Done
	};
	
	struct SolutionState {
		RunState state = RunState::NotRun;
		const char* failedMessage = nullptr;
		uint32_t elapsedTime;
		AnswerBuffer answers;
	};
	
	extern SolutionState solStates[25];
	
	void runAllNoInteraction();
	void runSolution(int day);
	
	void init();
	void update();
	
	void updateInput(int touchX, int touchY);
	
	void updateOutsideIRQ();
}
