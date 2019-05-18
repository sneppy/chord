#pragma once

#include "coremin.h"

#include "net/types.h"
#include "net/socket_dgram.h"
#include "net/socket_stream.h"

//////////////////////////////////////////////////
// Chord forwards
//////////////////////////////////////////////////

namespace Chord
{
	using namespace Net;

	class LocalNode;
	class ReceiveTask;
	class UpdateTask;
	class ListenTask;
	class ServeTask;
} // namespace Chord

#include "types.h"
#include "request.h"