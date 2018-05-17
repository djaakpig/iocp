#pragma once

struct NonCopyable abstract
{
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(const NonCopyable&&) = delete;
	const NonCopyable& operator =(const NonCopyable&) = delete;
	const NonCopyable& operator =(const NonCopyable&&) = delete;
};