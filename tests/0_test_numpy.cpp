//
// Created by mdupont on 22/12/18.
//

#include <iomanip>
#include <stdio.h>
#include <string>
#include <cstring>

#include "catch.hpp"

#include "impl/NumpyFile.hh"
#include "helpers.hh"





TEST_CASE("OutputNumpyTreeFile, Open numpy file", "OutputNumpyTreeFile") {

    remove("/tmp/z.npy");


    OutputNumpyTreeFile h;
    REQUIRE_FALSE(h.is_open());
    h.open("/tmp/z.npy");
    REQUIRE(h.is_open());
    h.close();
    REQUIRE_FALSE(h.is_open());

    remove("/tmp/z.npy");
}

TEST_CASE("InputNumpyTreeFile, Open numpy file", "OutputNumpyTreeFile") {

    remove("/tmp/z.npy");

    OutputNumpyTreeFile ho;
    ho.open("/tmp/z.npy");
    REQUIRE(ho.is_open());
    ho.close();

    InputNumpyTreeFile h;
    REQUIRE_FALSE(h.is_open());
    REQUIRE_NOTHROW(h.open("/tmp/z.npy"));
    REQUIRE(h.is_open());
    h.close();
    REQUIRE_FALSE(h.is_open());

    remove("/tmp/z.npy");
}

TEST_CASE("InputNumpyTreeFile, Open numpy file does not exist", "InputNumpyTreeFile") {

    remove("/tmp/z.npy") ;

    InputNumpyTreeFile h;
    REQUIRE_FALSE(h.is_open());
    REQUIRE_THROWS_AS(h.open("/tmp/z.npy"), std::ios_base::failure);
    REQUIRE_FALSE(h.is_open());
    REQUIRE_NOTHROW(h.close());
    REQUIRE_FALSE(h.is_open());

    remove("/tmp/z.npy");
}

TEST_CASE("OutputNumpyTreeFile, Can not open numpy file, permission denied", "OutputNumpyTreeFile") {
    OutputNumpyTreeFile h;
    REQUIRE_FALSE(h.is_open());
    REQUIRE_THROWS_AS( h.open("/z.npy"), std::ios_base::failure)  ;
    REQUIRE_FALSE(h.is_open());
    h.close();
    REQUIRE_FALSE(h.is_open());
}


TEST_CASE("OutputNumpyTreeFile, writeheader", "OutputNumpyTreeFile") {

    OutputNumpyTreeFile h;
    REQUIRE_NOTHROW(h.open("/tmp/z.npy"));
    SECTION("writeheader empty header")
    {
       REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable double header")
    {
        double d;
        h.write_variable("d", &d);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable float header")
    {
        float d;
        h.write_variable("d", &d);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable bool header")
    {
        bool d;
        h.write_variable("d", &d);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable string")
    {
        std::string s;
        REQUIRE_THROWS_AS(h.write_variable("string", &s), std::invalid_argument);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable string correct")
    {
        std::string s;
        REQUIRE_NOTHROW(h.write_variable("string", &s, 10));
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable string correct, len = 0 ")
    {
        std::string s;
        REQUIRE_THROWS_AS(h.write_variable("string", &s, 0), std::out_of_range);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable char*")
    {
        char s[] = "plop";
        REQUIRE_THROWS_AS(h.write_variable("string", &s), std::invalid_argument);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable char* correct, len = 0 ")
    {
        char s[] = "plop";
        REQUIRE_THROWS_AS(h.write_variable("string", s, 0), std::out_of_range);
        REQUIRE_NOTHROW(h.write_header());
    }

    SECTION("write_variable char* correct")
    {
        char s[] = "plop";
        REQUIRE_NOTHROW(h.write_variable("string", s, strlen(s)));
        REQUIRE_NOTHROW(h.write_header());
    }
    h.close();
}

TEST_CASE("OutputNumpyTreeFile, fill", "OutputNumpyTreeFile") {

    OutputNumpyTreeFile h;
    REQUIRE_NOTHROW(h.open("/tmp/z.npy"));

    SECTION("fill whitout call write_header")
    {
        REQUIRE_THROWS_AS(h.fill(), std::logic_error);
        REQUIRE(h.nb_elements() == 0);
    }

    SECTION("fill empty")
    {
        REQUIRE_NOTHROW(h.write_header());
        REQUIRE_NOTHROW(h.fill());
        REQUIRE(h.nb_elements() == 0);
    }

    SECTION("fill double header")
    {
        double d;
        REQUIRE_NOTHROW(h.write_variable("d", &d));
        REQUIRE_NOTHROW(h.write_header());
        REQUIRE_NOTHROW(h.fill());
        REQUIRE(h.nb_elements() == 1);
    }

    SECTION("fill double int header")
    {
        double d;
        REQUIRE_NOTHROW(h.write_variable("d", &d));
        double i;
        REQUIRE_NOTHROW(h.write_variable("i", &i));
        REQUIRE_NOTHROW(h.write_header());
        REQUIRE_NOTHROW(h.fill());
        REQUIRE(h.nb_elements() == 1);
    }

    SECTION("fill double int header")
    {
        double d;
        REQUIRE_NOTHROW(h.write_variable("d", &d));
        int i;
        REQUIRE_NOTHROW(h.write_variable("i", &i));
        REQUIRE_NOTHROW(h.write_header());
        int j;
        for (j = 0; j < 10; ++j)
        {
            REQUIRE_NOTHROW(h.fill());
            REQUIRE(h.nb_elements() == j+1);
        }
        REQUIRE(h.nb_elements() == 10);
    }

    SECTION("fill int header")
    {
        std::string s;
        REQUIRE_NOTHROW(h.write_variable("s", &s, 10));
        REQUIRE_NOTHROW(h.write_header());
        int j;
        for (j = 0; j < 10; ++j)
        {
            s = std::to_string(j);
            REQUIRE_NOTHROW(h.fill());
            REQUIRE(h.nb_elements() == j+1);
        }
        REQUIRE(h.nb_elements() == 10);
    }
    h.close();
}


TEST_CASE("InputNumpyTreeFile, fill & read", "InputNumpyTreeFile") {



//    remove("/tmp/z.npy");

    SECTION("open file")
    {

        OutputNumpyTreeFile ho;
        ho.open("/tmp/z.npy");
        ho.close();

        InputNumpyTreeFile hi;
        REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
        REQUIRE_NOTHROW(hi.close());
    }

    SECTION("open file")
    {
        OutputNumpyTreeFile ho;
        ho.open("/tmp/z.npy");
        ho.close();

        InputNumpyTreeFile hi;
        REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
        REQUIRE_THROWS_AS(hi.read_header(), std::runtime_error);
        REQUIRE_NOTHROW(hi.close());
    }

    SECTION("open file")
    {
        OutputNumpyTreeFile ho;
        ho.open("/tmp/z.npy");
        ho.write_header();
        ho.close();

        InputNumpyTreeFile hi;
        REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
        REQUIRE_THROWS_AS(hi.read_header(), std::runtime_error);
        REQUIRE_NOTHROW(hi.close());
    }

}


TEST_CASE("InputNumpyTreeFile, read data from empty file containing double" , "[InputNumpyTreeFile:read_empty_double]") {
  OutputNumpyTreeFile h;
  REQUIRE_NOTHROW(h.open("/tmp/z.npy"));
  double d = 2;
  REQUIRE_NOTHROW(h.write_variable("d", &d));
  REQUIRE_NOTHROW(h.write_header());
  h.close();

  SECTION("read double from file containing double")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    double dd = -1;
    REQUIRE_NOTHROW(hi.read_variable("d", &dd));
    REQUIRE(hi.nb_elements() == 0);
    REQUIRE(hi.data_to_read() == false);
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE(hi.data_to_read() == false);
    REQUIRE(dd == -1);
  }

  SECTION("read int from file containing double")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    int dd = -1;
    REQUIRE_THROWS_AS(hi.read_variable("d", &dd), std::runtime_error);
    REQUIRE(hi.nb_elements() == 0);
    REQUIRE(hi.data_to_read() == false);
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE(hi.data_to_read() == false);
    REQUIRE(dd == -1);
  }

}


TEST_CASE("InputNumpyTreeFile, read data from file containing one double", "[InputNumpyTreeFile:read_one_double]") {
  OutputNumpyTreeFile h;
  REQUIRE_NOTHROW(h.open("/tmp/z.npy"));
  double d = 2;
  REQUIRE_NOTHROW(h.write_variable("d", &d));
  REQUIRE_NOTHROW(h.write_header());
  REQUIRE_NOTHROW(h.fill());
  h.close();

  SECTION("read_next_entrie whitout call read_header")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_THROWS_AS(hi.read_next_entrie(), std::logic_error);
    REQUIRE(hi.nb_elements() == 0);
  }

  SECTION("read double from file containing double")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    double dd = -1;
    REQUIRE_NOTHROW(hi.read_variable("d", &dd));
    REQUIRE(hi.nb_elements() == 1);
    REQUIRE(hi.data_to_read() == true);
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE(hi.data_to_read() == false);
    REQUIRE(dd == d);
  }

  SECTION("read int from file containing double")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    int dd = -1;
    REQUIRE_THROWS_AS(hi.read_variable("d", &dd), std::runtime_error);
    REQUIRE(hi.nb_elements() == 1);
    REQUIRE(hi.data_to_read());
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE_FALSE(hi.data_to_read()); //we had only one entry to read
    REQUIRE(dd == -1);
  }

  remove("/tmp/z.npy");

}



TEST_CASE("InputNumpyTreeFile, read data from file containing one string", "[InputNumpyTreeFile:read_one_double]") {
  OutputNumpyTreeFile h;
  REQUIRE_NOTHROW(h.open("/tmp/z.npy"));
  std::string name = "plop";
  REQUIRE_NOTHROW(h.write_variable("name", &name, 10));
  REQUIRE_NOTHROW(h.write_header());
  REQUIRE_NOTHROW(h.fill());
  h.close();

  SECTION("read double from file containing string")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    double dd = -1;
    REQUIRE_THROWS_AS(hi.read_variable("d", &dd), std::runtime_error);
    REQUIRE_THROWS_AS(hi.read_variable("name", &dd), std::runtime_error);
    REQUIRE(hi.nb_elements() == 1);
    REQUIRE(hi.data_to_read());
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE_FALSE(hi.data_to_read());
    REQUIRE(dd == -1);
  }

  SECTION("read string from file containing string")
  {
    InputNumpyTreeFile hi;
    REQUIRE_NOTHROW(hi.open("/tmp/z.npy"));
    REQUIRE_NOTHROW(hi.read_header());

    std::string dd = "rien à voir";
    REQUIRE_NOTHROW(hi.read_variable("name", &dd));
    REQUIRE(hi.nb_elements() == 1);
    REQUIRE(hi.data_to_read());
    REQUIRE_NOTHROW(hi.read_next_entrie());
    REQUIRE_FALSE(hi.data_to_read()); //we had only one entry to read
    REQUIRE(dd == name);
  }

  remove("/tmp/z.npy");

}