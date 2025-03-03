#include "cppenv.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <string.h>
#include <string>
#include <string_view>
#include <system_error>

namespace cppenv
{
	static std::tuple<std::string_view, std::string_view> tokenizeStr(std::string_view sv, const char* delim)
	{
		// Token
		size_t posStart = 0;
		size_t posEnd   = sv.find(delim, posStart);

		if (posEnd == std::string_view::npos)
		{
			return {"", ""};
		}

		std::string_view token = sv.substr(posStart, posEnd);

		// Value
		posStart               = posEnd + strlen(delim);
		std::string_view value = (posStart < sv.length()) ? sv.substr(posStart) : "";

		return {token, value};
	}

	std::error_code setEnv(std::string_view name, std::string_view value)
	{
		return platform_setEnv(name, value);
	}

	std::error_code unsetEnv(std::string_view name)
	{
		return platform_unsetEnv(name);
	}

	std::error_code loadEnvFile(const std::filesystem::path& path)
	{
		std::ifstream ifs;
		ifs.open(path, std::ifstream::in);

		if (ifs.good())
		{
			for (std::string line; std::getline(ifs, line);)
			{
				const auto [token, value] = tokenizeStr(line, "=");
				std::error_code result    = setEnv(token, value);
				if (result.value() != static_cast<int>(Error::None))
				{
					return result;
				}
			}
		}
		else
		{
			return makeErrorCode(Error::ParsingEnvFileFailed);
		}

		return std::error_code();
	}
} // namespace cppenv
