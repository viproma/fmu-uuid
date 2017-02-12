// Copyright 2016-2017, SINTEF Ocean.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   1. Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cctype>
#include <fstream>
#include <functional>
#include <istream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

// Application name
#define MY_NAME "fmu-uuid"

// The UUID namespace (this was just generated randomly)
#define MY_UUID_NAMESPACE "23d0734c-3983-4f65-a8f7-3bd5c5693546"


// The program's main function.
void Run(
    const std::string& inputModelDescription,
    const std::string& placeholder,
    const std::string& outputModelDescription,
    const std::string& outputHeader,
    const std::string& variableName,
    std::function<void(const std::string&)> warn);

// Copies the entire contents of a file into a string.
std::string ReadFile(const std::string& path);

// Writes the contents of `string` into a file, replacing any previous content.
void WriteFile(const std::string& path, const std::string& string);

// Copies all data from `stream` into a string.
std::string Slurp(std::istream& stream);

// Returns a copy of `string` with all whitespace removed.
std::string NoWhitespace(const std::string& string);

// Returns a copy of `haystack` where all occurrences of `needle` have
// been replaced with `replacement`.
std::string Replace(
    const std::string& haystack,
    const std::string& needle,
    const std::string& replacement);

int main(int argc, const char* const argv[])
{
    try {
        if (argc < 2) {
            std::cout <<
                "Generates an UUID for an FMU.\n"
                "Usage:\n"
                "  " MY_NAME " <input md.> <placeholder> <output md.> <output hdr.> <macro name>\n"
                "Arguments:\n"
                "  input md.   = Path to model description file for which to generate UUID.\n"
                "  placeholder = String in input md. to replace with UUID in output md.\n"
                "  output md.  = Path of output model description file.\n"
                "  output hdr. = Path of output C header file that defines macro with UUID.\n"
                "  macro name  = A name for the macro that contains the UUID string.\n"
                << std::flush;
        } else if (argc != 6) {
            throw std::runtime_error("Wrong number of command-line arguments.");
        } else {
            Run(argv[1], argv[2], argv[3], argv[4], argv[5],
                [] (const std::string& s) { std::cerr << "Warning: " << s << std::endl; });
        }
    } catch (const std::logic_error& e) {
        std::cerr << "Internal error: " << e.what() << std::endl;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}


void Run(
    const std::string& inputModelDescription,
    const std::string& placeholder,
    const std::string& outputModelDescription,
    const std::string& outputHeader,
    const std::string& variableName,
    std::function<void(const std::string&)> warn)
{
    const auto input = ReadFile(inputModelDescription);

    const auto uuidNamespace = boost::uuids::string_generator{}(MY_UUID_NAMESPACE);
    const auto uuid = boost::uuids::name_generator{uuidNamespace}(NoWhitespace(input));
    const auto uuidString = boost::uuids::to_string(uuid);

    const auto output = Replace(input, placeholder, uuidString);
    if (output == input) {
        warn("Placeholder \"" + placeholder + "\" not present in input file \""
            + inputModelDescription + '"');
    }
    WriteFile(outputModelDescription, output);

    const auto headerContents =
        "#ifndef " + variableName + "\n"
        "#define " + variableName + " \"" + uuidString + "\"\n"
        "#endif\n";
    WriteFile(outputHeader, headerContents);
}


std::string ReadFile(const std::string& path)
{
    auto stream = std::ifstream{path, std::ios_base::binary};
    if (!stream) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return Slurp(stream);
}


void WriteFile(const std::string& path, const std::string& string)
{
    auto stream = std::ofstream{path, std::ios_base::binary};
    if (!stream) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    stream << string;
}


std::string Slurp(std::istream& stream)
{
    std::stringstream sstream;
    sstream << stream.rdbuf();
    return sstream.str();
}


std::string NoWhitespace(const std::string& string)
{
    auto output = std::string{};
    output.reserve(string.size());
    for (const char c : string) {
        if (!std::isspace(c)) output += c;
    }
    return output;
}


std::string Replace(
    const std::string& haystack,
    const std::string& needle,
    const std::string& replacement)
{
    auto output = std::string{};
    output.reserve(haystack.size());
    std::string::size_type i = 0;
    while (true) {
        const auto j = haystack.find(needle, i);
        output.append(haystack, i, j-i);
        if (j == std::string::npos) break;
        output.append(replacement);
        i = j + needle.size();
    }
    return output;
}
