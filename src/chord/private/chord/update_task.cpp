#include "chord/update_task.h"
#include "chord/local_node.h"

namespace Chord
{
	UpdateTask::UpdateTask(LocalNode * _node)
		: node{_node}
		, updateTimer{1.f}
		, checkTimer{1.f} {}
	
	bool UpdateTask::init()
	{
		currTick = prevTick = clock();
		return node;
	}

	int32 UpdateTask::run()
	{
		const bool bRunning = false;
		while (bRunning)
		{
			// Update time variables
			// ! Just here
			const float32 dt = (currTick = clock()) - prevTick;
			prevTick = currTick;

			if (updateTimer.tick(dt))
				node->update();
		}
	}
} // namespace Chord
