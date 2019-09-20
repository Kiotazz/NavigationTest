#pragma once

#include <vector>

template<class T>
class MinHeap
{
private:
	std::vector<T> m_stHeap;

public:
	MinHeap() {}

	MinHeap(unsigned int cap)
	{
		m_stHeap.reserve(cap);
	}

	MinHeap(const std::vector<T>& heap)
	{
		m_stHeap = heap;
		Heapify();
	}

	virtual ~MinHeap() {}

	void ConstructHeap(const std::vector<T>& data)
	{
		m_stHeap = data;
		Heapify();
	}

	void Heapify()
	{
		for (unsigned int p = m_stHeap.size() / 2 + 1; p > 0;)
		{
			heapifyTopDown(--p);
		}
	}

	T Top()
	{
		return m_stHeap.front();
	}

	T Extract()
	{
		T ret = std::move(m_stHeap.front());
		m_stHeap[0] = std::move(m_stHeap.back());
		m_stHeap.pop_back();
		heapifyTopDown(0);
		return ret;
	}

	void Insert(const T& el)
	{
		Emplace(el);
	}

	void Insert(T&& el)
	{
		Emplace(std::move(el));
	}

	template<class Valt>
	void Emplace(Valt&& el)
	{
		m_stHeap.emplace_back(std::forward<Valt>(el));
		heapifyDownTop(m_stHeap.size() - 1);
	}

	bool empty()
	{
		return m_stHeap.empty();
	}

	inline void clear()
	{
		m_stHeap.clear();
	}

	unsigned int size()
	{
		return m_stHeap.size();
	}

	inline unsigned int Capacity()
	{
		return m_stHeap.capacity();
	}

private:
	void heapifyTopDown(unsigned int p)
	{
		unsigned int size = m_stHeap.size();
		unsigned int lc, rc, c;
		while (true)
		{
			lc = leftChild(p);
			rc = rightChild(p);
			c = p;
			if (lc < size && m_stHeap[lc] < m_stHeap[c])
			{
				c = lc;
			}
			if (rc < size && m_stHeap[rc] < m_stHeap[c])
			{
				c = rc;
			}
			if (c == p)
			{
				return;
			}
			swap(p, c);
			p = c;
		}
	}

	void heapifyDownTop(unsigned int c)
	{
		unsigned int p;
		while (c > 0)
		{
			p = parent(c);
			if (m_stHeap[c] >= m_stHeap[p])
			{
				return;
			}
			swap(c, p);
			c = p;
		}
	}

	inline bool hasParent(unsigned int c)
	{
		return c > 0;
	}

	inline unsigned int parent(unsigned int c)
	{
		return (c - 1) / 2;
	}

	inline unsigned int leftChild(unsigned int p)
	{
		return p * 2 + 1;
	}

	inline unsigned int rightChild(unsigned int p)
	{
		return p * 2 + 2;
	}

	inline void swap(unsigned int a, unsigned int b)
	{
		std::swap(m_stHeap[a], m_stHeap[b]);
	}
};