//
// Created by mdupont on 07/05/19.
//



#include "catch.hpp"
#include "TreeFileManager.hh"
#include "helpers.hh"
#include "FileExceptions.hh"
#include "string.h"

using namespace std;


TEST_CASE("Load file with manager, test set_file, wrong type", "[InputTreeFileManager]") {
  helpers::remove_file("/tmp/whatever.plop");
  InputTreeFileManager h;
  REQUIRE_THROWS_AS(h.set_file("/tmp/whatever.plop", "plop"), UnknownKindManagerException);
}

TEST_CASE("Load file with manager, test set_file, good type, no file", "[InputTreeFileManager]") {
  auto kind = GENERATE(as<std::string>(), "npy", "root", "txt");
  string path = "/tmp/whatever." + kind;
  helpers::remove_file(path);
  InputTreeFileManager h;
  REQUIRE_THROWS_AS(h.set_file(path, kind), std::ios::failure);
  REQUIRE_NOTHROW(h.close());
}

TEST_CASE("Load file with manager, header npy root txt (double, string)", "[InputTreeFileManager]") {

  cout << "####start test = " << endl;
  auto kind = GENERATE(as<std::string>(), "npy", "root", "txt");
  string path = "/tmp/z." + kind;
  cout << "i = " << kind <<  " path = " << path << endl;


  helpers::remove_file("/tmp/z.root");
  helpers::remove_file("/tmp/z.npy");
  helpers::remove_file("/tmp/z.txt");


  OutputTreeFileManager ho;
  ho.add_file("/tmp/z.root", "root");
  ho.add_file("/tmp/z.npy", "npy");
  ho.add_file("/tmp/z.txt", "txt");

  double d;
  ho.write_variable("double", &d);

  string s;
  ho.write_variable("string", &s, 32);

  char cc[16];
  ho.write_variable("char*", cc, 16);


  ho.write_header();
  d = 2.3;
  s = "plop";
  strncpy(cc, "bouf", 16);




  ho.fill();
  ho.close();


  REQUIRE(helpers::file_exists(path));


  SECTION("set file")
  {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
  }

  SECTION("read header") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
  }

  SECTION("no read header, read_variable") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    double a;
    REQUIRE_THROWS_AS(h.read_variable("double", &a), std::logic_error);
  }

  SECTION("no read header, data_to_read") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_THROWS_AS(h.data_to_read(), std::logic_error);
  }

  SECTION("no read header, has_variable") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_THROWS_AS(h.has_variable("nmae"), std::logic_error);
  }

  SECTION("no read header, get_type_of_variable") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_THROWS_AS(h.get_type_of_variable("nmae"), std::logic_error);
  }

  SECTION("check nb_elements") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.nb_elements() == 1);
  }

  SECTION("wrong key") {
    REQUIRE(helpers::file_exists(path));
    cout << "\tSECTION (wrong key) i = " << kind <<  " path = " << path << endl;
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    double a;
    REQUIRE_THROWS_AS(h.read_variable("int", &a), KeyNotFoundInHeaderException);
  }

  SECTION("check has variable") {
    REQUIRE(helpers::file_exists(path));
    cout << "\tSECTION (check has variable) i = " << kind <<  " path = " << path << endl;
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.has_variable("string"));
    REQUIRE(h.has_variable("double"));
    REQUIRE_FALSE(h.has_variable("wrong"));
    REQUIRE_NOTHROW(h.close());
  }



  if(kind != "txt")
  {

    SECTION("check kind") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (check kind) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE(h.get_type_of_variable("double") == typeid(double));
      REQUIRE(h.get_type_of_variable("double") != typeid(double*));
      REQUIRE_NOTHROW(h.close());
    }

    SECTION("wrong kind for double") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (wrong key) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      int a;
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE_THROWS_AS(h.read_variable("double", &a), TypeMismatchHeaderException);
      string f;
      REQUIRE_THROWS_AS(h.read_variable("double", &f), TypeMismatchHeaderException);
      char c[32];
      REQUIRE_THROWS_AS(h.read_variable("double", c), TypeMismatchHeaderException);
    }

    SECTION("wrong kind for string, use int") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (wrong key) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      int a;
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE_THROWS_AS(h.read_variable("string", &a), TypeMismatchHeaderException);
      REQUIRE_THROWS_AS(h.read_variable("char*", &a), TypeMismatchHeaderException);
    }

    SECTION("good kind for string, use char[32]") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (wrong key) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      char a[32];
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE_NOTHROW(h.read_variable("string", a, 32));
      REQUIRE(h.data_to_read());
      REQUIRE_NOTHROW(h.read_next_entrie());
      REQUIRE_FALSE(h.data_to_read());
      REQUIRE(a == s);
    }

    SECTION("good kind for string, use char[]") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (wrong key) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      char a[32];
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE_NOTHROW(h.read_variable("string", a));
      REQUIRE(h.data_to_read());
      REQUIRE_NOTHROW(h.read_next_entrie());
      REQUIRE_FALSE(h.data_to_read());
      REQUIRE(a == s);
    }
  }

  if(kind == "npy")
  {
    SECTION("check kind") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (check kind) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE(h.get_type_of_variable("string") == typeid(string));
      REQUIRE(h.get_type_of_variable("char*") == typeid(string));
      REQUIRE_NOTHROW(h.close());
    }
  }
  else
  {
    SECTION("check kind") {
      REQUIRE(helpers::file_exists(path));
      cout << "\tSECTION (check kind) i = " << kind <<  " path = " << path << endl;
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE_THROWS_AS(h.get_type_of_variable("string") == typeid(string), NoTypeInHeaderException);
      REQUIRE_THROWS_AS(h.get_type_of_variable("char*") == typeid(string), NoTypeInHeaderException);
      REQUIRE_NOTHROW(h.close());
    }
  }


  SECTION("check double") {
    REQUIRE(helpers::file_exists(path));
    cout << "\tSECTION (check value) i = " << kind <<  " path = " << path << endl;
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    double a;
    REQUIRE_NOTHROW(h.read_variable("double", &a));
    REQUIRE(h.data_to_read());
    REQUIRE_NOTHROW(h.read_next_entrie());
    REQUIRE_FALSE(h.data_to_read());
    REQUIRE(a == d);
    REQUIRE_NOTHROW(h.close());
  }

  SECTION("check value string") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    string a;
    REQUIRE_NOTHROW(h.read_variable("string", &a));
    REQUIRE(h.data_to_read());
    REQUIRE_NOTHROW(h.read_next_entrie());
    REQUIRE_FALSE(h.data_to_read());
    REQUIRE(a == s);
    REQUIRE_NOTHROW(h.close());
  }

}


TEST_CASE("Load file with manager, number of element, pick random", "[InputTreeFileManager]")
{

  cout << "####start test = " << endl;
  auto kind = GENERATE(as<std::string>(), "npy", "root", "txt");
  string path = "/tmp/z." + kind;
  cout << "i = " << kind << " path = " << path << endl;


  helpers::remove_file("/tmp/z.root");
  helpers::remove_file("/tmp/z.npy");
  helpers::remove_file("/tmp/z.txt");


  OutputTreeFileManager ho;
  ho.add_file("/tmp/z.root", "root");
  ho.add_file("/tmp/z.npy", "npy");
  ho.add_file("/tmp/z.txt", "txt");

  double d;
  ho.write_variable("double", &d);

  string s;
  ho.write_variable("string", &s, 32);

  ho.write_header();

  d = 1.0;
  s = "one";
  ho.fill();

  d = 2.0;
  s = "two";
  ho.fill();
  ho.close();

  SECTION("check nb_elements") {
    REQUIRE(helpers::file_exists(path));
    InputTreeFileManager h;
    REQUIRE_NOTHROW(h.set_file(path, kind));
    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.nb_elements() == 2);
  }

//  if(kind == "txt")
  {
    SECTION("check value") {
      REQUIRE(helpers::file_exists(path));
      InputTreeFileManager h;
      REQUIRE_NOTHROW(h.set_file(path, kind));
      REQUIRE_NOTHROW(h.read_header());
      REQUIRE(h.nb_elements() == 2);
      double a = -1.0;
      REQUIRE_NOTHROW(h.read_variable("double", &a));
      string f = "plop";
      REQUIRE_NOTHROW(h.read_variable("string", &f));
      REQUIRE_NOTHROW(h.read_entrie(0));
      REQUIRE(a == 1.0);
      REQUIRE(f == "one");
      REQUIRE_NOTHROW(h.read_entrie(1));
      REQUIRE(a == 2.0);
      REQUIRE(f == "two");
      REQUIRE_NOTHROW(h.read_entrie(0));
      REQUIRE(a == 1.0);
      REQUIRE(f == "one");
    }
  }

  REQUIRE(helpers::file_exists(path));
}


TEST_CASE("Load file with chain", "[InputTreeFileChain]")
{
  helpers::remove_file("/tmp/z1.root");
  helpers::remove_file("/tmp/z1.npy");
  helpers::remove_file("/tmp/z1.txt");

  helpers::remove_file("/tmp/z2.root");
  helpers::remove_file("/tmp/z2.npy");
  helpers::remove_file("/tmp/z2.txt");


  {
    OutputTreeFileManager ho;
    ho.add_file("/tmp/z1.root", "root");
    ho.add_file("/tmp/z1.npy", "npy");
    ho.add_file("/tmp/z1.txt", "txt");

    double d;
    ho.write_variable("double", &d);
    string s;
    ho.write_variable("string", &s, 32);
    ho.write_header();
    d = 1.0;
    s = "one";
    ho.fill();
    d = 2.0;
    s = "two";
    ho.fill();
    ho.close();
  }

  {
    OutputTreeFileManager ho;
    ho.add_file("/tmp/z2.root", "root");
    ho.add_file("/tmp/z2.npy", "npy");
    ho.add_file("/tmp/z2.txt", "txt");

    double d;
    ho.write_variable("double", &d);
    string s;
    ho.write_variable("string", &s, 32);
    ho.write_header();
    d = 3.0;
    s = "three";
    ho.fill();
    d = 4.0;
    s = "four";
    ho.fill();
    ho.close();
  }

  SECTION("check nb_elements one file") {
    InputTreeFileChain h;
    REQUIRE_NOTHROW(h.add_file("/tmp/z1.root", "root"));
    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.nb_elements() == 2);
    double a = -1.0;
    REQUIRE_NOTHROW(h.read_variable("double", &a));
    string f = "plop";
    REQUIRE_NOTHROW(h.read_variable("string", &f));
    REQUIRE_NOTHROW(h.read_entrie(0));
    REQUIRE(a == 1.0);
    REQUIRE(f == "one");
    REQUIRE_NOTHROW(h.read_entrie(1));
    REQUIRE(a == 2.0);
    REQUIRE(f == "two");
    REQUIRE_NOTHROW(h.read_entrie(0));
    REQUIRE(a == 1.0);
    REQUIRE(f == "one");
    REQUIRE_NOTHROW(h.close());
  }

  SECTION("check nb_elements two files") {
    InputTreeFileChain h;
    REQUIRE_NOTHROW(h.add_file("/tmp/z1.root", "root"));
    REQUIRE_NOTHROW(h.add_file("/tmp/z2.root", "root"));
    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.nb_elements() == 4);
    double a = -1.0;
    REQUIRE_NOTHROW(h.read_variable("double", &a));
    string f = "plop";
    REQUIRE_NOTHROW(h.read_variable("string", &f));
    REQUIRE_NOTHROW(h.read_entrie(0));
    REQUIRE(a == 1.0);
    REQUIRE(f == "one");
    REQUIRE_NOTHROW(h.read_entrie(1));
    REQUIRE(a == 2.0);
    REQUIRE(f == "two");
    REQUIRE_NOTHROW(h.read_entrie(0));
    REQUIRE(a == 1.0);
    REQUIRE(f == "one");
    REQUIRE_NOTHROW(h.read_entrie(2));
    REQUIRE(a == 3.0);
    REQUIRE(f == "three");
    REQUIRE_NOTHROW(h.read_entrie(3));
    REQUIRE(a == 4.0);
    REQUIRE(f == "four");
    REQUIRE_NOTHROW(h.close());
  }

  SECTION("check nb_elements six files") {
    InputTreeFileChain h;
    REQUIRE_NOTHROW(h.add_file("/tmp/z1.root", "root"));
    REQUIRE_NOTHROW(h.add_file("/tmp/z2.root", "root"));

    REQUIRE_NOTHROW(h.add_file("/tmp/z1.npy", "npy"));
    REQUIRE_NOTHROW(h.add_file("/tmp/z2.npy", "npy"));

    REQUIRE_NOTHROW(h.add_file("/tmp/z1.txt", "txt"));
    REQUIRE_NOTHROW(h.add_file("/tmp/z2.txt", "txt"));


    REQUIRE_NOTHROW(h.read_header());
    REQUIRE(h.nb_elements() == 2*6);
    double a = -1.0;
    REQUIRE_NOTHROW(h.read_variable("double", &a));
    string f = "plop";
    REQUIRE_NOTHROW(h.read_variable("string", &f));

    for(int i = 0; i < 3; ++i)
    {
      REQUIRE_NOTHROW(h.read_entrie(i*4));
      REQUIRE(a == 1.0);
      REQUIRE(f == "one");
      REQUIRE_NOTHROW(h.read_entrie(i*4+1));
      REQUIRE(a == 2.0);
      REQUIRE(f == "two");
      REQUIRE_NOTHROW(h.read_entrie(0));
      REQUIRE(a == 1.0);
      REQUIRE(f == "one");
      REQUIRE_NOTHROW(h.read_entrie(i*4+2));
      REQUIRE(a == 3.0);
      REQUIRE(f == "three");
      REQUIRE_NOTHROW(h.read_entrie(i*4+3));
      REQUIRE(a == 4.0);
      REQUIRE(f == "four");

    }


    REQUIRE_NOTHROW(h.close());
  }




  helpers::remove_file("/tmp/z1.root");
  helpers::remove_file("/tmp/z1.npy");
  helpers::remove_file("/tmp/z1.txt");

  helpers::remove_file("/tmp/z2.root");
  helpers::remove_file("/tmp/z2.npy");
  helpers::remove_file("/tmp/z2.txt");
}







