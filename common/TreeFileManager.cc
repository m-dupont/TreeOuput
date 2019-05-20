//
// Created by mdupont on 12/03/18.
//

#include "TreeFileManager.hh"


#include <iostream>
#include <memory>
#include <iostream>
#include <exception>
#include <sstream>
#include <algorithm>
#include <functional>

#include "FileExceptions.hh"


using namespace std;


//map<const string, const string> OutputTreeFileFactory::s_methods;


bool OutputTreeFileFactory::_register(const string name,
                                        TCreateOutputTreeFileMethod& funcCreate)
{
    auto it = get_methods_map().find(name);
    if(it != get_methods_map().end())
    {
      throw KeyAlreadyExistsException("nooooooooo");
    }

    get_methods_map().emplace(name, funcCreate);
    return true;
}

std::unique_ptr<OutputTreeFile>
OutputTreeFileFactory::_create(const string& name)
{
  auto it = get_methods_map().find(name);
  if(it != get_methods_map().end())
  {
    return it->second(); // call the createFunc
  }

  return nullptr;
}




OutputTreeFileManager::OutputTreeFileManager()
{
  m_nameOfTree = Tree::default_tree_name();
}

OutputTreeFileManager::OutputTreeFileManager(OutputTreeFileManager &&m) :
  m_listOfTreeFile(move(m.m_listOfTreeFile)),
  m_nameOfTree(move(m.m_nameOfTree))

{}




void OutputTreeFileManager::write_variable(const std::string &name, const std::string *p, size_t nb_char)
{
  for(auto&& f : m_listOfTreeFile)
  {
    f->write_variable(name, p, nb_char);
  }
}

void OutputTreeFileManager::write_variable(const std::string &name, const char *p, size_t nb_char)
{
  for(auto&& f : m_listOfTreeFile)
  {
    f->write_variable(name, p, nb_char);
  }
}

void OutputTreeFileManager::close()
{
  for(auto&& f : m_listOfTreeFile)
  {
    f->close();
  }
}

void OutputTreeFileManager::fill()
{
  for(auto& f : m_listOfTreeFile)
  {
    f->fill();
  }
}

void OutputTreeFileManager::write_header()
{
  for(auto&& f : m_listOfTreeFile)
  {
    f->set_tree_name(m_nameOfTree);
    f->write_header();
  }
}

std::unique_ptr<OutputTreeFile> OutputTreeFileManager::add_file(const std::string &file_path, const std::string &kind)
{

  auto h = OutputTreeFileFactory::_create(kind);

  if(!h)
  {
    std::stringstream ss;
    ss << "Error do not know type '"  << kind; ;
    throw std::runtime_error(ss.str());
  }

  h->open(file_path);
  m_listOfTreeFile.push_back(move(h));
  return h;
}

OutputTreeFileManager::~OutputTreeFileManager()
{

}

void OutputTreeFileManager::set_tree_name(const std::string &name)
{
  m_nameOfTree = name;
}

bool InputTreeFileFactory::_register(const std::string name, TCreateInputTreeFileMethod &funcCreate)
{

  auto it = get_methods_map().find(name);
  if(it != get_methods_map().end())
  {
    throw KeyAlreadyExistsException("nooooooooo");
  }

  get_methods_map().emplace(name, funcCreate);
  return true;
}

std::unique_ptr<InputTreeFile> InputTreeFileFactory::_create(const std::string &name)
{
  auto it = get_methods_map().find(name);
  if(it != get_methods_map().end())
  {
    return it->second(); // call the createFunc
  }

  return nullptr;
}



InputTreeFileManager::InputTreeFileManager()
{
  m_nameOfTree = Tree::default_tree_name();
}

std::unique_ptr<InputTreeFile> InputTreeFileManager::set_file(const std::string &file_path, const std::string &kind)
{
  auto h = InputTreeFileFactory::_create(kind);

  if(!h)
  {
    std::stringstream ss;
    ss << "Error do not know type '"  << kind; ;
    throw UnknownKindManagerException(ss.str());
  }

  h->open(file_path);
  m_inputtreefile = move(h);
  return h;
}

bool InputTreeFileManager::data_to_read()
{
  return m_inputtreefile->data_to_read();
}

void InputTreeFileManager::close()
{
  if(m_inputtreefile)
    m_inputtreefile->close();
}

void InputTreeFileManager::read_next_entrie()
{
  m_inputtreefile->read_next_entrie();
}

void InputTreeFileManager::read_header()
{
  m_inputtreefile->set_tree_name(m_nameOfTree);
  m_inputtreefile->read_header();
}

void InputTreeFileManager::read_variable(const std::string &name, void *p, std::type_index t_index)
{
  m_inputtreefile->read_variable(name, p, t_index);
}

void InputTreeFileManager::read_variable(const std::string &name, char *p)
{
  m_inputtreefile->read_variable(name, p);
}

void InputTreeFileManager::read_variable(const std::string &name, char *p, size_t nb_char)
{
  m_inputtreefile->read_variable(name, p, nb_char);
}

void InputTreeFileManager::read_variable(const std::string &name, std::string *p)
{
  m_inputtreefile->read_variable(name, p);
}


void InputTreeFileManager::set_tree_name(const std::string &name)
{
  m_nameOfTree = name;
}

bool InputTreeFileManager::has_variable(const std::string &name)
{
  return m_inputtreefile->has_variable(name);
}

std::type_index InputTreeFileManager::get_type_of_variable(const std::string &name)
{
  return m_inputtreefile->get_type_of_variable(name);
}

uint64_t InputTreeFileManager::nb_elements()
{
  return m_inputtreefile->nb_elements();
}

void InputTreeFileManager::read_entrie(const uint64_t &i)
{
  m_inputtreefile->read_entrie(i);
}


//std::unique_ptr<InputTreeFile> InputTreeFileManager::get_file(const std::string &file_path, const std::string &kind)
//{
//
//    auto h = InputTreeFileFactory::_create(kind);
//
//    if(!h)
//    {
//        std::stringstream ss;
//        ss << "Error do not know type '"  << kind; ;
//        throw UnknownKindManagerException(ss.str());
//    }
//
//    h->open(file_path);
////    m_inputtreefile = move(h);
//    return h;
//}
std::unique_ptr<InputTreeFile> InputTreeFileChain::add_file(const std::string &file_path, const std::string &kind)
{
  auto h = InputTreeFileFactory::_create(kind);

  if(!h)
  {
    std::stringstream ss;
    ss << "Error do not know type '"  << kind; ;
    throw std::runtime_error(ss.str());
  }

  h->open(file_path);
  m_listOfTreeFile.push_back(move(h));
  return h;
}

bool InputTreeFileChain::data_to_read()
{
  for(auto &f: m_listOfTreeFile)
  {
    if(f->data_to_read())
      return true;
  }
  return false;
}

void InputTreeFileChain::read_entrie()
{
  for(auto &f: m_listOfTreeFile)
  {
    if(f->data_to_read())
    {
      f->read_next_entrie();
      return;
    }
  }

}

void InputTreeFileChain::read_header()
{
  for(auto &f: m_listOfTreeFile)
  {
    f->set_tree_name(m_nameOfTree);
    f->read_header();
  }
}

void InputTreeFileChain::read_variable(const std::string &name, char *p)
{
  for(auto &f: m_listOfTreeFile)
  {
    f->read_variable(name, p);
  }

}

void InputTreeFileChain::read_variable(const std::string &name, std::string *p)
{
  for(auto &f: m_listOfTreeFile)
  {
    f->read_variable(name, p);
  }
}

void InputTreeFileChain::read_variable(const std::string &name, char *p, size_t nb_char)
{
  for(auto &f: m_listOfTreeFile)
  {
    f->read_variable(name, p, nb_char);
  }
}

void InputTreeFileChain::read_variable(const std::string &name, void *p, std::type_index t_index)
{
  for(auto &f: m_listOfTreeFile)
    f->read_variable(name, p, t_index);

}

void InputTreeFileChain::close()
{
  for(auto &f: m_listOfTreeFile)
    f->close();
}

void InputTreeFileChain::set_tree_name(const std::string &name)
{
  m_nameOfTree = name;
}

bool InputTreeFileChain::has_variable(const std::string &name)
{
  for(auto &f: m_listOfTreeFile)
  {
    if(!f->has_variable(name))
      return false;
  }
  return true;
}

std::type_index InputTreeFileChain::get_type_of_variable(const std::string &name)
{
  vector<std::type_index> v;
  for(auto &f: m_listOfTreeFile)
  {
    try
    {
      v.push_back(f->get_type_of_variable(name));
      cout << "file = " << f->path()  << " t = " <<  f->get_type_of_variable(name).name() << endl;
    }
    catch (const NoTypeInHeaderException&)
    {

    }
  }

  if(v.empty())
    throw NoTypeInHeaderException("Provided files not able to provide type in data");

  if ( std::adjacent_find( v.begin(), v.end(), std::not_equal_to<std::type_index>() ) == v.end() )
  {
    // All elements are equals
    return v.at(0);
  }
  else
    throw runtime_error("Differents type index !");



}

uint64_t InputTreeFileChain::nb_elements()
{
  uint64_t n = 0;
  for(auto &f: m_listOfTreeFile)
  {
    n += f->nb_elements();
  }
  return n;
}

InputTreeFileChain::InputTreeFileChain()
{
  m_nameOfTree = Tree::default_tree_name();
}

void InputTreeFileChain::read_entrie(const uint64_t &i)
{
  uint64_t seek = i;

  for(auto &f: m_listOfTreeFile)
  {
    if(seek < f->nb_elements())
    {
      f->read_entrie(seek);
      return;
    }
    seek -= f->nb_elements();
  }
}
