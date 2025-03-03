#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>

namespace cppenv
{
	enum class Error
	{
		None                   = 0,
		InvalidArguments       = 1,
		SetEnvironmentFailed   = 2,
		UnsetEnvironmentFailed = 3,
		ParsingEnvFileFailed   = 4,
	};

	class ErrorCategory : public std::error_category
	{
	  public:
		ErrorCategory() {}

		const char* name() const noexcept override
		{
			return "InvalidArguments";
		}

		std::string message(int err) const override
		{
			switch (static_cast<Error>(err))
			{
				case Error::InvalidArguments:
					return "one or more arguments are invalid";
				case Error::SetEnvironmentFailed:
					return "failed to set the environment variable";
				case Error::UnsetEnvironmentFailed:
					return "failed to unset the environment variable";
				default:
					return "unknown error";
			}
		}
	};

	inline std::error_code makeErrorCode(Error err)
	{
		return {static_cast<int>(err), ErrorCategory()};
	}

	// @brief Sets an environment variables value using the name as the environment variable key
	// @param name The name of the environment variable
	// @param value The value to assign to the environment variable
	// @return Returns true on success
	std::error_code setEnv(std::string_view name, std::string_view value);

	// @brief Clears/Removes a single environment variable
	// @param name The name of the environment variable to unset
	// @return Returns true on successful deletion
	std::error_code unsetEnv(std::string_view name);

	// @brief Reads a file at the given path and sets all the valid environment variables contained in it
	// @param path The filepath to the file containing environment variables
	// @return Return true on successful processing of environment file
	std::error_code loadEnvFile(const std::filesystem::path& path);

	// @brief Returns the environment variable value using the platform implementation
	// @param name The name of the environment variable to retrieve
	// @return Returns a string of the environment variable or an empty string on failure
	std::string platform_getEnv(std::string_view name);

	// @brief Sets an environment variable with value using the platform implementation
	// @param name The name of the environment variable to set
	// @param value The string representation of the value to set
	// @return Returns true after successfully setting the environment variable
	std::error_code platform_setEnv(std::string_view name, std::string_view value);

	// @brief Unsets/Clears an environment variable
	// @param name The name of the environment variable to unset
	// @return Returns true on successful deletion
	std::error_code platform_unsetEnv(std::string_view name);

	// @brief Retrieves the environment variable for a given name
	// @param name The name of the environment variable to fetch
	// @param defaultValue The default value to return if the variable does not exist or fails
	// @return Returns the value from the environment variable or the default value
	template <typename Type>
	Type getEnv(const std::string& name, Type defaultValue)
	{
		constexpr bool TYPE_VALID = std::is_same_v<Type, std::string>;
		static_assert(TYPE_VALID, "getenv(): unsupported type");

		const std::string value = platform_getEnv(name.c_str());
		if (value.empty())
		{
			return defaultValue;
		}

		return value;
	}

	// @brief retrieves the environment variable for a given name
	// @param name the name of the environment variable to fetch
	// @param defaultvalue the default value to return if the variable does not exist or fails
	// @return returns a string containing the values for the environment variable or the default value
	inline std::string getEnv(const std::string& name, std::string& defaultValue)
	{
		const std::string result = platform_getEnv(name);
		return result.empty() ? defaultValue : result;
	}

	// @brief retrieves the environment variable for a given name
	// @param name the name of the environment variable to fetch
	// @param defaultvalue the default value to return if the variable does not exist or fails
	// @return returns a string containing the values for the environment variable or the default value
	inline std::string getEnv(const std::string& name, std::string_view defaultValue)
	{
		// TODO: Check this is passed in correctly
		std::string result = platform_getEnv(name);
		return result.empty() ? std::string(defaultValue) : result;
	}

	// @brief retrieves the environment variable for a given name
	// @param name the name of the environment variable to fetch
	// @param defaultvalue the default value to return if the variable does not exist or fails
	// @return returns a string containing the values for the environment variable or the default value
	inline std::string getEnv(const std::string& name, const char* defaultValue)
	{
		std::string result = platform_getEnv(name);
		return result.empty() ? defaultValue : result;
	}

	// @brief Retrieves the environment variable for a given name
	// @param name The name of the environment variable to fetch
	// @param defaultValue The default value to return if the variable does not exist or fails
	// @return Returns the value from the environment variable or the default value
	template <>
	inline int getEnv<int>(const std::string& name, int defaultValue)
	{
		const std::string value = platform_getEnv(name.c_str());
		if (value.empty())
		{
			return defaultValue;
		}

		char* end{};
		int result = std::strtoul(value.c_str(), &end, 0);
		if (result < 0)
		{
			return defaultValue;
		}

		return result;
	}

	// @brief Retrieves the environment variable for a given name
	// @param name The name of the environment variable to fetch
	// @param defaultValue The default value
	// @return A float containing the value of the environment variable or the default value
	template <>
	inline float getEnv<float>(const std::string& name, float defaultValue)
	{
		const std::string value = platform_getEnv(name.c_str());

		if (value.empty())
		{
			return defaultValue;
		}

		char* endPtr{};
		float result = std::strtof(value.c_str(), &endPtr);

		if (*endPtr != '\0')
		{
			return defaultValue;
		}

		return result;
	}

	// @brief Retrieves the environment variable for a given name
	// @param name The name of the environment variable to fetch
	// @param defaultValue The default value
	// @return A double containing the value of the environment variable or the default value
	template <>
	inline double getEnv<double>(const std::string& name, double defaultValue)
	{
		const std::string value = platform_getEnv(name.c_str());

		if (value.empty())
		{
			return defaultValue;
		}

		char* endPtr{};
		double result = std::strtod(value.c_str(), &endPtr);

		if (*endPtr != '\0')
		{
			return defaultValue;
		}

		return result;
	}
} // namespace cppenv
