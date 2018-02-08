#ifndef SRSTRING_H
#define SRSTRING_H

#include <deque>
#include <string>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <cstdarg>
#include <typeinfo>
#include <regex>
#include <locale>
#include <codecvt>

enum eRegExp{
	eRe_notMatched,
	eRe_matched,
	eRe_searched
};

template<typename TChar>
class SRStringBase : public std::basic_string<TChar>
{
public:
	SRStringBase() : std::basic_string<TChar>() {}
	SRStringBase(TChar c) { this->push_back(c); }
	SRStringBase(const TChar * str) : std::basic_string<TChar>(str) {}
	SRStringBase(TChar * str)  : std::basic_string<TChar>(str) {}
	SRStringBase(std::basic_string<TChar> str) : std::basic_string<TChar>(str) {}
	SRStringBase(const SRStringBase &str) : std::basic_string<TChar>(str.c_str()) {}

	template<typename ... Args>
	static SRStringBase<TChar> sprintf(const SRStringBase<TChar> &format, Args ... args)
	{
		size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
		std::unique_ptr<char[]> buf( new char[ size ] );
		snprintf( buf.get(), size, format.c_str(), args ... );
		return SRStringBase<TChar>( buf.get() );
	}

	template<typename ... Args>
	void format(const SRStringBase<TChar> &format, Args ... args)
	{
		this->clear();
		size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
		std::unique_ptr<char[]> buf( new char[ size ] );
		snprintf( buf.get(), size, format.c_str(), args ... );
		this->append(SRStringBase<TChar>( buf.get() ));
	}
	template<typename ... Args>
	void args(Args ... args)
	{
		size_t size = snprintf( nullptr, 0, this->c_str(), args ... ) + 1;
		std::unique_ptr<char[]> buf( new char[ size ] );
		snprintf( buf.get(), size, this->c_str(), args ... );
		this->clear();
		this->append(SRStringBase<TChar>( buf.get() ));
	}

	const SRStringBase<TChar> toUpper()
	{
		std::transform(this->begin(), this->end(), this->begin(), ::toupper);
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> toLower()
	{
		std::transform(this->begin(), this->end(), this->begin(), ::tolower);
		return SRStringBase<TChar>(*this);
	}

	int toInt(int base = 10)
	{
		int ret;
		try {
			ret = std::stoi(std::basic_string<TChar>(*this), 0, base);
		} catch (std::exception &) {
			ret = std::stoll(std::basic_string<TChar>(*this), 0, base);
		}
		return ret;
	}
	unsigned int toUInt(int base = 10)
	{
		unsigned int ret;
		try {
			ret = std::stoi(std::basic_string<TChar>(*this), 0, base);
		} catch (std::exception &) {
			ret = std::stoull(std::basic_string<TChar>(*this), 0, base);
		}
		return ret;
	}
	long toLong(int base = 10)
	{
		long ret;
		try {
			ret = std::stol(std::basic_string<TChar>(*this), 0, base);
		} catch (std::exception &) {
			ret = std::stoll(std::basic_string<TChar>(*this), 0, base);
		}
		return ret;
	}
	long long toLongLong(int base = 10)
	{
		return std::stoll(std::basic_string<TChar>(*this), 0, base);
	}
	unsigned long toULong(int base = 10)
	{
		unsigned long ret;
		try {
			ret = std::stoul(std::basic_string<TChar>(*this), 0, base);
		} catch (std::exception &) {
			ret = std::stoull(std::basic_string<TChar>(*this), 0, base);
		}
		return ret;
	}
	unsigned long long toULongLong(int base = 10)
	{
		return std::stoull(std::basic_string<TChar>(*this), 0, base);
	}
	float toFloat()
	{
		float ret;
		try {
			ret = std::stof(std::basic_string<TChar>(*this));
		} catch (std::exception &) {
			ret = std::stod(std::basic_string<TChar>(*this));
		}
		return ret;
	}
	double toDouble()
	{
		double ret;
		try {
			ret = std::stod(std::basic_string<TChar>(*this));
		} catch (std::exception &) {
			ret = std::stold(std::basic_string<TChar>(*this));
		}
		return ret;
	}
	long double toLongDouble()
	{
		return std::stold(std::basic_string<TChar>(*this));
	}

	const std::deque<SRStringBase<TChar>> split(TChar delimeter)
	{
		std::basic_stringstream<TChar> ss(*this);
		std::basic_string<TChar> item;
		std::deque<SRStringBase<TChar>> splittedStrings;
		while (std::getline(ss, item, delimeter))
		{
			splittedStrings.push_back(item);
		}
		return splittedStrings;
	}

	void push_front(TChar __c)
	{
		this->insert(0, std::basic_string<TChar>(SRStringBase(__c)));
	}
	void pop_front() noexcept
	{
		this->erase(0, 1);
	}

	const SRStringBase<TChar> replace(size_t start, size_t len, SRStringBase<TChar> new_str)
	{
		this->std::basic_string<TChar>::replace(start, len, std::basic_string<TChar>(new_str));
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> replace(SRStringBase<TChar> old_str, SRStringBase<TChar> new_str)
	{
		size_t find_pos = 0;
		while (true){
			size_t pos = this->find(std::basic_string<TChar>(old_str), find_pos);
			if (pos == this->npos)
				break;
			replace(pos, old_str.length(), new_str);
			find_pos = pos + new_str.length();
		}
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> replace(TChar old_str, SRStringBase<TChar> new_str)
	{
		size_t find_pos = 0;
		while (true){
			size_t pos = this->find(std::basic_string<TChar>(SRStringBase<TChar>(old_str)), find_pos);
			if (pos == this->npos)
				break;
			replace(pos, 1, new_str);
			find_pos = pos + 1;
		}
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> replace(SRStringBase<TChar> old_str, TChar new_str)
	{
		size_t find_pos = 0;
		while (true){
			size_t pos = this->find(std::basic_string<TChar>(old_str), find_pos);
			if (pos == this->npos)
				break;
			replace(pos, old_str.length(), new_str);
			find_pos = pos + new_str.length();
		}
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> replace(TChar old_str, TChar new_str)
	{
		size_t find_pos = 0;
		while (true){
			size_t pos = this->find(std::basic_string<TChar>(SRStringBase<TChar>(old_str)), find_pos);
			if (pos == this->npos)
				break;
			replace(pos, 1, new_str);
			find_pos = pos + 1;
		}
		return SRStringBase<TChar>(*this);
	}

	const SRStringBase<TChar> removeFirst(size_t len = 1)
	{
		std::basic_string<TChar>::erase(0, len);
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> removeLast(size_t len = 1)
	{
		std::basic_string<TChar>::erase(this->length() - len, len);
		return SRStringBase<TChar>(*this);
	}

	const SRStringBase<TChar> remove(size_t start, size_t len)
	{
		std::basic_string<TChar>::erase(start, len);
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> remove(TChar chr)
	{
		replace(chr, "");
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> remove(SRStringBase<TChar> str)
	{
		replace(str, "");
		return SRStringBase<TChar>(*this);
	}

	const SRStringBase<TChar> removeOne(TChar chr)
	{
		size_t pos = this->find(std::basic_string<TChar>(SRStringBase<TChar>(chr)));
		if (pos != this->npos){
			this->std::basic_string<TChar>::replace(pos, 1, std::basic_string<TChar>(""));
		}
		return SRStringBase<TChar>(*this);
	}
	const SRStringBase<TChar> removeOne(SRStringBase<TChar> str)
	{
		size_t pos = this->find(std::basic_string<TChar>(str));
		if (pos != this->npos){
			this->std::basic_string<TChar>::replace(pos, str.length(), std::basic_string<TChar>(""));
		}
		return SRStringBase<TChar>(*this);
	}

	bool isEmpty()
	{
		return (this->length() == 0);
	}

	bool indexOf(const SRStringBase<TChar> &str)
	{
		return (this->find(std::basic_string<TChar>(str)) != this->npos);
	}

	eRegExp indexIn(const std::regex &re)
	{
		if (std::regex_match(this->c_str(), cmatch, re))
			return eRegExp::eRe_matched;
		else if (std::regex_search(this->c_str(), cmatch, re))
			return eRegExp::eRe_searched;
		return eRegExp::eRe_notMatched;
	}
	eRegExp indexIn(const SRStringBase<TChar> &re, std::regex::flag_type flag = std::regex::ECMAScript)
	{
		std::regex _re(re, flag);
		return indexIn(_re);
	}
	const SRStringBase<TChar> cap(int id)
	{
		if (id >= cmatch.size())
			return "";
		return cmatch[id].str();
	}
	size_t sizeCap()
	{
		return cmatch.size();
	}
	const std::cmatch lastCmatch()
	{
		return cmatch;
	}

	const SRStringBase<wchar_t> toUnicode()
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		if (typeid (TChar) == typeid (wchar_t))
			return SRStringBase<TChar>(*this);
		return converter.from_bytes(*this);
	}
	const SRStringBase<char> toAnsi()
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		if (typeid (TChar) == typeid (char))
			return SRStringBase<TChar>(*this);
		return converter.to_bytes(*this);
	}

	void fromUnicode(const SRStringBase<wchar_t> &str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		this->clear();
		if (typeid (TChar) == typeid (wchar_t))
			this->append((TChar*)str.c_str());
		else this->append((TChar*)converter.to_bytes(str).c_str());
	}
	void fromAnsi(const SRStringBase<char> &str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		this->clear();
		if (typeid (TChar) == typeid (char))
			this->append((TChar*)str.c_str());
		else this->append((TChar*)converter.from_bytes(str).c_str());
	}

	static SRStringBase<wchar_t> Ansi2Unicode(const char* str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}
	static SRStringBase<char> Unicode2Ansi(const wchar_t* str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(str);
	}

private:
	std::cmatch cmatch;
};

typedef SRStringBase<char> SRString;
typedef std::deque<SRString> SRStringList;
typedef SRStringBase<wchar_t> SRWstring;
typedef std::deque<SRWstring> SRWstringList;

#endif // SRSTRING_H
