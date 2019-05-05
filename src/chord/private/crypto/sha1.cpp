#include "crypto/sha1.h"

void Crypto::sha1(const String & str, uint32 hash[5])
{
	// Init hash
	hash[0] = 0x67452301;
	hash[1] = 0xEFCDAB89;
	hash[2] = 0x98BADCFE;
	hash[3] = 0x10325476;
	hash[4] = 0xC3D2E1F0;

	// String length
	const uint32 len = str.getLength();

	// Get a modifiable copy of str
	Array<ansichar> data(str.getArray());

	// Append a `1`
	data.add(0x80);

	// Align to 64 byte by padding with zeros
	const uint32 align		= Math::alignUp(len + 1, 0x40);
	const uint32 padding	= align - len;
	data.resize(align);
	PlatformMemory::memset(*data + len + 1, 0x0, padding - 8);

	// Append message length
	reinterpret_cast<uint64*>(*data + align - 8)[0] = (uint64)len;

	// Process blocks of 64 bytes
	for (uint32 k = 0; k < align; k += 0x40)
	{
		const uint32 * words = reinterpret_cast<uint32*>(*data + k);

		// Extend to 80 words of 32-bit
		uint32 w[80]; PlatformMemory::memcpy(w, words, 0x10 * sizeof(uint32));
		for (uint32 i = 0x10; i < 0x50; ++i)
			w[i] = Math::shiftRotateLeft<uint32>(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
		
		// Working values
		uint32 h[5] = {
			hash[0],
			hash[1],
			hash[2],
			hash[3],
			hash[4]
		};

		for (uint32 i = 0; i < 0x50; ++i)
		{
			uint32 f, k;

			switch (i / 20)
			{
				case 0:
					f = (h[1] & h[2]) ^ (~h[1] & h[3]);
					k = 0x5A827999;
					break;
				
				case 1:
					f = h[1] ^ h[2] ^ h[3];
					k = 0x6ED9EBA1;
					break;
					
				case 2:
					f = (h[1] & h[2]) ^ (h[1] & h[3]) ^ (h[2] & h[3]);
					k = 0x8F1BBCDC;
					break;
				
				case 3:
					f = h[1] ^ h[2] ^ h[3];
					k = 0xCA62C1D6;
					break;
			}

			uint32 t = Math::shiftRotateLeft(h[0], 5) + f + h[4] + k + w[i];
			h[4] = h[3];
			h[3] = h[2];
			h[2] = Math::shiftRotateLeft(h[1], 30);
			h[1] = h[0];
			h[0] = t;
		}
		
		// Update current hash value
		for (int32 i = 0; i < 5; ++i)
			hash[i] += h[i];
	}
}