//--------------------------------------------------------------------------------------
// File: BinaryReader.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <exception>
#include <stdexcept>
#include <type_traits>

//#include "PlatformHelpers.h"


namespace DirectX
{
    // Helper for reading binary data, either from the filesystem a memory buffer.
    class BinaryReader
    {
    public:
        explicit BinaryReader(_In_z_ wchar_t const* fileName);
        BinaryReader(_In_reads_bytes_(dataSize) uint8_t const* dataBlob, size_t dataSize);

        BinaryReader(BinaryReader const&) = delete;
        BinaryReader& operator= (BinaryReader const&) = delete;
        
        // Reads a single value.
        template<typename T> T const& Read()
        {
            return *ReadArray<T>(1);
        }


        // Reads an array of values.
        template<typename T> T const* ReadArray(size_t elementCount)
        {
            static_assert(std::is_pod<T>::value, "Can only read plain-old-data types");

            uint8_t const* newPos = mPos + sizeof(T) * elementCount;

            if (newPos < mPos)
                throw std::overflow_error("ReadArray");

            if (newPos > mEnd)
                throw std::exception("End of file");

            auto result = reinterpret_cast<T const*>(mPos);

            mPos = newPos;

            return result;
        }


        // Lower level helper reads directly from the filesystem into memory.
        static HRESULT ReadEntireFile(_In_z_ wchar_t const* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* dataSize);


    private:
        // The data currently being read.
        uint8_t const* mPos;
        uint8_t const* mEnd;

        std::unique_ptr<uint8_t[]> mOwnedData;
    };


	// Helper smart-pointers
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10) || (defined(_XBOX_ONE) && defined(_TITLE)) || !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	struct virtual_deleter { void operator()(void* p) noexcept { if (p) VirtualFree(p, 0, MEM_RELEASE); } };
#endif

	struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); } };

	struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

	typedef std::unique_ptr<void, handle_closer> ScopedHandle;

	inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }
}
