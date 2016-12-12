#pragma once

#include "common.h"
#include <stdint.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>	
#include <vector>
#include <lua.hpp> 

namespace w3x {

	struct number {
		lua_State* l;
		const char* z;

		number(lua_State* L, const char* input)
			: l(L)
			, z(input)
		{ }

		void parse_number()
		{
			for (bool ok = false; !ok;) {
				switch (*z) {
				case '\0':
				case '\n':
				case '\r':
					accept_integer(0);
					return;
				case '\t':
				case ' ':
					z++;
					break;
				default:
					ok = true;
					break;
				}
			}

			char* endptr = 0;
			errno = 0;
			long i = strtol(z, &endptr, 0);
			if (errno == ERANGE && (i == LONG_MAX || i == LONG_MIN)) {
				accept_integer(0);
				return;
			}
			if (endptr && *endptr != '.') {
				accept_integer(i);
				return;
			}
			errno = 0;
			double d = strtod(z, NULL);
			if (errno == ERANGE && (d == HUGE_VAL || d == -HUGE_VAL)) {
				accept_double(0.);
				return;
			}
			accept_double(d);
		}

		void parse_ascii()
		{
			const char* p = z;
			int32_t n = 0;
			for (; z - p <= 4;) {
				switch (*z) {
				case '\0':
				case '\n':
				case '\r':
				case '\'':
					accept_integer(n);
					return;
				default:
					n <<= 8;
					n += (int32_t)(unsigned char)*z;
					z++;
					break;
				}
			}
			accept_integer(0);
		}

		void parse()
		{
			if (*z == '\'') {
				z++;
				return parse_ascii();
			}
			parse_number();
		}

		void accept_integer(int64_t i) {
			lua_pushinteger(l, i);
		}
		void accept_double(double d) {
			lua_pushnumber(l, d);
		}
	};
}
