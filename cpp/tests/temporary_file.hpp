///
/// \file       temporary_file.hpp
/// \brief      Temporary files for tests that read or write paths.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_CPP_TEMPORARY_FILE_HPP
#define GRAPH_MATCHING_CPP_TEMPORARY_FILE_HPP

#include <filesystem>
#include <fstream>
#include <string_view>
#include <system_error>

namespace graph_matching::testing {

///
/// \brief      A file in the system temporary directory, removed on destruction.
///
class temporary_file {
public:
    ///
    /// \brief      Write a file to the temporary directory.
    ///
    /// \param      name        File name within the temporary directory.
    /// \param      contents    Text to write.
    ///
    temporary_file(std::string_view name, std::string_view contents)
        : m_path(std::filesystem::temp_directory_path() / name) {
        std::ofstream(m_path) << contents;
    }

    temporary_file(const temporary_file &) = delete;
    temporary_file &operator=(const temporary_file &) = delete;
    temporary_file(temporary_file &&) = delete;
    temporary_file &operator=(temporary_file &&) = delete;

    ~temporary_file() {
        std::error_code ignored;
        std::filesystem::remove(m_path, ignored);
    }

    ///
    /// \return     The file's path.
    ///
    [[nodiscard]] const std::filesystem::path &path() const noexcept { return m_path; }

private:
    std::filesystem::path m_path; ///< Path of the file.
};

} // namespace graph_matching::testing

#endif // GRAPH_MATCHING_CPP_TEMPORARY_FILE_HPP
