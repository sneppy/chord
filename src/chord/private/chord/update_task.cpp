#include "chord/update_task.h"
#include "chord/local_node.h"

namespace Chord
{
	UpdateTask::UpdateTask(LocalNode * _node)
		: node{_node}
		, updateTimer{1.f}
		, checkTimer{2.f} {}
	
	bool UpdateTask::init()
	{
		currTick = prevTick = clock();
		return node;
	}

	int32 UpdateTask::run()
	{
		const bool bRunning = true;
		while (bRunning)
		{
			// Update time variables
			const float32 dt = ((currTick = clock()) - prevTick) / (float32)CLOCKS_PER_SEC;
			prevTick = currTick;

			// Run updates
			if (updateTimer.tick(dt))
			{
				node->stabilize();
				node->fixFingers();
			}

			// Run checks
			const float32 delta = checkTimer.getDelta();
			if (checkTimer.tick(dt))
			{
				node->checkPredecessor();
				node->checkRequests(delta);
			}
		}
	}
} // namespace Chord
