//
// Created by mdupont on 07/05/19.
//



#include "catch.hpp"
#include "TreeFileManager.hh"
#include "helpers.hh"


TEST_CASE("Add two same file", "[OutputTreeFileManager]") {
  OutputTreeFileManager ho;
  ho.add_file("/tmp/z.root", "root");
  ho.add_file("/tmp/z.root", "npy");
}

TEST_CASE("Save file with manager", "[OutputTreeFileManager]") {

  remove("/tmp/z.root");
  remove("/tmp/z.npy");
  remove("/tmp/z.txt");


  OutputTreeFileManager ho;
  ho.add_file("/tmp/z.root", "root");
  ho.add_file("/tmp/z.npy", "npy");

  char name[32] = "wxcvb";
  ho.write_variable("name", name, 32);
  ho.write_header();

}