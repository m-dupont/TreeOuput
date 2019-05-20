//
// Created by mdupont on 13/03/18.
//

#include "RootTreeFile.hh"
#include <sstream>
#include <iostream>
#include <fstream>

#include "TLeaf.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "TreeFileManager.hh"
#include "FileExceptions.hh"

using namespace std;


RootTree::RootTree() : Tree()
{

//  m_tmapOfDefinition[typeid(Char_t)] = "C";

  add_leaflist<Char_t>("B", "Char_t");
  add_leaflist<UChar_t>("b", "UChar_t");
  add_leaflist<Short_t>("S", "Short_t");
  add_leaflist<UShort_t>("s", "UShort_t");
  add_leaflist<Int_t>("I", "Int_t");
  add_leaflist<UInt_t>("i", "UInt_t");
  add_leaflist<Float_t>("F", "Float_t");
  add_leaflist<Double_t>("D", "Double_t");
  add_leaflist<Long64_t>("L", "Long64_t");
  add_leaflist<ULong64_t>("l", "ULong64_t");
  add_leaflist<Bool_t>("O", "Bool_t");

  add_leaflist<string>("C", "");
  add_leaflist<char*>("C", "");
}

void RootTree::register_variable(const std::string &name, const void *p, std::type_index t_index)
{
  void *pp = (void*)p;

  std::stringstream leaf_ss;
  auto b = name.find_first_of("[");
  leaf_ss << name.substr(0, b) << "/" << m_tmapOfDefinition.at(t_index);
  string leaf_s = leaf_ss.str();

//  cout << "RootTreeFile::register_variable name = " << name << " t_index = " << t_index.name() << " leaf_s = " << leaf_s << "\n";


  m_ttree->Branch(name.c_str(), pp, leaf_s.c_str());
//  m_ttree->Branch(name.c_str(), pp);

}

void RootTree::register_variable(const std::string&, const std::string*, size_t )
{

}

void RootTree::register_variable(const std::string &name, const char *p, size_t)
{
  register_variable(name, p, typeid(string));

}

void OutputRootTreeFile::open(const std::string& s)
{
  File::open(s.c_str(), ios_base::out);
  m_file = new TFile(s.c_str(), "RECREATE");
//  cout << "create tree name = " << m_nameOfTree << " from file " << endl;
  m_ttree = new TTree(m_nameOfTree.c_str(), m_nameOfTree.c_str());
}

void InputRootTreeFile::open(const std::string& s)
{
  File::open(s.c_str(), ios_base::in);
  m_file = new TFile(s.c_str(), "READ");
  if(m_file->IsZombie())
  {
    std::stringstream ss;
    ss << "Error opening file! '"  << s <<  "' : " << strerror(errno) ;
    throw std::ios::failure(ss.str());
  }



}

void RootTree::close()
{
  m_file->Close();
  delete m_file;
}

void OutputRootTreeFile::close()
{
  m_file->Write();
  RootTree::close();
}

void InputRootTreeFile::close()
{
  RootTree::close();
}


bool OutputRootTreeFile::is_open()
{
    return m_file->IsOpen();
}

bool InputRootTreeFile::is_open()
{
  return m_file->IsOpen();
}

void OutputRootTreeFile::fill()
{

  for(auto ss: m_mapConstStringToRootString)
  {
//    ss.second->assign(*ss.first);
    strcpy(ss.second, ss.first->data());
  }

  m_ttree->Fill();
}

void OutputRootTreeFile::write_header()
{

}

void OutputRootTreeFile::write_variable(const std::string &name, const void *p, std::type_index t_index)
{
    this->register_variable(name, p, t_index);
}
void OutputRootTreeFile::write_variable(const std::string &name, const std::string *p, size_t nb_char)
{
    char *s = new char[nb_char];
    m_mapConstStringToRootString.emplace(p, s);


    this->write_variable(name, s, nb_char);
}
void OutputRootTreeFile::write_variable(const std::string &name, const char *p, size_t nb_char)
{
    this->register_variable(name, p, nb_char);
}


void InputRootTreeFile::check_existence_and_kind(const std::string &name, std::type_index t_index)
{
  if(!m_read_header_called)
    throw std::logic_error("read_header not called");

  assert(m_ttree);
  auto leaf = m_ttree->GetLeaf(name.c_str());

  if(!leaf)
  {
    std::stringstream ss;
    ss << "Variable named '" << name << "' not found !";
    throw KeyNotFoundInHeaderException(ss.str());
  }

  auto key = string(leaf->GetTypeName ());

//  cout << "t_index = " << t_index.name() << endl;
//  cout << "leaf = " << key << " recorded = " << m_tmapOfTindexToLongDefinition.at(t_index) << endl;
//  cout << "leaf GetLenStatic = " << leaf->GetLenStatic() << endl;
//  cout << "leaf GetLen = " << leaf->GetLen() << endl;

  if(key == "Char_t" and (t_index == typeid(string)  or t_index == typeid(char*) ))
    return;


  if( key !=  m_tmapOfTindexToLongDefinition.at(t_index) )
  {
    std::stringstream ss;
    ss << "type_index given to store '" << name <<"' has not the right type (given " << m_tmapOfTindexToLongDefinition.at(t_index) << " but need " << leaf->GetTypeName () << ")";
    throw TypeMismatchHeaderException(ss.str());
  }

}

void InputRootTreeFile::read_variable(const std::string &name, void *p, std::type_index t_index)
{
  this->check_existence_and_kind(name, t_index);
  m_ttree->SetBranchAddress(name.c_str(), p);
}

void InputRootTreeFile::read_variable(const std::string &name, char *p)
{
  this->check_existence_and_kind(name, typeid(char*));
  m_ttree->SetBranchAddress(name.c_str(), p);
}

void InputRootTreeFile::read_variable(const std::string &name, char *p, size_t)
{
  this->check_existence_and_kind(name, typeid(char*));
  this->read_variable(name, p);
}

void InputRootTreeFile::read_variable(const std::string &name, std::string *p)
{
  this->check_existence_and_kind(name, typeid(string));

  char* s = new char[1024];
  m_mapNewStringToRefString.emplace(s, p);
  m_ttree->SetBranchAddress(name.c_str(), s);
}

void InputRootTreeFile::read_header()
{
//    cout << "read tree name = " << m_nameOfTree << " from file " << endl;
    m_file->GetObject(m_nameOfTree.c_str(), m_ttree);
    assert(m_ttree);
    m_current_entry = 0;
    m_read_header_called = true;
}
void InputRootTreeFile::read_next_entrie()
{
    m_ttree->GetEntry(m_current_entry);

    for(auto ss: m_mapNewStringToRefString)
    {
        ss.second->assign((ss.first));
    }

    m_current_entry++;
}
bool InputRootTreeFile::data_to_read()
{
  if(!m_read_header_called)
    throw std::logic_error("read_header not called");
  return m_current_entry < m_ttree->GetEntries();
}



InputRootTreeFile::~InputRootTreeFile()
{
    for(auto ss: m_mapNewStringToRefString)
    {
        delete[](ss.first);
    }
}


void OutputRootTreeFile::set_tree_name(const std::string &name)
{
  OutputTreeFile::set_tree_name(name);
  m_ttree->SetName(name.c_str());
}

void InputRootTreeFile::set_tree_name(const std::string &name)
{
  InputTreeFile::set_tree_name(name);
}

InputRootTreeFile::InputRootTreeFile() : RootTree(), m_read_header_called(false)
{}



bool InputRootTreeFile::has_variable(const std::string &name)
{
  if(!m_read_header_called)
    throw std::logic_error("read_header not called");

  return m_ttree->GetListOfBranches()->FindObject(name.c_str());
}

type_index InputRootTreeFile::get_type_of_variable(const std::string &name)
{
  if(!m_read_header_called)
    throw std::logic_error("read_header not called");

  auto leaf = m_ttree->GetLeaf(name.c_str());
  auto key = string(leaf->GetTypeName ());

  if(key == "Char_t")
    throw NoTypeInHeaderException("type of string, char, char* can not be retrieved. No distinction are mage in ROOT file");

  return m_tmapOfLongDefinitionToIndex.at(key);
}


uint64_t InputRootTreeFile::nb_elements()
{
  assert(m_ttree);
  return m_ttree->GetEntries();
}

void InputRootTreeFile::read_entrie(const uint64_t &i)
{
  m_ttree->GetEntry(i);
  for(auto ss: m_mapNewStringToRefString)
  {
    ss.second->assign((ss.first));
  }
  m_current_entry = i+1;
}


bool OutputRootTreeFile::s_registered =
    OutputTreeFileFactory::_register(OutputRootTreeFile::_get_factory_name(), &OutputRootTreeFile::_create_method<OutputRootTreeFile>);

bool InputRootTreeFile::s_registered =
    InputTreeFileFactory::_register(OutputRootTreeFile::_get_factory_name(), &InputRootTreeFile::_create_method<InputRootTreeFile>);












