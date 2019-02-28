#pragma once 

#include <assert.h>
#include <ctype.h>
#include <string_view>

namespace w3x {

	enum class ctype {
		none = 0,
		digit = 1,
		alpha = 2,
		underscode = 4,
	};

	static ctype ctypemap[256] = {
		/*0x00*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x10*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x20*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x30*/ ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x40*/ ctype::none, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
		/*0x50*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::underscode,
		/*0x60*/ ctype::none, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
		/*0x70*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x80*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x90*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xA0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xB0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xC0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xD0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xE0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xF0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
	};

	inline void expect(const char*& p, const char c)
	{
		assert(*p == c);
		p++;
	}

	inline bool is_digit(char c)
	{
		return ctypemap[c] == ctype::digit;
	}

	inline bool is_alpha_or_underscode(char c)
	{
		return !!((int)ctypemap[c] & ((int)ctype::alpha | (int)ctype::underscode));
	}

	inline bool is_alnum_or_underscode(char c)
	{
		return !!((int)ctypemap[c] & ((int)ctype::alpha | (int)ctype::digit | (int)ctype::underscode));
	}

	inline bool equal(const char* p, const char c)
	{
		return *p == c;
	}

	inline bool equal(const char* p, const char c[])
	{
		return *p == c[0] || *p == c[1];
	}

	inline bool consume(const char*& p, const char c)
	{
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	inline bool consume(const char*& p, const char c[])
	{
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	template <size_t n>
	bool equal_str(const char* p, const char(&c)[n])
	{
		return *p == c[0] || equal_str<n - 1>(p + 1, reinterpret_cast<const char(&)[n - 1]>(*(&c + 1)));
	}

	template <>
	bool equal_str<1>(const char* p, const char(&c)[1])
	{
		return *p == c[0];
	}

	template <size_t n>
	bool consume_str(const char*& p, const char(&c)[n])
	{
		if (equal_str(p, c)) {
			p += n - 1;
			return true;
		}
		return false;
	}

	inline void trim_right(std::string_view& input)
	{
		for (auto it = input.end(); it != input.begin();)
		{
			if (!isspace((unsigned char)*(--it)))
			{
				input.remove_suffix(std::distance(++it, input.end()));
				return;
			}
		}
		input.remove_suffix(input.size());
	}
}
