#include "engine/binary_identity.h"
#include <windows.h>
#include <bcrypt.h>
#include <array>
#include <cstring>

namespace nimby::engine {
namespace {
struct File {
    HANDLE value = INVALID_HANDLE_VALUE;
    ~File() { if (value != INVALID_HANDLE_VALUE) CloseHandle(value); }
};
struct Algorithm {
    BCRYPT_ALG_HANDLE value{};
    ~Algorithm() { if (value) BCryptCloseAlgorithmProvider(value, 0); }
};
struct Hash {
    BCRYPT_HASH_HANDLE value{};
    ~Hash() { if (value) BCryptDestroyHash(value); }
};
bool read_at(HANDLE file, LONGLONG offset, void* data, DWORD size) noexcept {
    LARGE_INTEGER pos{}; pos.QuadPart = offset;
    DWORD count{};
    return SetFilePointerEx(file, pos, nullptr, FILE_BEGIN) &&
        ReadFile(file, data, size, &count, nullptr) && count == size;
}
}
bool is_research_build(const NimbyBinaryInfo& info) noexcept {
    constexpr char known[] = "FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE";
    return info.file_size == 12583424 && std::memcmp(info.sha256, known, sizeof known) == 0;
}
uint32_t identify(const wchar_t* path, NimbyBinaryInfo& out) noexcept {
    out = {}; out.struct_size = sizeof out;
    if (!path || !*path) return NIMBY_INVALID_ARGUMENT;
    // Deny concurrent writes/deletion while headers and hash are read from one handle.
    File file{CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (file.value == INVALID_HANDLE_VALUE) return NIMBY_IO_ERROR;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.value, &size)) return NIMBY_IO_ERROR;
    IMAGE_DOS_HEADER dos{};
    if (!read_at(file.value, 0, &dos, sizeof dos) || dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < static_cast<LONG>(sizeof dos))
        return NIMBY_INVALID_BINARY;
    if (size.QuadPart < static_cast<LONGLONG>(dos.e_lfanew) + static_cast<LONGLONG>(sizeof(IMAGE_NT_HEADERS64))) return NIMBY_INVALID_BINARY;
    IMAGE_NT_HEADERS64 nt{};
    if (!read_at(file.value, dos.e_lfanew, &nt, sizeof nt) || nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt.FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER64)) return NIMBY_INVALID_BINARY;
    Algorithm algorithm;
    if (BCryptOpenAlgorithmProvider(&algorithm.value, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) return NIMBY_INTERNAL_ERROR;
    Hash hash;
    if (BCryptCreateHash(algorithm.value, &hash.value, nullptr, 0, nullptr, 0, 0) < 0) return NIMBY_INTERNAL_ERROR;
    LARGE_INTEGER start{};
    if (!SetFilePointerEx(file.value, start, nullptr, FILE_BEGIN)) return NIMBY_IO_ERROR;
    std::array<unsigned char, 65536> buffer{};
    uint64_t total{};
    for (;;) {
        DWORD count{};
        if (!ReadFile(file.value, buffer.data(), static_cast<DWORD>(buffer.size()), &count, nullptr)) return NIMBY_IO_ERROR;
        if (!count) break;
        total += count;
        if (BCryptHashData(hash.value, buffer.data(), count, 0) < 0) return NIMBY_INTERNAL_ERROR;
    }
    if (total != static_cast<uint64_t>(size.QuadPart)) return NIMBY_IO_ERROR;
    std::array<unsigned char, 32> digest{};
    if (BCryptFinishHash(hash.value, digest.data(), static_cast<ULONG>(digest.size()), 0) < 0) return NIMBY_INTERNAL_ERROR;
    constexpr char hex[] = "0123456789ABCDEF";
    size_t i = 0;
    for (const auto byte : digest) {
        out.sha256[i++] = hex[byte >> 4];
        out.sha256[i++] = hex[byte & 15];
    }
    out.file_size = total;
    out.recognized_research_build = is_research_build(out);
    return NIMBY_OK;
}
}
