//
// Created by mdupont on 03/04/19.
//


#include <fstream>
#include <stdio.h>

#include "helpers.hh"

using namespace std;
using namespace helpers;


bool helpers::file_exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
bool helpers::remove_file(const std::string &name)
{
    return remove(name.c_str());
}
