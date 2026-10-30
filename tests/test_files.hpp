///
/// \file       test_files.hpp
/// \brief      Temporary files for tests that read or write paths.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_TEST_FILES_HPP
#define GRAPH_MATCHING_TEST_FILES_HPP

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace graph_matching::testing {

///
/// \brief      A file in the system temporary directory, removed on destruction.
///
class temporary_file {
public:
    ///
    /// \brief      Reserve a temporary path, optionally writing contents to it.
    ///
    /// \param      name        File name within the temporary directory.
    /// \param      contents    Text to write; nothing is written if empty.
    ///
    explicit temporary_file(std::string_view name, std::string_view contents = {})
        : m_path(std::filesystem::temp_directory_path() / name) {
        if (!contents.empty()) {
            std::ofstream(m_path) << contents;
        }
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
    /// \return     The file's path, as a C string.
    ///
    [[nodiscard]] const char *c_str() const { return m_path.c_str(); }

private:
    std::filesystem::path m_path;
};

} // namespace graph_matching::testing

#endif // GRAPH_MATCHING_TEST_FILES_HPP
