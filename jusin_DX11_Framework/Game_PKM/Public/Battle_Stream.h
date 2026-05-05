#pragma once
#include "Battle_Data.h"

NS_BEGIN(Game_PKM)

class IStream
{
public:
	virtual ~IStream() = default;

	virtual _bool Is_Reading() const PURE;
	virtual _bool Is_Writing() const PURE;

	virtual void IO(void* pData, size_t iSize) PURE;
};

inline void Serialize(IStream& Stream, _ubyte& v) { Stream.IO(&v, sizeof(v)); }
inline void Serialize(IStream& Stream, _byte& v) { Stream.IO(&v, sizeof(v)); }
inline void Serialize(IStream& Stream, _ushort& v) { Stream.IO(&v, sizeof(v)); }
inline void Serialize(IStream& Stream, _uint& v) { Stream.IO(&v, sizeof(v)); }
inline void Serialize(IStream& Stream, _float& v) { Stream.IO(&v, sizeof(v)); }
inline void Serialize(IStream& Stream, _bool& v) { Stream.IO(&v, sizeof(v)); }

void Serialize(IStream& Stream, POKEMON_INSTANCE& v);
void Serialize(IStream& Stream, PARTY& v);
void Serialize(IStream& Stream, TRAINER_DATA& v);

NS_END