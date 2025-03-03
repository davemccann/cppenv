#include "cppenv.h"

#include <gtest/gtest.h>
#include <string>
#include <system_error>
#include <type_traits>
#include <variant>

using TestType = std::variant<int, float, double, std::string>;
struct TestParameter
{
	std::string varName;
	TestType expectedValue;
	TestType defaultValue;
};

static std::vector<TestParameter> testCases = {
	{"SOME_INTEGER",       123,                  0   },
	{"SOME_FLOAT",         123.123f,             0.0f},
	{"SOME_DOUBLE",        123.123456789,        0.0 },
	{"SOME_STRING",        "some_string",        ""  },
	{"SOME_SPACED_STRING", "string with spaces", ""  },
};

template <typename T>
inline void expectEQ(T lhs, T rhs)
{
	if constexpr (std::is_same_v<T, float>)
	{
		EXPECT_FLOAT_EQ(lhs, rhs);
	}
	else if constexpr (std::is_same_v<T, double>)
	{
		EXPECT_DOUBLE_EQ(lhs, rhs);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		EXPECT_STREQ(lhs.c_str(), rhs.c_str());
	}
	else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>)
	{
		EXPECT_STREQ(lhs, rhs);
	}
	else
	{
		EXPECT_EQ(lhs, rhs);
	}
}

class OSUnitTests : public ::testing::Test
{
  protected:
	void SetUp() override
	{
		std::error_code result = cppenv::loadEnvFile(".env");
		EXPECT_EQ(result.value(), static_cast<int>(cppenv::Error::None)) << result.message();
	}

	void TearDown() override
	{
		for (const auto& test : testCases)
		{
			std::error_code err = cppenv::unsetEnv(test.varName);
			EXPECT_EQ(err.value(), static_cast<int>(cppenv::Error::None));
		}
	}
};

TEST_F(OSUnitTests, SetEnvValidParam)
{
	std::error_code result = cppenv::setEnv("SOME_INTEGER", "123");
	EXPECT_EQ(result.value(), static_cast<int>(cppenv::Error::None)) << result.message();
	int iResult = cppenv::getEnv("SOME_INTEGER", 0);
	EXPECT_EQ(iResult, 123);
}

TEST_F(OSUnitTests, GetEnvInt)
{
	auto testCase = testCases[0];
	int result    = cppenv::getEnv(testCase.varName, std::get<int>(testCase.defaultValue));
	EXPECT_EQ(result, std::get<int>(testCase.expectedValue));
}

TEST_F(OSUnitTests, GetenvFloat)
{
	auto testCase = testCases[1];
	float result  = cppenv::getEnv(testCase.varName, std::get<float>(testCase.defaultValue));
	EXPECT_FLOAT_EQ(result, std::get<float>(testCase.expectedValue));
}

TEST_F(OSUnitTests, GetEnvDouble)
{
	auto testCase = testCases[2];
	double result = cppenv::getEnv(testCase.varName, std::get<double>(testCase.defaultValue));
	EXPECT_DOUBLE_EQ(result, std::get<double>(testCase.expectedValue));
}

TEST_F(OSUnitTests, GetEnvString)
{
	auto testCase      = testCases[3];
	std::string result = cppenv::getEnv(testCase.varName, std::get<std::string>(testCase.defaultValue));
	EXPECT_STREQ(result.c_str(), std::get<std::string>(testCase.expectedValue).c_str());
}

TEST_F(OSUnitTests, GetEnvCString)
{
	auto testCase               = testCases[3];
	std::string_view defaultVal = "";
	std::string result          = cppenv::getEnv(testCase.varName, defaultVal);
	EXPECT_STREQ(result.c_str(), std::get<std::string>(testCase.expectedValue).c_str());
}

TEST_F(OSUnitTests, GetenvDefault)
{
	int result = cppenv::getEnv<int>("INVALID_ENV", 0);
	EXPECT_EQ(result, 0);
}

TEST_F(OSUnitTests, LoadEnvFile)
{
	std::error_code result = cppenv::loadEnvFile(".env");
	EXPECT_EQ(result.value(), static_cast<int>(cppenv::Error::None)) << result.message();

	for (const auto& test : testCases)
	{
		std::visit(
			[&](auto&& expectedValue, auto&& defaultValue)
			{
				using Type       = std::decay_t<decltype(expectedValue)>;
				auto defaultVal  = std::get<Type>(test.defaultValue);
				auto expectedVal = std::get<Type>(test.expectedValue);

				auto testResult = cppenv::getEnv(test.varName, defaultVal);
				expectEQ(testResult, expectedVal);
			},
			test.expectedValue, test.defaultValue);
	}
}

TEST_F(OSUnitTests, LoadEnvFileParseError)
{
	std::error_code result = cppenv::loadEnvFile("bad.env");
	EXPECT_EQ(result.value(), static_cast<int>(cppenv::Error::ParsingEnvFileFailed)) << result.message();
}

TEST_F(OSUnitTests, UnsetEnv)
{
	auto testCase = testCases[0];

	std::error_code err = cppenv::unsetEnv(testCase.varName);
	EXPECT_EQ(err.value(), static_cast<int>(cppenv::Error::None)) << err.message();

	int result = cppenv::getEnv(testCase.varName, std::get<int>(testCase.defaultValue));
	EXPECT_EQ(result, std::get<int>(testCase.defaultValue));
}

TEST_F(OSUnitTests, UnsetEnvInvalidVar)
{
	std::error_code result = cppenv::unsetEnv("INVALID");
	EXPECT_EQ(result.value(), static_cast<int>(cppenv::Error::None)) << result.message();
}
