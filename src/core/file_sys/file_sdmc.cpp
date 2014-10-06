// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <sys/stat.h>

#include "common/common_types.h"
#include "common/file_util.h"

#include "core/file_sys/file_sdmc.h"
#include "core/file_sys/archive_sdmc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileSys namespace

namespace FileSys {

File_SDMC::File_SDMC(const Archive_SDMC* archive, const std::string& path, const Mode mode) {
    // TODO(Link Mauve): normalize path into an absolute path without "..", it can currently bypass
    // the root directory we set while opening the archive.
    // For example, opening /../../etc/passwd can give the emulated program your users list.
    std::string real_path = archive->GetMountPoint() + path;

    if (!mode.create_flag && !FileUtil::Exists(real_path)) {
        file = nullptr;
        return;
    }

    std::string mode_string;
    if (mode.read_flag)
        mode_string += "r";
    if (mode.write_flag)
        mode_string += "w";

    file = new FileUtil::IOFile(real_path, mode_string.c_str());
}

File_SDMC::~File_SDMC() {
    Close();
}

/**
 * Read data from the file
 * @param offset Offset in bytes to start reading data from
 * @param length Length in bytes of data to read from file
 * @param buffer Buffer to read data into
 * @return Number of bytes read
 */
size_t File_SDMC::Read(const u64 offset, const u32 length, u8* buffer) const {
    file->Seek(offset, SEEK_SET);
    return file->ReadBytes(buffer, length);
}

/**
 * Write data to the file
 * @param offset Offset in bytes to start writing data to
 * @param length Length in bytes of data to write to file
 * @param flush The flush parameters (0 == do not flush)
 * @param buffer Buffer to read data from
 * @return Number of bytes written
 */
size_t File_SDMC::Write(const u64 offset, const u32 length, const u32 flush, const u8* buffer) const {
    file->Seek(offset, SEEK_SET);
    size_t written = file->WriteBytes(buffer, length);
    if (flush)
        file->Flush();
    return written;
}

/**
 * Get the size of the file in bytes
 * @return Size of the file in bytes
 */
size_t File_SDMC::GetSize() const {
    return static_cast<size_t>(file->GetSize());
}

/**
 * Set the size of the file in bytes
 * @param size New size of the file
 * @return true if successful
 */
bool File_SDMC::SetSize(const u64 size) const {
    file->Resize(size);
    file->Flush();
    return true;
}

/**
 * Close the file
 * @return true if the file closed correctly
 */
bool File_SDMC::Close() const {
    return file->Close();
}

} // namespace FileSys
