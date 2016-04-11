#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Game/GameUtility.h>

static const unsigned char pattern[] = { 0x50, 0x30, 0x20, 0x22, 0x11, 0x67, 0x87, 0x33, 0x99, 0xFF, 0x65, 0x11, 0x00, 0x00, 0x00, 0x00, 0x44, 0x11, 0x22, 0x44, 0x92, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x29 };

class PatternScannerTest : public ::testing::Test
{
protected:

	PatternScannerTest()
	{ }

	~PatternScannerTest()
	{ }
};

TEST_F(PatternScannerTest, test_pattern_scan_few_bytes_begin)
{
	intptr_t result = GameUtility::FindPattern(reinterpret_cast<const char*>(pattern), "xxxxx");
	ASSERT_EQ(result, reinterpret_cast<intptr_t>(&pattern[0]));
}

TEST_F(PatternScannerTest, test_pattern_scan_all_bytes_begin)
{
	intptr_t result = GameUtility::FindPattern(
		reinterpret_cast<const char*>(pattern), "xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	ASSERT_EQ(result, reinterpret_cast<intptr_t>(&pattern[0]));
}

TEST_F(PatternScannerTest, test_pattern_scan_few_bytes_mid)
{
	intptr_t result = GameUtility::FindPattern(reinterpret_cast<const char*>(&pattern[4]), "xxxx");
	ASSERT_EQ(result, reinterpret_cast<intptr_t>(&pattern[4]));
}

TEST_F(PatternScannerTest, test_pattern_scan_few_bytes_wildcard)
{	
	intptr_t result = GameUtility::FindPattern(reinterpret_cast<const char*>(&pattern[5]), "x?xx?x??xxx");
	ASSERT_EQ(result, reinterpret_cast<intptr_t>(&pattern[5]));
}
