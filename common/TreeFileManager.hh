//
// Created by mdupont on 12/03/18.
//

#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>

#include "TreeFile.hh"


typedef const std::function<std::unique_ptr<OutputTreeFile>()> TCreateOutputTreeFileMethod;
typedef std::map<const std::string,TCreateOutputTreeFileMethod> CreateOutputTreeFileMethodMap;



class OutputTreeFileFactory
{
public:
  static bool _register(const std::string name, TCreateOutputTreeFileMethod& funcCreate);
  static std::unique_ptr<OutputTreeFile> _create(const std::string& name);

private:
  OutputTreeFileFactory() = delete;
  static CreateOutputTreeFileMethodMap& get_methods_map()
  {
    static CreateOutputTreeFileMethodMap s_methods;
    return s_methods;
  }

};



class OutputTreeFileManager
{
public:
  OutputTreeFileManager();
  OutputTreeFileManager(OutputTreeFileManager &&m);
  virtual ~OutputTreeFileManager();

  std::unique_ptr<OutputTreeFile> add_file(const std::string &file_path, const std::string &kind);

  template<typename T>
  void write_variable(const std::string &name, const T *p)
  {
    for(auto&& f : m_listOfTreeFile)
    {
      f->write_variable(name, p, typeid(T));
    }
  }

  void write_variable(const std::string &name, const std::string *p, size_t nb_char);
  void write_variable(const std::string &name, const char *p, size_t nb_char);
  void set_tree_name(const std::string &name);

  void fill();
  void close();
  void write_header();



private:
  std::vector<std::unique_ptr<OutputTreeFile>> m_listOfTreeFile;
  std::string m_nameOfTree;
};



typedef const std::function<std::unique_ptr<InputTreeFile>()> TCreateInputTreeFileMethod;
typedef std::map<const std::string,TCreateInputTreeFileMethod> CreateInTreeFileMethodMap;


class InputTreeFileFactory
{
public:
  static bool _register(const std::string name, TCreateInputTreeFileMethod& funcCreate);
  static std::unique_ptr<InputTreeFile> _create(const std::string& name);



private:
  InputTreeFileFactory() = delete;
  static CreateInTreeFileMethodMap& get_methods_map()
  {
    static CreateInTreeFileMethodMap s_methods;
    return s_methods;
  }

};

class InputTreeFileManager
{
public:

//  static std::unique_ptr<InputTreeFile> get_file(const std::string &file_path, const std::string &kind);

  InputTreeFileManager();
  std::unique_ptr<InputTreeFile> set_file(const std::string &file_path, const std::string &kind);
  bool data_to_read();
  void close();
  void read_next_entrie();
  void read_entrie(const uint64_t& i);
  void read_header();
  void set_tree_name(const std::string &name);
  uint64_t nb_elements();

  template<typename T>
  void read_variable(const std::string name, T *p)
  {
    read_variable(name, p, typeid(T));
  }

  void read_variable(const std::string &name, char* p);
  void read_variable(const std::string &name, std::string *p);
  void read_variable(const std::string &name, char* p, size_t nb_char);

  bool has_variable(const std::string &name);
  std::type_index get_type_of_variable(const std::string &name);



private:
  void read_variable(const std::string &name, void *p, std::type_index t_index);
  std::unique_ptr<InputTreeFile> m_inputtreefile;
  std::string m_nameOfTree;
};


class InputTreeFileChain
{

public:
  std::unique_ptr<InputTreeFile> add_file(const std::string &file_path, const std::string &kind);
  bool data_to_read();
  void read_entrie();
  void read_header();
  void close();

  InputTreeFileChain();

  void set_tree_name(const std::string &name);
  uint64_t nb_elements();
  void read_entrie(const uint64_t& i);

  template<typename T>
  void read_variable(const std::string name, T *p)
  {
    read_variable(name, p, typeid(T));
  }

  void read_variable(const std::string &name, char* p);
  void read_variable(const std::string &name, std::string *p);
  void read_variable(const std::string &name, char* p, size_t nb_char);

  bool has_variable(const std::string &name);
  std::type_index get_type_of_variable(const std::string &name);


private:
  void read_variable(const std::string &name, void *p, std::type_index t_index);

  std::vector<std::unique_ptr<InputTreeFile>> m_listOfTreeFile;
  std::string m_nameOfTree;

};





