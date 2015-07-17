#ifndef MURMURHASH_H
#define MURMURHASH_H

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

//-----------------------------------------------------------------------------
// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard
// construction. Bulk speed should be identical to Murmur2, small-key speed
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the algorithm
// more amenable to incremental implementations. All other caveats from
// MurmurHash2 still apply.
unsigned int MurmurHash2A ( const void * key, int len, unsigned int seed );

//-----------------------------------------------------------------------------
// CMurmurHash2A, by Austin Appleby

// This is a sample implementation of MurmurHash2A designed to work
// incrementally.

// Usage -

// CMurmurHash2A hasher
// hasher.Begin(seed);
// hasher.Add(data1,size1);
// hasher.Add(data2,size2);
// ...
// hasher.Add(dataN,sizeN);
// unsigned int hash = hasher.End()

class CMurmurHash2A
{
public:

	void Begin ( unsigned int seed = 0 );

	void Add ( const unsigned char * data, int len );

	unsigned int End ( void );

private:

	static const unsigned int m = 0x5bd1e995;
	static const int r = 24;

	void MixTail ( const unsigned char * & data, int & len );

	unsigned int m_hash;
	unsigned int m_tail;
	unsigned int m_count;
	unsigned int m_size;
};

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else   // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32  ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x86_128 ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );

//-----------------------------------------------------------------------------

#endif
