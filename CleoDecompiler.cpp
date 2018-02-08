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

#include "CleoDecompiler.h"
#include <functional>

#include "FileMan/cfiletext.h"
#include "FileMan/cfilebin.h"
#include "FileMan/cfileinfo.h"

CleoDecompiler::CleoDecompiler( SRString SCM )
{
	const std::regex reOpcode( R"(([0-9a-f]{4}=-?\d),(.*))", std::regex::icase );
	const std::regex reParams( R"(([0-9a-f]{4})=(-?\d))", std::regex::icase );

	_script = nullptr;
	_decompileMap = nullptr;
	setDefaultTypes();

	CFileText scm( SCM );
	scm.toggleSafeOnDelete( false );

	for( int i = 0; i < scm.count(); ++i ) {
		SRString opcode( scm.at( i ) );

		if( opcode.indexIn( reOpcode ) ) {
			SRString params = opcode.cap( 1 );
			SRString text = opcode.cap( 2 );

			if( params.indexIn( reParams ) ) {
				int opcodeId = SRString( params.cap( 1 ) ).toInt( 16 );
				int paramsCount = SRString( params.cap( 2 ) ).toInt();
				_params[opcodeId] = {paramsCount, text};
			}
		}
	}
}

CleoDecompiler::~CleoDecompiler()
{
	delete_s( _script );
	delete_s( _decompileMap );
}

bool CleoDecompiler::loadScript( SRString script )
{
	if( !CFileInfo::exist( script ) )
		return false;

	CFileBin bin( script );
	bin.toggleSafeOnDelete( false );

	_queue.clear();
	_outputcode.clear();
	_ip = 0;

	delete_s( _script );
	delete_s( _decompileMap );

	_size = bin.size();
	_script = new unsigned char[bin.size()];
	_decompileMap = new unsigned char[bin.size()];

	memset( _decompileMap, 0, bin.size() );

	for( int i = 0; i < bin.size(); ++i )
		_script[i] = bin.at( i );

	return true;
}

SRStringList CleoDecompiler::decompile()
{
	SRStringList ret;
	processDecompile();
	processHexInsert();

	for( auto out : _outputcode ) {
		if( hasInDeque( _labels, out.first ) )
			ret.push_back( SRString::sprintf( "\n:label_%08X", out.first ) );

		ret.push_back( out.second );
	}

	return ret;
}

void CleoDecompiler::processHexInsert()
{
	int start = -1;
	SRString opcode;
	std::function<void( int, int )> comment =
	[this, &opcode]( int i, int j ) {
		opcode += "// ";

		for( int k = i - j; k < i; ++k )
			opcode += hexSymbol( ( int )_script[k] );
	};

	for( int i = 0; i < _size; ++i ) {
		if( _decompileMap[i] == 1 )
			continue;

		if( start == -1 ) {
			start = i;
			opcode = "hex\n  ";
		}

		for( int j = 0;; ++j, ++i ) {
			if( _decompileMap[i] == 1 ) {
				comment( i, j );
				opcode += "\nend";
				_outputcode[start] = opcode;
				start = -1;
				break;
			}

// 			if (j == 8){
// 				opcode += "\n  ";
// 				j = -1;
// 			}
// 			SRString ch = "";
// 			if ((int)_script[i] >= 0x20)
// 				ch = SRString::sprintf("%c", (int)_script[i]);
// 			opcode += SRString::sprintf("%02X{%s} ", _script[i], ch.c_str());
			if( j == 16 ) {
				comment( i, j );
				opcode += "\n  ";
				j = -1;
			}

			opcode += SRString::sprintf( "%02X ", _script[i] );
		}
	}
}

SRString CleoDecompiler::hexSymbol( int code )
{
	if( code >= 0x20 || code == 0x09 )
		return SRString::sprintf( "%c", code );
	switch( code ){
		case 0x00:
			return "<NULL>";
		case 0x01:
			return "<SOH>";
		case 0x02:
			return "<STX>";
		case 0x03:
			return "<ETX>";
		case 0x04:
			return "<EOT>";
		case 0x05:
			return "<ENQ>";
		case 0x06:
			return "<ACK>";
		case 0x07:
			return "<BEL>";
		case 0x08:
			return "<BS>";
		case 0x0A:
			return "<LF>";
		case 0x0B:
			return "<VT>";
		case 0x0C:
			return "<FF>";
		case 0x0D:
			return "<CR>";
		case 0x0E:
			return "<SO>";
		case 0x0F:
			return "<SI>";
		case 0x10:
			return "<DLE>";
		case 0x11:
			return "<DC1>";
		case 0x12:
			return "<DC2>";
		case 0x13:
			return "<DC3>";
		case 0x14:
			return "<DC4>";
		case 0x15:
			return "<NAK>";
		case 0x16:
			return "<SYN>";
		case 0x17:
			return "<ETB>";
		case 0x18:
			return "<CAN>";
		case 0x19:
			return "<EM>";
		case 0x1A:
			return "<SUB>";
		case 0x1B:
			return "<ESC>";
		case 0x1C:
			return "<FS>";
		case 0x1D:
			return "<GS>";
		case 0x1E:
			return "<RS>";
		case 0x1F:
			return "<US>";
	}
	return "<WTF?>";
}

void CleoDecompiler::processDecompile()
{
	while( true ) {

		while( _decompileMap[_ip] == 1 && _ip < _size )
			_ip++;

		if( _ip >= _size )
			if( !nextQueue() )
				break;

		auto ip = _ip;
		auto op = readOpcode();

		if( op.isEmpty() ) {
			undo();

			if( !nextQueue() )
				break;

			continue;
		}

// 		_outputcode.push_back({ip, op});
		_outputcode[ip] = op;
	}
}

bool CleoDecompiler::nextQueue()
{
	if( _queue.size() == 0 )
		return false;;

	_ip = _queue.front();
	_queue.pop_front();
	return true;
}

SRString CleoDecompiler::readOpcode()
{
	auto opcode = readOpcodeNumber();
	stParams params;

	try {
		params = _params.at( opcode.opcode );
	}
	catch( std::exception e ) {
		return "";
	}

	auto opcodeText = params.opcode;
	auto args = readArgs( opcode.opcode );

	if( opcode.opcode == 0x0002 ) { // jump
		auto label = args.front().toInt() * -1;

		if( _decompileMap[label] == 0 )
			_ip = label;
	}
	else if( opcode.opcode == 0x0D6 ) { // if
		auto t = args.front().toInt();

		if( t == 0 )
			args.front() = "";
		else if( t > 10 )
			args.front() = "or";
		else
			args.front() = "and";
	}

	pasteArgs( opcodeText, args );

	if( opcode.isInvert ) {
		opcode.opcode += 0x8000;
		opcodeText = "not " + opcodeText;
	}

	return SRString::sprintf( "%04X: %s", opcode, opcodeText.c_str() );
}

void CleoDecompiler::pasteArgs( SRString &opcodeText, SRStringList &args )
{
	SRString re = "(\%1\\w\%)";
	int i = 1;

	while( opcodeText.indexIn( re ) ) {
		auto argId = opcodeText.cap( 1 );

		if( argId.indexIn( R"(\d+p)" ) ) {
			int label = args.front().toInt() * -1;

			if( _decompileMap[label] == 0 )
				_queue.push_back( label );

			if( !hasInDeque( _labels, label ) )
				_labels.push_back( label );

			args.front() = SRString::sprintf( "@label_%08X", label );
		}

		if( args.size() > 0 ) {
			opcodeText.replace( argId, args.front() );
			args.pop_front();
		}
		else throw std::runtime_error( "Error: Incorrect args" );

		re = "(\%" + std::to_string( ++i ) + "\\w\%)";
	}

	for( ; args.size() > 0; ) {
		opcodeText += " " + args.front();
		args.pop_front();
	}
}

SRStringList CleoDecompiler::readArgs( unsigned short opcode )
{
	SRStringList params;
	std::function<bool()> readAndJoinParam =
	[this, &params]() {
		auto param = readArg();

		if( param.isEmpty() )
			return false;

		params.push_back( param );
		return true;
	};

	if( _params[opcode].count != -1 ) {
		for( int i = 0; i < _params[opcode].count; ++i )
			readAndJoinParam();
	}
	else {
		while( true ) {
			if( !readAndJoinParam() )
				break;
		}
	}

	return params;
}

SRString CleoDecompiler::readArg()
{
	unsigned char typeId = read<unsigned char>();

	if( typeId == 0 )
		return "";

	switch( Type( typeId ) ) {
		case eVT_Int:
			return std::to_string( read<int>() );

		case eVT_Float:
			return std::to_string( read<float>() );

		case eVT_String:
			return "\"" + read( read<unsigned char>() ) + "\"";

		case eVT_String8:
			return "'" + read( 8 ) + "' /* 8b string*/";

		case eVT_String16:
			return "'" + read( 16 ) + "' /* 16b string */";

		case eVT_LocalVar:
			return std::to_string( read<short>() ) + "@";

		case eVT_GlobalVar:
			return "$" + std::to_string( read<unsigned short>() );

		case eVT_LocalVarString8:
			return std::to_string( read<short>() ) + "@s";

		case eVT_GlobalVarString8:
			return "$" + std::to_string( read<unsigned short>() ) + "s";

		case eVT_LocalVarString16:
			return std::to_string( read<short>() ) + "@v";

		case eVT_GlobalVarString16:
			return "$" + std::to_string( read<unsigned short>() ) + "v";

		case eVT_Short:
			return std::to_string( ( int )read<short>() );

		case eVT_Byte:
			return std::to_string( ( int )read<char>() );
	}
}

stOpcodeNumber CleoDecompiler::readOpcodeNumber()
{
	if( _script == nullptr )
		throw std::runtime_error( "Script not loaded" );

	bool isInvert = false;
	auto opcode = read<unsigned short>();

	if( opcode > 0x7FFF ) {
		opcode -= 0x8000;
		isInvert = true;
	}

	return {opcode, isInvert};
}

SRString CleoDecompiler::read( int len )
{
	char *str = new char[len + 1];
	memcpy( str, ( void * )( ( size_t )_script + _ip ), len );
	lastSize.push_back( len );
	memset( ( void * )( ( size_t )_decompileMap + _ip ), 1, len );
	_ip += len;
	str[len] = 0;
	SRString ret = str;
	delete[] str;
	return ret;
}

void CleoDecompiler::undo()
{
	_ip -= lastSize.back();
	memset( ( void * )( ( size_t )_decompileMap + _ip ), 0, lastSize.back() );
	lastSize.pop_back();
}

void CleoDecompiler::setDefaultTypes()
{
	SetTypeId( 0x01, eVT_Int );
	SetTypeId( 0x06, eVT_Float );
	SetTypeId( 0x02, eVT_GlobalVar );
	SetTypeId( 0x03, eVT_LocalVar );
	SetTypeId( 0x05, eVT_Short );
	SetTypeId( 0x04, eVT_Byte );
	SetTypeId( 0x09, eVT_String8 );
	SetTypeId( 0x0A, eVT_GlobalVarString8 );
	SetTypeId( 0x0B, eVT_LocalVarString8 );
	SetTypeId( 0x0E, eVT_String );
	SetTypeId( 0x0F, eVT_String16 );
	SetTypeId( 0x10, eVT_LocalVarString16 );
	SetTypeId( 0x11, eVT_GlobalVarString16 );
}

void CleoDecompiler::SetTypeId( int id, eVarType type )
{
	_types[id] = type;
}

eVarType CleoDecompiler::Type( int id )
{
	return _types[id];
}

std::map<unsigned short, stParams> CleoDecompiler::paramsData()
{
	return _params;
} 
