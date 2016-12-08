#pragma once

#include "common.h"
#include <stdint.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>	
#include <vector>
#include <lua.hpp> 

namespace w3x {

	struct txt {
		lua_State* l;
		const char* z;
		int line = 1;

		txt(lua_State* L, const char* input)
			: l(L)
			, z(input)
		{
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}

		void incline()
		{
			char old = *z;
			assert(equal(z, "\n\r"));
			z++;
			if (equal(z, "\n\r") && !equal(z, old))
				z++;
			++line;
		}

		void parse_whitespace()
		{
			for (;;)
			{
				if (!consume(z, " \t"))
				{
					return;
				}
			}
		}

		template <class ... Args>
		bool error(const char* fmt, const Args& ... args)
		{
			lua_pushinteger(l, line);
			lua_pushfstring(l, fmt, args...);
			return false;
		}

		bool parse_comment()
		{
			expect(z, '/');
			expect(z, '/');
			for (;; z++) {
				switch (*z) {
				case '\n':
				case '\r':
				case '\0':
					return true;
				}
			}
		}

		bool parse_section()
		{
			expect(z, '[');
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case ']':
					accept_section(p, z - p);
					z++;
					parse_whitespace();
					return true;
				case '\n':
				case '\r':
				case '\0':
					return error("']' expected near '%c'", *z);
				}
			}
		}

		bool parse_value()
		{
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case '\n':
				case '\r':
				case '\0':
					accept_value(p, z - p);
					return true;
				}
			}
		}

		bool parse_assgin()
		{
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case '=':
					accept_key(p, z - p);
					z++;
					return parse_value();
				case '\n':
				case '\r':
				case '\0':
					return true;
				}
			}
		}

		bool parse_line()
		{
			switch (*z) {
			case '[':
				return parse_section();
			case '/':
				if (z[1] == '/') {
					return parse_comment();
				}
			default:
				return parse_assgin();
			}
		}

		bool parse()
		{
			accept_begin();
			for (;;) {
				if (!parse_line()) {
					return false;
				}
				switch (*z) {
				case '\0':
					accept_end();
					return true;
				case '\n':
				case '\r':
					incline();
					break;
				default:
					return error("'\\n' expected near '%c'", *z);
				}
			}
		}

		void accept_begin()
		{
			lua_newtable(l);
			lua_newtable(l);
		}

		void accept_end()
		{
			lua_pop(l, 1);
		}

		void accept_section(const char* str, size_t len)
		{
			lua_pop(l, 1);
			lua_newtable(l);
			lua_pushlstring(l, str, len);
			lua_pushvalue(l, -2);
			lua_rawset(l, -4);
		}

		void accept_key(const char* str, size_t len)
		{
			lua_pushlstring(l, str, len);
		}

		void accept_value(const char* str, size_t len)
		{
			lua_pushlstring(l, str, len);
			lua_rawset(l, -3);
		}
	};
}
