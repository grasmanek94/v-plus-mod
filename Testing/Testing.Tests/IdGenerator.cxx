#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IdCounter.hxx>
#include <ArrayUtils.hxx>

class IdGeneratorTest : public ::testing::Test
{
protected:
	IdCounter counter;
	const static size_t max_values = 5;

	IdGeneratorTest()
		: counter(max_values)
	{
		
	}

	~IdGeneratorTest()
	{
		
	}
};

TEST_F(IdGeneratorTest, test_max)
{
	ASSERT_EQ(counter.Max(), max_values);
}

TEST_F(IdGeneratorTest, test_get_id_works)
{
	ASSERT_NE(counter.GetId(), -1);
}

TEST_F(IdGeneratorTest, test_get_id_incremental)
{
	ASSERT_EQ(counter.GetId(), 0);
	ASSERT_EQ(counter.GetId(), 1);
	ASSERT_EQ(counter.GetId(), 2);
	ASSERT_EQ(counter.GetId(), 3);
	ASSERT_EQ(counter.GetId(), 4);
}

TEST_F(IdGeneratorTest, test_get_id_run_out_of_ids)
{
	for (size_t i = 0; i < max_values; ++i)
	{
		ASSERT_EQ(counter.GetId(), i);		
	}
	ASSERT_EQ(counter.GetId(), (size_t)-1);
}

TEST_F(IdGeneratorTest, test_free_id_works)
{
	for (size_t i = 0; i < max_values; ++i)
	{
		ASSERT_EQ(counter.FreeId(i), false);
	}
	
	size_t id = counter.GetId();

	ASSERT_NE(id, -1);

	for (size_t i = 0; i < max_values; ++i)
	{
		ASSERT_EQ(counter.FreeId(i), id == i);
	}
}

TEST_F(IdGeneratorTest, test_free_id_freed_ids_available_for_get)
{
	for (size_t i = 0; i < max_values; ++i)
	{
		counter.GetId();
	}

	counter.FreeId(1);
	counter.FreeId(2);

	ASSERT_TRUE(testing::IsInArray(counter.GetId(), { 1, 2 }));
	ASSERT_TRUE(testing::IsInArray(counter.GetId(), { 1, 2 }));
	ASSERT_EQ(counter.GetId(), -1);
}

TEST_F(IdGeneratorTest, test_free_id_freed_ids_never_same_twice)
{
	for (size_t i = 0; i < max_values; ++i)
	{
		counter.GetId();
	}

	for (size_t i = 0; i < max_values; ++i)
	{
		counter.FreeId((max_values - 1) - i);
	}

	std::vector<size_t> id_list;
	for (size_t i = 0; i < max_values; ++i)
	{
		id_list.push_back(counter.GetId());
	}

	ASSERT_TRUE(testing::IsArrayUnique(id_list));

	ASSERT_EQ(counter.GetId(), -1);
}

TEST_F(IdGeneratorTest, test_get_id_never_same_twice)
{
	std::vector<size_t> id_list;
	for (size_t i = 0; i < max_values; ++i)
	{
		id_list.push_back(counter.GetId());
	}

	ASSERT_TRUE(testing::IsArrayUnique(id_list));

	ASSERT_EQ(counter.GetId(), -1);
}
