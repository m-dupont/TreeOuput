//
// Created by mdupont on 12/03/18.
//

#include "TreeFile.hh"

#include <iostream>

using namespace std;

Tree::Tree()
{

  this->add_size<double>();

  this->add_name<double>("double");

  this->add_size<long double>();
  this->add_size<float>();
//  this->add_size<int>();
  this->add_size<uint8_t>();
  this->add_size<uint16_t>();
  this->add_size<uint32_t>();
  this->add_size<uint64_t>();

  this->add_size<int8_t>();
  this->add_size<int16_t>();
  this->add_size<int32_t>();
  this->add_size<int64_t>();

  this->add_size<bool>();
  this->add_size<char>();


}


std::string Tree::default_tree_name()
{
  return "tree";
}

const std::string Tree::type_to_name(std::type_index t_index)
{
    return m_tmapOfName[t_index];
}

void OutputTreeFile::set_tree_name(const std::string &name)
{
  m_nameOfTree = name;
}

OutputTreeFile::~OutputTreeFile()
{
//  cout << "~OutputTreeFile" << endl;
}

OutputTreeFile::OutputTreeFile()
{
  m_nameOfTree = Tree::default_tree_name();
}



InputTreeFile::InputTreeFile()
{
  m_nameOfTree = Tree::default_tree_name();
}

void InputTreeFile::set_tree_name(const std::string &name)
{
  m_nameOfTree = name;
}

void InputTreeFile::read_variable(const std::string &name, std::string *p)
{
  this->read_variable(name, p, typeid(p));
}

void InputTreeFile::read_variable(const std::string &name, char *p)
{
  this->read_variable(name, p, typeid(p));
}



void InputTreeFile::read_variable(const std::string &name, char *p, size_t)
{
  this->read_variable(name, p, typeid(p));
}


