#include <gtest/gtest.h>
#include <iostream>
#include <functional>
#include <algorithm>
#include "BloomFilter.hpp"

size_t hashA(const std::string& key)
{
	return 1;
}

class BLOOM_FILTER : public ::testing::Test
{
public:
	void SetUp()
	{
		filter = new BloomFilter<std::string>( 12, 4, hashA, hashA);
		std::string key("key");
		filter->add(key);
	}
	void TearDown()
	{
		delete filter;
	}

	BloomFilter<std::string>* filter;
};

TEST_F(BLOOM_FILTER, STATES)
{
	std::string key("key");
// check presence	
	EXPECT_EQ(filter->checkPresence(key), true);

// check remove
	filter->remove(key);
	EXPECT_EQ(filter->checkPresence(key), false);
}

TEST_F(BLOOM_FILTER, ITERATOR)
{
	EXPECT_EQ(true, filter->begin() != filter->end());
	EXPECT_EQ(false, filter->begin() == filter->end());

	std::for_each(std::begin(*filter), std::end(*filter), 
				  [](std::vector<bool>& v)
				  {
				  	for (size_t i = 0; i < v.size(); i++)
					{
						if (v[i] == true)
							std::cerr << "1";
						else
						{
							std::cerr << "0";
						}
					}

					std::cerr << std::endl;
				  });
}

int main( int argc, char** argv) {
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}