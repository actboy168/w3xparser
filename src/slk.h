#pragma once

#include "common.h"
#include <stdint.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>	
#include <vector>
#include <lua.hpp> 

namespace w3x {

	struct slk {
		std::vector<std::string_view> col;
		std::vector<std::string_view> row;
		std::vector<std::vector<std::string_view>> data;

		lua_State* l;
		const char* z;
		int line = 1;

		uint32_t maxx = 0;
		uint32_t maxy = 0;
		uint32_t curx = 1;
		uint32_t cury = 1;

		slk(const char* input)
			: z(input)
		{ }

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

		bool parse_whitespace_and_newline()
		{
			bool has_nl = false;
			for (;;)
			{
				if (!consume(z, " \t"))
				{
					if (equal(z, "\n\r"))
					{
						incline();
						has_nl = true;
					}
					else
					{
						return has_nl;
					}
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

		bool parse_integer(uint32_t& i)
		{
			const char* p = z;
			i = 0;
			if (!consume(p, '0'))
			{
				if (!is_digit(*p))
					return error("invalid number near '%s'", std::string(z, p - z + 1).c_str());
				i *= 10;
				i += *p - '0';
				for (p++; is_digit(*p); p++)
				{
					// 2^32 = 4294967296
					if (i > 0x19999999 || (i == 0x19999999 && *p >= '6')) {
						return error("integer too big near '%s'", std::string(z, p - z).c_str());
					}
					i *= 10;
					i += *p - '0';
				}
			}
			z = p;
			return true;
		}

		bool parse_line_b()
		{
			z++;
			for (;;) {
				parse_whitespace();
				if (!consume(z, ';')) {
					if (equal(z, "\n\r")) {
						return true;
					}
					return false;
				}
				parse_whitespace();

				switch (*z) {
				case 'X':
					z++;
					parse_whitespace();
					if (!parse_integer(maxx)) {
						return false;
					}
					break;
				case 'Y':
					z++;
					parse_whitespace();
					if (!parse_integer(maxy)) {
						return false;
					}
					break;
				default:
					for (; *z != ';'; z++) {
						switch (*z) {
						case '\0': return true;
						case '\n': return true;
						case '\r': return true;
						default: break;
						}
					}
					break;
				}
			}
		}

		bool parse_line_c()
		{
			z++;
			for (;;) {
				parse_whitespace();
				if (!consume(z, ';')) {
					if (equal(z, "\n\r")) {
						return true;
					}
					return false;
				}
				parse_whitespace();
				switch (*z) {
				case 'X':
					z++;
					parse_whitespace();
					if (!parse_integer(curx)) {
						return false;
					}
					if (maxx < curx || curx < 1) {
						return error("slk data corrupted.");
					}
					break;
				case 'Y':
					z++;
					parse_whitespace();
					if (!parse_integer(cury)) {
						return false;
					}
					if (maxy < cury || cury < 1) {
						return error("slk data corrupted.");
					}
					break;
				case 'K': {
					z++;
					parse_whitespace();
					const char* p = z;
					for (; *p != ';' && *p != '\n' && *p != '\r' && *p != '\0'; ++p)
					{
					}
					std::string_view s(z, p - z);
					trim_right(s);

					if (curx == 1) {
						if (cury != 1) {
							col[cury - 1] = s;
						}
					}
					else if (cury == 1) {
						row[curx - 1] = s;
					}
					else {
						data[curx - 1][cury - 1] = s;
					}
					z = p;
				}
					break;
				default:
					for (; *z != ';'; z++) {
						switch (*z) {
						case '\0': return true;
						case '\n': return true;
						case '\r': return true;
						default: break;
						}
					}
					break;
				}
			}
		}

		bool parse_line()
		{
			switch (*z) {
			case 'B':
				if (!parse_line_b()) {
					return false;
				}
				if (maxx == 0 || maxy == 0) {
					return error("slk data corrupted.");
				}
				row.resize(maxx);
				col.resize(maxy);
				data.assign(maxx, std::vector<std::string_view>(maxy));
				return true;
			case 'C':
				if (maxx == 0) {
					return error("slk data corrupted.");
				}
				return parse_line_c();
			case 'E':
				return true;
			default:
				for (;; z++) {
					switch (*z) {
					case '\0': return true;
					case '\n': return true;
					case '\r': return true;
					default: break;
					}
				}
				break;
			}
		}

		bool parse()
		{
			if (!consume_str(z, "ID;PWXL;N;E")) {
				return false;
			}
			while (!equal(z, '\0')) {
				if (!parse_whitespace_and_newline()) {
					return false;
				}
				if (!parse_line()) {
					return false;
				}
				if (equal(z, 'E')) {
					return true;
				}
			}
			return false;
		}
	};
}
