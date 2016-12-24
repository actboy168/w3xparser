#pragma once

#include <stdint.h>

namespace w3x {
	namespace mdx {
		inline void rewriteFloats(char*& buf, size_t n)
		{
			for (size_t i = 0; i < n; ++i)
			{
				*(uint16_t*)buf = 0;
				buf += 4;
			}
		}

		inline void rewriteChunk(char*& buf, size_t n)
		{
			buf += 4;
			uint32_t len = *(uint32_t*)buf;
			buf += 4;
			rewriteFloats(buf, len * n);
		}

		inline void skipChunk(char*& buf, size_t n)
		{
			buf += 4;
			uint32_t len = *(uint32_t*)buf;
			buf += 4 + n * len;
		}

		inline void processGEOS(char*& buf)
		{
			const char* end = buf + *(uint32_t*)buf + 4;
			buf += 4;
			while (buf < end)
			{
				buf += 4;
				rewriteChunk(buf, 3); // VRTX
				rewriteChunk(buf, 3); // NRMS
				skipChunk(buf, 4); // PTYP
				skipChunk(buf, 4); // PCNT
				skipChunk(buf, 2); // PVTX
				skipChunk(buf, 1); // GNDX
				skipChunk(buf, 4); // MTGC
				skipChunk(buf, 4); // MATS
				buf += 36;
				rewriteChunk(buf, 7);
				buf += 4;		// UVAS
				uint32_t uvas_count = *(uint32_t*)buf;
				buf += 4;
				for (size_t i = 0; i < uvas_count; ++i)
				{
					rewriteChunk(buf, 2); // UVBS
				}
			}
		}

		inline void processKXXX(char*& buf, int dimensions)
		{
			uint32_t n = *(uint32_t*)buf;
			buf += 4;
			uint32_t lineType = *(uint32_t*)buf;
			uint32_t floatsPerData = dimensions * ((lineType>1) ? 3 : 1);
			buf += 8;
			for (size_t i = 0; i < n; ++i)
			{
				buf += 4;
				rewriteFloats(buf, floatsPerData);
			}
		}

		inline void processKXXXList(char*& buf, const char* end)
		{
			while (buf < end)
			{
				switch (*(uint32_t*)buf)
				{
				case 'CSGK':
				case 'RTGK':
					buf += 4;
					processKXXX(buf, 3);
					break;
				case 'TRGK':
					buf += 4;
					processKXXX(buf, 4);
					break;
				default:
					buf += 4;
					return;
				}
			}
		}

		inline void processBONE(char*& buf)
		{
			const char* end = buf + *(uint32_t*)buf + 4;
			buf += 4;
			while (buf < end)
			{
				uint32_t n = *(uint32_t*)buf;
				const char* xend = buf + n;
				buf += 96;
				processKXXXList(buf, xend);
				buf += 8;
			}
		}

		inline void processHELP(char*& buf)
		{
			const char* end = buf + *(uint32_t*)buf + 4;
			buf += 4;
			while (buf < end)
			{
				uint32_t n = *(uint32_t*)buf;
				const char* xend = buf + n;
				buf += 96;
				processKXXXList(buf, xend);
			}
		}

		inline void opt(char* buf, int size)
		{
			const char* end = buf + size;
			if (*(uint32_t*)buf != 'XLDM')
				return;
			buf += 4;

			for (; buf<end;)
			{
				switch (*(uint32_t*)buf)
				{
				case 'SOEG':
					buf += 4;
					processGEOS(buf);
					break;
				case 'ENOB':
					buf += 4;
					processBONE(buf);
					break;
				case 'PLEH':
					buf += 4;
					processHELP(buf);
					break;
				default:
					buf += 4;
					buf += 4 + *(uint32_t*)buf;
					break;
				}
			}
		}
	}
}
