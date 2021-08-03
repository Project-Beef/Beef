#include "io.hh"

#include <stdint.h>

#include <string_view>
#include <filesystem>

#ifdef _WIN32
    #include <Windows.h>
#elif __LINUX__
    #include <unistd.h>
#else
    #error "Unsupported platform"
#endif

namespace beef::io
{
    void* read_all_bytes(const std::string_view& filename, uintmax_t& filesize) noexcept
    {
        filesize = std::filesystem::file_size(filename);
        void* data = malloc(filesize);

        if (!data)
            return nullptr;

        FILE* fp = nullptr;
#ifdef _WIN32
        fopen_s(&fp, filename.data(), "rb");
#else
        fp = fopen(filename.data(), "rb");
#endif

        if (!fp)
            return nullptr;

        fread(data, 1, filesize, fp);
        fclose(fp);

        return data;
    }

    void write_all_bytes(const std::string_view& filename, const void* data, uintmax_t data_size) noexcept
    {
        FILE* fp = nullptr;
#ifdef _WIN32
        fopen_s(&fp, filename.data(), "wb");
#else
        fp = fopen(filename.data(), "wb");
#endif

        if (!fp) 
            return;

        fwrite(data, 1, data_size, fp);
        fflush(fp);
        fclose(fp);
    }


    bool create_symlink(const char* linkName, const char* file) noexcept
    {
#ifdef _WIN32
        BOOLEAN status = CreateSymbolicLinkA(linkName, file, NULL);
        return status != FALSE;
#elif __LINUX
        int status = symlink(file, linkName);
        return status == 0;
#endif

        return false;
    }

    bool create_link(const char* linkName, const char* file) noexcept
    {
#ifdef _WIN32
        BOOLEAN status = CreateHardLinkA(linkName, file, NULL);
        return status != FALSE;
#elif __LINUX
        int status = link(file, linkName);
        return status == 0;
#endif

        return false;
    }
}
