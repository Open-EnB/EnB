/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	A stand-in for Westwood's wlib Dictionary<K,V> (libs\wwonline\wlib\dictionary.h; EA's copy:
**	Generals Tools\matchbot\wlib), whose inlined code UDPConnection has (its channel table, the
**	constructors 0x82C680 / 0x82C980 / 0x82CBF0, the send core 0x82D420, the destructor 0x82CDC0;
**	shrink / expand 0x82E6A0 / 0x82EBB0, remove 0x82E8A0, getPointer 0x82E800). The layout is wlib's
**	(0x34 bytes): the table, entries, size, tableBits, log2Size, keepSize, the hash function, the
**	shrink / expand thresholds 0.2 / 0.8 and the minimum size 128; nodes {key, value, hashNext}.
**	Not EA's code: the methods are written for the harness (WWOnline is an open decision, as for
**	Wtime), with wlib's names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DICTIONARY_H  // NAME 4 [dictionary.h]: wlib's dictionary.h (EA's Generals Tools matchbot wlib); its place here assumed
#define DICTIONARY_H

#include <string.h>

template <class K, class V>
class DNode  // NAME 4: wlib's name
{
public:
	K key;					// NAME 4: wlib's name
	V value;				// NAME 4: wlib's name
	DNode<K, V> *hashNext;	// NAME 4: wlib's name
};

template <class K, class V>
class Dictionary  // NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
{
public:
	Dictionary(unsigned int (*hashFn)(const K &key)) :
		SHRINK_THRESHOLD(0.20), EXPAND_THRESHOLD(0.80), MIN_TABLE_SIZE(128)
	{
		log2Size = MIN_TABLE_SIZE;
		size = MIN_TABLE_SIZE;
		tableBits = 0;
		while (log2Size) { tableBits++; log2Size >>= 1; }
		tableBits--;
		size = 1 << tableBits;
		entries = 0;
		keepSize = false;
		table = (DNode<K, V> **)new DNode<K, V> *[size];
		memset(table, 0, size * sizeof(void *));
		hashFunc = hashFn;
	}
	~Dictionary() { clear(); delete [] table; }

	void clear()
	{
		for (unsigned int i = 0; i < size; i++) {
			DNode<K, V> *temp = table[i];
			while (temp) {
				DNode<K, V> *del = temp;
				temp = temp->hashNext;
				delete del;
			}
			table[i] = 0;
		}
		entries = 0;
		while (size > (unsigned int)MIN_TABLE_SIZE && !keepSize) {
			resize(size / 2);
		}
	}
	bool add(const K &key, const V &value)
	{
		DNode<K, V> *node = new DNode<K, V>;
		node->key = key;
		node->value = value;
		node->hashNext = 0;
		remove(key);
		unsigned int h = keyHash(key);
		node->hashNext = table[h];
		table[h] = node;
		entries++;
		if (!keepSize && (double)entries / (double)size >= EXPAND_THRESHOLD) {
			resize(size * 2);
		}
		return(true);
	}
	bool getPointer(const K &key, V **value) const
	{
		for (DNode<K, V> *n = table[keyHash(key)]; n; n = n->hashNext) {
			if (n->key == key) {
				*value = &n->value;
				return(true);
			}
		}
		return(false);
	}
	bool remove(const K &key)
	{
		unsigned int h = keyHash(key);
		DNode<K, V> **link = &table[h];
		while (*link) {
			if ((*link)->key == key) {
				DNode<K, V> *del = *link;
				*link = del->hashNext;
				delete del;
				entries--;
				if (!keepSize && size > (unsigned int)MIN_TABLE_SIZE && (double)entries / (double)size <= SHRINK_THRESHOLD) {
					resize(size / 2);
				}
				return(true);
			}
			link = &(*link)->hashNext;
		}
		return(false);
	}
	unsigned int getEntries() const { return(entries); }

	DNode<K, V> **table;			// +0x00 NAME 4: wlib's names, here and below
	unsigned int entries;			// +0x04
	unsigned int size;				// +0x08
	unsigned int tableBits;			// +0x0C
	unsigned int log2Size;			// +0x10
	bool keepSize;					// +0x14
	unsigned int (*hashFunc)(const K &key);	// +0x18
	const double SHRINK_THRESHOLD;	// +0x20
	const double EXPAND_THRESHOLD;	// +0x28
	const int MIN_TABLE_SIZE;		// +0x30

private:
	unsigned int keyHash(const K &key) const { return(hashFunc(key) & ((1 << tableBits) - 1)); }
	void resize(unsigned int new_size)
	{
		DNode<K, V> **old = table;
		unsigned int old_size = size;
		size = new_size;
		tableBits = 0;
		for (unsigned int s = size; s > 1; s >>= 1) {
			tableBits++;
		}
		table = (DNode<K, V> **)new DNode<K, V> *[size];
		memset(table, 0, size * sizeof(void *));
		for (unsigned int i = 0; i < old_size; i++) {
			DNode<K, V> *n = old[i];
			while (n) {
				DNode<K, V> *next = n->hashNext;
				unsigned int h = keyHash(n->key);
				n->hashNext = table[h];
				table[h] = n;
				n = next;
			}
		}
		delete [] old;
	}
};

#endif
