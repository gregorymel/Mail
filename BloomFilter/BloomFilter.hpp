#pragma once

#include <functional>
#include <limits>
#include <string>
#include <vector>
#include <iostream>

template <typename Key>
class BloomFilter;


/*****************ITERATOR DEFENITION*************************/
template <typename ValueType>
class BloomFilterIterator : public std::iterator<std::input_iterator_tag, ValueType>
{
	template <typename Key>
	friend class BloomFilter;
private:
	std::vector<bool> mask;

	BloomFilterIterator(std::vector<bool>&& moved) : mask(moved) {}
public:
	BloomFilterIterator() {};

	//typename BloomFilterIterator::reference operator*() const;
	std::vector<bool>& operator*();
	BloomFilterIterator& operator++();
	bool operator!=(BloomFilterIterator const& other) const;
	bool operator==(BloomFilterIterator const& other) const; 
};


template <typename ValueType>
bool BloomFilterIterator<ValueType>::operator==(BloomFilterIterator const& other) const
{
	if (mask.size() == other.mask.size())
	{
		for (size_t pos = 0; pos < mask.size(); pos++) 
			if (mask[pos] != other.mask[pos])
				return false;
	} else
		return false;

	return true;
}

template <typename ValueType>
bool BloomFilterIterator<ValueType>::operator!=(BloomFilterIterator const& other) const
{
	if (mask.size() == other.mask.size())
	{
		for (size_t pos = 0; pos < mask.size(); pos++) 
			if (mask[pos] != other.mask[pos])
				return true;
	} else
		return true;

	return false;
}


template <typename ValueType>
BloomFilterIterator<ValueType>& BloomFilterIterator<ValueType>::operator++()
{
	for (size_t i = 0; i < mask.size(); i++)
	{
		if (mask[i] == true)
			mask[i] = false;
		else
		{
			mask[i] = true;
			break;
		}
	}

	return *this;
}

template <typename ValueType>
std::vector<bool>& BloomFilterIterator<ValueType>::operator*()
{
	#if 0
	for (size_t i = 0; i < mask.size(); i++)
	{
		if (mask[i] == true)
			std::cerr << "1";
		else
		{
			std::cerr << "0";
		}
	}

	std::cerr << std::endl;
	#endif
	
	return mask;
}

/********************************************************************************/

template <typename Key>
class BloomFilter
{
	typedef std::function<size_t(const Key&)> Hash;
	#define MAX_FLAG_NUM std::numeric_limits<unsigned int>::max()

	std::vector<unsigned int> m_flags;
	const size_t m_size;
	const uint8_t m_hashs;

	const Hash hashA;
	const Hash hashB;

	size_t hashI(const Key& _key, uint8_t i)
	{
		return ( hashA( _key) + i * hashB( _key)) % m_size;
	}

public: 
	BloomFilter( const BloomFilter<Key>& other);

	template <typename T>
	friend void swap( BloomFilter<T>& first, BloomFilter<T>& second);

	BloomFilter<Key>& operator=( BloomFilter<Key> copied);

	BloomFilter( size_t m_size, uint8_t m_hashs, Hash _hashA, Hash _hashB);

	void add( const Key _key);
	void remove( const Key _key);
	bool checkPresence( const Key _key) ;

// itearator
	typedef BloomFilterIterator<bool> iterator;
	typedef BloomFilterIterator<const bool> const_iterator;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	#ifdef DEBUG
	inline void DEBUG_PRINT()
	{
		std::cerr << "KEY: ";
		for (size_t i = 0; i < m_size; i++)
		{
			if (m_flags[i] > 0)
				std::cerr << "1";
			else
			{
				std::cerr << "0";
			}
		}
		std::cerr << std::endl;
	}
	#endif

};

template <typename Key>
BloomFilter<Key>::BloomFilter( size_t m_size, uint8_t m_hashs, 
						       Hash _hashA, Hash _hashB) : m_size( m_size),
								  	 					   m_hashs( m_hashs),
													       hashA( _hashA),
													       hashB( _hashB)
{
	m_flags.resize( m_size);
}

template <typename Key>
void BloomFilter<Key>::add( const Key _key)
{
	for ( uint8_t i = 0; i < m_hashs; i++)
	{
		if ( m_flags[ hashI( _key, i)] < MAX_FLAG_NUM)
			m_flags[ hashI( _key, i)]++;
	}
}

template <typename Key>
void BloomFilter<Key>::remove( const Key _key)
{
	for ( uint8_t i = 0; i < m_hashs; i++)
	{
		if ( m_flags[ hashI( _key, i)] > 0)
			m_flags[ hashI( _key, i)]--;
	}
}

template <typename Key>
bool BloomFilter<Key>::checkPresence( const Key _key)
{
	for ( uint8_t i = 0; i < m_hashs; i++)
	{
		if ( m_flags[ hashI( _key, i)] == 0)
			return false;
	}

	return true;
}

template <typename Key>
BloomFilter<Key>::BloomFilter( const BloomFilter<Key>& other) 
							   : m_size( other.m_size),
								 m_hashs( other.m_hashs),
								 hashA( other.hashA),
								 hashB( other.hashB)
{
	std::copy( m_flags.begin(), m_flags.end(), other.m_flags);
}

template <typename Key>
void swap( BloomFilter<Key>& first, BloomFilter<Key>& second)
{
	using std::swap;
	swap( first.m_size, second.m_size);
	swap( first.m_hashs, second.m_hashs);
	swap( first.hashA, second.hashA);
	swap( first.hashB, second.hashB);
	swap( first.m_flags, second.m_flags);
}

template <typename Key>
BloomFilter<Key>& BloomFilter<Key>::operator=( BloomFilter<Key> copied)
{
	swap( *this, copied);
	return *this;
} 

/**********************ITERATOR***********************/
template <typename Key>
typename BloomFilter<Key>::iterator BloomFilter<Key>::begin()
{
	std::vector<bool> mask;

	// number of filled cells
	unsigned int cells = 0;

	for (int i = 0; i < m_size; i++)
	{
		if (m_flags[i] > 0)
			cells++;
	}

	std::cerr << cells << std::endl;

	if (cells > 0)
	{
		mask.resize(cells);
		mask[0] = true;
	}

	return iterator(std::move(mask));
	//return itearator(std::move(mask));
}

template <typename Key>
typename BloomFilter<Key>::iterator BloomFilter<Key>::end()
{
	std::vector<bool> mask;

	// number of filled cells
	unsigned int cells = 0;

	for (int i = 0; i < m_size; i++)
	{
		if (m_flags[i] > 0)
			cells++;
	}

	if (cells >= m_hashs)
	{
		mask.resize(cells);
		for (uint8_t i = 0; i < m_hashs; i++)
		{
			cells--;
			mask[cells] = true; 
		}
	}

	//return iterator(mask);
	return iterator(std::move(mask));
}

template <typename Key>
typename BloomFilter<Key>::const_iterator BloomFilter<Key>::begin() const
{
	std::vector<bool> mask;

	// number of filled cells
	unsigned int cells = 0;

	for (int i = 0; i < m_size; i++)
	{
		if (m_flags[i] > 0)
			cells++;
	}

	if (cells > 0)
	{
		mask.resize(cells);
		mask[0] = true;
	}

	return const_iterator(std::move(mask));
}	

template <typename Key>
typename BloomFilter<Key>::const_iterator BloomFilter<Key>::end() const
{
	std::vector<bool> mask;

	// number of filled cells
	unsigned int cells = 0;

	for (int i = 0; i < m_size; i++)
	{
		if (m_flags[i] > 0)
			cells++;
	}

	if (cells >= m_hashs)
	{
		mask.resize(cells);
		for (uint8_t i = 0; i < m_hashs; i++)
		{
			cells--;
			mask[cells] = true; 
		}
	}

	return const_iterator(std::move(mask));
}
