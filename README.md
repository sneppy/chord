# Chord

C++ implementation of the [Chord protocol](https://pdos.csail.mit.edu/papers/ton:chord/paper-ton.pdf), a DHT (Distributed Hash Table) p2p protocol.

Contributors:

- Andrea Mecchia @ [sneppy](https://github.com/sneppy)
- Lorenzo Pratissoli @ [lpraat](https://github.com/lpraat)

## Usage

> The library is still a WIP

The following code spawns a new chord ring with a single node:

```cpp
#include "coremin.h"
#include "hal/threading.h"
#include "chord/chord.h"

int32 main()
{
	Chord::LocalNode node;
	auto receiver = RunnableThread::create(new Chord::ReceiveTask(&localNode), "Receiver");
	auto updater = RunnableThread::create(new Chord::UpdateTask(&localNode), "Updater");
	while (1);

	return 0;
}
```

To connect to an existing chord ring you must know the address of an existing node:

```cpp
Chord::LocalNode node;

// Connect to known host
char hostString[] = "node.chord.example:5000";
Ipv4 host; if (Net::getHostAddr(host, hostString)) node.join(host);

auto receiver = RunnableThread::create(new Chord::ReceiveTask(&localNode), "Receiver");
auto updater = RunnableThread::create(new Chord::UpdateTask(&localNode), "Updater");
while(1);
```

To perform a lookup:

```cpp
// This returns a promise
auto result = node.lookup(0xf5ec50d1);
printf("found key @ [%s]\n", *result.get().getInfoString());
```

If for some reason the key is not found the address of the returned peer is the wildcard address:

```cpp
auto result = node.lookup(key);
if (result.get().addr == Ipv4::any) printf("key not found\n");
```