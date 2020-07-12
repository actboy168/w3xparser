package.path = [[.\test\?.lua]]
package.cpath = [[.\build\msvc\bin\?.dll]]

local w3xparser = require 'w3xparser'
local print_r = require 'print_r'

local function EQUAL(a, b)
	for k, v in pairs(a) do
		if type(v) == 'table' then
			EQUAL(v, b[k])
		else
			assert(v == b[k])
		end
	end
end

local n = 0
local function TEST(type, script, t)
	n = n + 1
	local name = 'TEST-' .. n
	local r = w3xparser[type](script)
	local ok, e = pcall(EQUAL, r, t)
	if not ok then
		print(script)
		print('--------------------------')
		print_r(r)
		print('--------------------------')
		print_r(t)
		print('--------------------------')
		error(name)
	end
	local ok, e = pcall(EQUAL, t, r)
	if not ok then
		print(script)
		print('--------------------------')
		print_r(r)
		print('--------------------------')
		print_r(t)
		print('--------------------------')
		error(name)
	end
end

TEST('txt', [==[
[A]]
// [B]
 [C]
 k= 1 // 1
[A]
m=1
 k=2
]==]
,
{
  a = { [' k'] = {' 1 ','2'},  ['m'] = {'1'}  }
}
)

TEST('slk', [==[
ID;PWXL;N;E
B;Y2;X3
C;Y1;X1;K""
C;X2;K"t2"
C;X3;K"t3"
C;Y2;X1;K"t1"
C;X2;K1.0123456789012
C;X3;K"0;XX"
E
]==]
,
{
	t1 = {
		t2 = 1.0123456789012,
		t3 = '0;XX'
	}
}
)

TEST('ini', [==[
[A]
// [B]
 [C]
 k= 1 
[A]
m=1
 k=2
]==]
,
{
  A = { [' k'] = ' 1 ',  ['m'] = '1' }
}
)

-- txt value的切割规则(神TM复杂)
-- 1.如果第一个字符是逗号,则添加一个空串
-- 2.如果当前字符为引号,则匹配到下一个引号,并忽略2端的字符
-- 3.如果当前字符为逗号,则忽略该字符.如果上一个字符是逗号,则添加一个空串
-- 4.否则匹配到下一个逗号,并忽略该字符
local list = {
{'1',        { '1' }},
{',',        { '' }},
{',1',       { '' , '1' }},
{'1,',       { '1' }},
{'1,1',      { '1' , '1' }},
{'"1"',      { '1' }},
{'"1',       { '1' }},
{'1"',       { '1"' }},
{'"1,1"',    { '1,1' }},
{'"1"1',     { '1' , '1' }},
{'"1",1',    { '1' , '1' }},
{'"1","1"',  { '1' , '1' }},
{'1"1"',     { '1"1"' }},
{'1,"1"',    { '1' , '1' }},
{'1,,1',     { '1' , '' , '1' }},
{'1"1"1',    { '1"1"1' }},
{',1"1"1',   { '' , '1"1"1' }},
{'1,"1"1',   { '1' , '1' , '1' }},
{'1",1"1',   { '1"' , '1"1' }},
{'1"1,"1',   { '1"1' , '1' }},
{'1"1",1',   { '1"1"' , '1' }},
{'1"1"1,',   { '1"1"1' }},
{'1,1,1',    { '1' , '1' , '1' }},
{'"1,1,1',   { '1,1,1' }},
{'1",1,1',   { '1"' , '1' ,'1' }},
{'1,"1,1',   { '1' , '1,1' }},
{'1,1",1',   { '1' , '1"' , '1' }},
{'1,1,"1',   { '1' , '1' , '1' }},
{'1,1,1"',   { '1' , '1' , '1"' }},
{',"',       { '' , '' }},
{',",',      { '' , ',' }},
{',","',     { '' , ',' }},
{',",",',    { '' , ',' }},
{',",","',   { '' , ',' , "" }},
{',",",",',  { '' , ',' , ',' }},
{'1,1"',     { '1' , '1"' }},
{'"1"2,3',   { '1' , '2' , '3' }},
}

for _, test in ipairs(list) do
	TEST('txt', '[a]\nb=' .. test[1], {a={b=test[2]}})
end

local tonumber = w3xparser.tonumber
assert(tonumber('0.1') == 0.1)
assert(tonumber('.1') == 0.1)
assert(tonumber(' .1') == 0.1)
assert(tonumber('a.1') == 0)
assert(tonumber('10') == 10)
assert(tonumber('010') == 8)
assert(tonumber('0x10') == 16)
assert(tonumber(' 10') == 10)
assert(tonumber(' 010') == 8)
assert(tonumber(' 0x10') == 16)
assert(tonumber("'a'") == 97)
assert(tonumber("'abcd'") == 1633837924)
assert(tonumber("'abcde'") == 0)
assert(tonumber(" 'a'") == 0)
assert(tonumber('0.1 a') == 0.1)
assert(tonumber('0.1a') == 0.1)
assert(tonumber(' 10 1') == 10)
assert(tonumber(' 10 //test') == 10)


local float2bin = w3xparser.float2bin
local bin2float = w3xparser.bin2float

assert(bin2float(float2bin('10')) == '10.')
assert(bin2float(float2bin('10.0')) == '10.')
assert(bin2float(float2bin('10.1')) == '10.1')
assert(bin2float(float2bin(' 10.1')) == '10.1')
assert(bin2float(float2bin(' 10.1 //test')) == '10.1')
assert(bin2float(float2bin(' 10.1 1')) == '10.1')
assert(bin2float(float2bin('10.123456789012345678901234567890')) == '10.123457')
assert(bin2float(float2bin('0.1')) == '0.1')
assert(bin2float(float2bin('.1')) == '0.1')
assert(bin2float(float2bin('.1234567890')) == '0.123457')
assert(bin2float(float2bin('0.99')) == '0.99')

print('test ok!')
