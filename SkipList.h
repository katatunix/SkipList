#ifndef _SKIP_LIST_H_
#define _SKIP_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

template<typename K>
class IKeyComparer
{
public:
	virtual int compare(const K& key1, const K& key2) const = 0;
};

template<typename K, typename D>
class SkipList
{
private:

	typedef IKeyComparer<K> KeyComparer;

	struct Node
	{
	public:
		K m_key;
		D m_data;
		Node** m_nextList;
		int m_nextCount;

		Node(int levelCount) // make sure levelCount > 0
		{
			m_nextCount = levelCount;
			m_nextList = new Node*[m_nextCount];
			for (int i = 0; i < m_nextCount; i++) m_nextList[i] = NULL;
		}
	};

	enum
	{
		MAX_LEVEL_COUNT = 32,
		DEFAULT_GEN_PERCENT = 50
	};

public:

	SkipList(const KeyComparer& keyComparer) :
		m_keyComparer(keyComparer),
		m_kMaxLevelCount(MAX_LEVEL_COUNT),
		m_kGenPercent(DEFAULT_GEN_PERCENT)
	{
		init();
	}
	
	SkipList(const KeyComparer& keyComparer, int maxLevelCount, int genPercent) :
		m_keyComparer(keyComparer),
		m_kMaxLevelCount(maxLevelCount),
		m_kGenPercent(genPercent)
	{
		init();
	}

	virtual ~SkipList()
	{
		if (m_kMaxLevelCount <= 0) return;

		Node* node = m_head;
		while (node)
		{
			Node* next = node->m_nextList[0];
			delete node;
			node = next;
		}

		delete [] m_updates;
	}

	void insert(const K& key, const D& data)
	{
		if (m_kMaxLevelCount <= 0) return;

		Node* node = m_head;
		for (int i = m_curLevelCount - 1; i >= 0; i--)
		{
			Node* nodeNext;
			int comp = -1;
			while (true)
			{
				nodeNext = node->m_nextList[i];
				if (nodeNext == m_tail) break;
				comp = m_keyComparer.compare(nodeNext->m_key, key);
				if (comp >= 0) break;
				node = nodeNext;
			}
			if (comp == 0)
			{
				nodeNext->m_data = data;
				return;
			}
			m_updates[i] = node;
		}

		int newLevelCount = genLevelCount();
		if (newLevelCount > m_curLevelCount)
		{
			for (int i = newLevelCount - 1; i >= m_curLevelCount; i--)
			{
				m_updates[i] = m_head;
			}
			m_curLevelCount = newLevelCount;
		}

		node = new Node(newLevelCount);
		node->m_key = key;
		node->m_data = data;

		for (int i = newLevelCount - 1; i >= 0; i--)
		{
			node->m_nextList[i] = m_updates[i]->m_nextList[i];
			m_updates[i]->m_nextList[i] = node;
		}
	}

	void printAll()
	{
		int count = 0;
		Node* node = m_head->m_nextList[0];
		int maxLevelCount = 0;
		while (node != m_tail)
		{
			printf("[%d]\t%d / %d.\t%d\n", count, node->m_key, node->m_data, node->m_nextCount);
			count++;

			node = node->m_nextList[0];
		}
		printf("maxLevelCount = %d\n", maxLevelCount);
	}

	void printAll(const char* filePath)
	{
		FILE* f = fopen(filePath, "wt");
		int count = 0;
		Node* node = m_head->m_nextList[0];
		int maxLevelCount = 0;
		while (node != m_tail)
		{
			fprintf(f, "[%d]\t%d / %d.\t%d\n", count, node->m_key, node->m_data, node->m_nextCount);
			count++;

			if (maxLevelCount < node->m_nextCount)
				maxLevelCount = node->m_nextCount;

			node = node->m_nextList[0];
		}
		fprintf(f, "maxLevelCount = %d\n", maxLevelCount);
		fclose(f);
	}

private:

	void init()
	{
		if (m_kMaxLevelCount <= 0) return;
		
		m_head = new Node(m_kMaxLevelCount);
		m_tail = new Node(m_kMaxLevelCount);

		for (int i = 0; i < m_head->m_nextCount; i++)
		{
			m_head->m_nextList[i] = m_tail;
		}

		m_updates = new Node*[m_kMaxLevelCount];

		m_curLevelCount = 0;

		srand((unsigned int)time(NULL));
	}

	int genLevelCount()
	{
		int count = 1;
		while (rand() % 100 + 1 <= m_kGenPercent && count < m_kMaxLevelCount) count++;
		return count;
	}

	const int m_kMaxLevelCount;
	const int m_kGenPercent;

	int m_curLevelCount;
	
	Node* m_head;
	Node* m_tail;

	Node** m_updates; // helper

	const KeyComparer& m_keyComparer;
};

#endif
