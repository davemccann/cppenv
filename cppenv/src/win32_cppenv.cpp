#include "cppenv.h"

#include <cstdio>
#include <cwchar>
#include <string>
#include <string_view>
#include <system_error>
#include <wchar.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace cppenv
{
	static std::string wideToUTF8Str(const std::wstring_view str)
	{
		if (str.empty())
		{
			return "";
		}

		const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded <= 0)
		{
			return "";
		}

		std::string resultStr = std::string(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), resultStr.data(), sizeNeeded, nullptr, nullptr);

		return resultStr;
	}

	static std::wstring utf8ToWideStr(const std::string_view str)
	{
		if (str.empty())
		{
			return L"";
		}

		const int charsWritten = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
		if (charsWritten <= 0)
		{
			return L"";
		}

		std::wstring resultStr = std::wstring(charsWritten, '\0');
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), resultStr.data(), charsWritten);

		return resultStr;
	}

#if defined(UNICODE) || defined(_UNICODE)
	std::string platform_getEnv(std::string_view name)
	{
		const std::wstring variableName = utf8ToWideStr(name);

		DWORD sizeNeeded = GetEnvironmentVariable(variableName.c_str(), nullptr, 0);
		if (sizeNeeded <= 0)
		{
			return "";
		}

		std::wstring buffer(sizeNeeded, '\0');

		DWORD result = GetEnvironmentVariable(variableName.c_str(), buffer.data(), sizeNeeded);
		if (result == 0)
		{
			return "";
		}

		return wideToUTF8Str(buffer);
	}

	std::error_code platform_setEnv(std::string_view name, std::string_view value)
	{
		if (name.empty() || value.empty())
		{
			return makeErrorCode(Error::InvalidArguments);
		}

		const std::wstring variableName  = utf8ToWideStr(name);
		const std::wstring variableValue = utf8ToWideStr(value);

		BOOL result = SetEnvironmentVariable(variableName.c_str(), variableValue.c_str());
		if (!result)
		{
			return makeErrorCode(Error::SetEnvironmentFailed);
		}

		return std::error_code();
	}

	std::error_code platform_unsetEnv(std::string_view name)
	{
		const std::wstring wideName = utf8ToWideStr(name);

		BOOL result = SetEnvironmentVariable(wideName.c_str(), NULL);
		if (!result)
		{
			return makeErrorCode(Error::UnsetEnvironmentFailed);
		}

		return std::error_code();
	}

#else
	std::string platform_getEnv(const char* name)
	{
		DWORD sizeNeeded = GetEnvironmentVariable(name, nullptr, 0);
		if (sizeNeeded <= 0)
		{
			printf("failed to get environment - name: %s errorcode: %ld\n", name, GetLastError());
			return "";
		}

		std::string buffer(sizeNeeded, '\0');
		DWORD result = GetEnvironmentVariable(name, buffer.data(), sizeNeeded);
		if (result == 0)
		{
			printf("failed to get environment - name: %s errorcode: %ld\n", name, GetLastError());
			return "";
		}

		return buffer;
	}

	std::error_code platform_setEnv(std::string_view name, std::string_view value)
	{
		if (name.empty() || value.empty())
		{
			return makeErrorCode(Error::InvalidArguments);
		}

		const std::string variableName(name);
		const std::string variableValue(value);

		BOOL result = SetEnvironmentVariable(variableName, variableValue);
		if (!result)
		{
			return makeErrorCode(Error::SetEnvironmentFailed);
		}

		return std::error_code();
	}

	std::error_code platform_unsetEnv(const char* name)
	{
		BOOL result = SetEnvironmentVariable(name, NULL);
		if (!result)
		{
			return makeErrorCode(Error::UnsetEnvironmentFailed);
		}

		return std::error_code();
	}

#endif
} // namespace cppenv
