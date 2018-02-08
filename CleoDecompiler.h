/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  SR_team <sr-tream@yandex.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLEODECOMPILER_H
#define CLEODECOMPILER_H

#include <map>

#include "SRString.hpp"

enum eVarType {
	eVT_Int,
	eVT_Float,
	eVT_String,
	eVT_String8,
	eVT_String16,
	eVT_LocalVar,
	eVT_GlobalVar,
	eVT_LocalVarString8,
	eVT_GlobalVarString8,
	eVT_LocalVarString16,
	eVT_GlobalVarString16,
	eVT_Short,
	eVT_Byte
};

struct stParams {
	int count;
	SRString opcode;
};

struct stOpcodeNumber {
	int opcode;
	bool isInvert;
};

template<typename T>
bool delete_s( T ptr )
{
	if( ptr == nullptr )
		return false;

	delete[] ptr;
	ptr = nullptr;
	return true;
}

template<typename T>
static bool hasInDeque( std::deque<T> &deq, const T &value )
{
	for( auto it : deq )
		if( it == value )
			return true;

	return false;
}

/**
 * Decompiler for cleo scripts
 */
class CleoDecompiler
{
public:
	/**
	 * \brief Default constructor
	 * \detail Load and parse SCM file
	 */
	CleoDecompiler( SRString SCM = "SASCM.INI" );

	/**
	 * Destructor
	 */
	~CleoDecompiler();

	bool loadScript( SRString script );

	SRStringList decompile();

protected:
	void processHexInsert();
	SRString hexSymbol( int code );
	void processDecompile();
	bool nextQueue();

public:
	SRString readOpcode();

protected:
	void pasteArgs( SRString &opcodeText, SRStringList &args );
	SRStringList readArgs( unsigned short opcode );
	SRString readArg();
	stOpcodeNumber readOpcodeNumber();
	SRString read( int len );
	template<typename T>
	T read()
	{
		lastSize.push_back( sizeof( T ) );
		T value = *( T * )( ( size_t )_script + _ip );
		memset( ( void * )( ( size_t )_decompileMap + _ip ), 1, lastSize.back() );
		_ip += lastSize.back();
		return value;
	}
	void undo();

public:
	void setDefaultTypes();
	void SetTypeId( int id, eVarType type );
	eVarType Type( int id );

	std::map<unsigned short, stParams> paramsData();

private:
	std::deque<int>lastSize; // Размер прочтенного блока
	std::map<unsigned short, stParams> _params; // [опкод] = параметры
	std::map<int, eVarType> _types; // [id типа] = тип
	std::deque<int> _queue; // Очередь веток
	int _ip; // текущая позиция
	int _size; // Размер скрипта
	unsigned char *_script; // Скрипт
	unsigned char *_decompileMap; // Карта скрипта. 1 - декомпилированный участок, 0 - не декомпилированный
	std::deque<int> _labels; // адреса меток
// 	std::deque<std::pair<int, SRString>> _outputcode; // <адрес|опкод>
	std::map<int, SRString> _outputcode; // [адрес] = опкод
};

#endif // CLEODECOMPILER_H
