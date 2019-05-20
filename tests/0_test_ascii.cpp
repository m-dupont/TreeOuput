//
// Created by mdupont on 02/04/19.
//

#include <iomanip>


#include <string>
#include <cstring>


#include "catch.hpp"
#include "impl/AsciiFile.hh"
#include "helpers.hh"
#include "FileExceptions.hh"

TEST_CASE("OutputAsciiTreeFile, Open output ascii file", "[OutputAsciiTreeFile:open_output_file]")
{
    std::string path = "/tmp/z.txt";
    helpers::remove_file(path);
    REQUIRE_FALSE(helpers::file_exists(path));

    SECTION("open_file char* name")
    {
        OutputAsciiTreeFile h;
        REQUIRE_FALSE(h.is_open());
        REQUIRE_NOTHROW(h.open(path.c_str()));
        REQUIRE(h.is_open());
        REQUIRE_NOTHROW(h.close());
        REQUIRE(helpers::file_exists(path));
    }


    SECTION("open_file string name")
    {
        OutputAsciiTreeFile h;
        REQUIRE_FALSE(h.is_open());
        REQUIRE_NOTHROW(h.open(path));
        REQUIRE(h.is_open());
        REQUIRE_NOTHROW(h.close());
        REQUIRE(helpers::file_exists(path));
    }

    SECTION("permission denied")
    {
        std::string path = "/z.txt";
        OutputAsciiTreeFile h;
        REQUIRE_FALSE(helpers::file_exists(path));
        REQUIRE_THROWS_AS( h.open(path), std::ios_base::failure)  ;
        REQUIRE_NOTHROW(h.close());
    }


    helpers::remove_file(path);
}


TEST_CASE("OutputAsciiTreeFile, write empty header", "[OutputAsciiTreeFile:write_empty_header]")
{
    std::string path = "/tmp/z.txt";
    helpers::remove_file(path);
    REQUIRE_FALSE(helpers::file_exists(path));

    OutputAsciiTreeFile h;
    REQUIRE_FALSE(h.is_open());
    REQUIRE_NOTHROW(h.open(path));
    REQUIRE(h.is_open());

    SECTION("write_header normal")
    {
        REQUIRE(h.is_open());
        REQUIRE_NOTHROW(h.write_header());
        REQUIRE_NOTHROW(h.close());
    }

    SECTION("write_header closed file")
    {
        REQUIRE(h.is_open());
        REQUIRE_NOTHROW(h.close());
        REQUIRE_FALSE(h.is_open());
        REQUIRE_THROWS_AS(h.write_header(), ClosedFileException);
    }

    helpers::remove_file(path);
}

TEST_CASE("InputAsciiTreeFile, Open input non existing ascii file", "[InputAsciiTreeFile:open_input_non_existing_file]")
{
    std::string path = "/tmp/z.txt";
    helpers::remove_file(path);
    REQUIRE_FALSE(helpers::file_exists(path));
    InputAsciiTreeFile h;
    REQUIRE_THROWS_AS( h.open(path), std::ios_base::failure) ;
    REQUIRE_FALSE(h.is_open());
    REQUIRE_THROWS_AS(h.read_header(), ClosedFileException);
    REQUIRE_NOTHROW(h.close());
    REQUIRE_FALSE(h.is_open());
}

TEST_CASE("InputAsciiTreeFile, Open input empty ascii file", "[InputAsciiTreeFile:open_input_empty_file]")
{
    std::string path = "/tmp/z.txt";
    helpers::remove_file(path);

    OutputAsciiTreeFile ho;
    REQUIRE_FALSE(ho.is_open());
    REQUIRE_NOTHROW(ho.open(path.c_str()));
    REQUIRE(ho.is_open());


    SECTION("read not existing header")
    {
        ho.close();
        REQUIRE(helpers::file_exists(path));
        InputAsciiTreeFile hi;
        REQUIRE_NOTHROW(hi.open(path));
        REQUIRE(hi.is_open());
        REQUIRE_THROWS_AS(hi.read_header(), MissingHeaderException);
        REQUIRE_NOTHROW(hi.close());
    }

    SECTION("read empty but existing header")
    {
        ho.write_header();
        ho.close();
        InputAsciiTreeFile hi;
        REQUIRE_NOTHROW(hi.open(path));
        REQUIRE(hi.is_open());
        REQUIRE_THROWS_AS(hi.read_header(), MalFormedHeaderException);
        REQUIRE_NOTHROW(hi.close());
    }

    helpers::remove_file(path);
}