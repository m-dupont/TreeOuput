#include <iostream>
#include <fstream>

#include <typeindex>

#include "impl/NumpyFile.hh"
#include "impl/AsciiFile.hh"
#include "impl/RootTreeFile.hh"
#include <cassert>

#include <unordered_map>

#include <cstring>
#include <random>
#include <string>


#include <iomanip>
using namespace std;

//typedef std::unordered_map<std::type_index, string> tmap;


#include "TreeFileManager.hh"
#include "HDF5File.hh"


void example_save_numpy_file()
{

  OutputNumpyTreeFile h;
  h.open("/tmp/z5.npy");
//
  char s[32] = "wxcvb";
  h.write_variable("name", s, 20);
  unsigned int trackID = 0;

  h.write_variable("trackID", &trackID);

//
  double gauss = 0.;
  h.write_variable("gauss", &gauss);

//
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(5.0,2.0);

  uint64_t i;

  h.write_header();

  for (i = 0; i < 500; ++i)
  {
    strcpy(s,"plop");
    trackID = (i+1)*2;
    gauss = distribution(generator);
    h.fill();

  }
  h.close();
}


void example_save_and_open_root_file()
{

  {
    OutputRootTreeFile ho;
    ho.open("/tmp/z.root");

    char name[32] = "wxcvb";
    ho.write_variable("name", name, 32);

    string str = "plop";
    ho.write_variable("str", &str, 32);

    double a = 2;
    ho.write_variable("a", &a);

    ho.write_header();
    for(int i = 0; i < 4; ++i)
    {
      a += i + 0.33;
      cout << "\tsore a = " << a <<  " name = " << name << "str = " << str << endl;
      ho.fill();
      strcpy(name,"plop");
      str = "beuh";
    }

    ho.close();

  }


  cout << "###########" << endl;


  {


    InputRootTreeFile h;

    h.open("/tmp/z.root");
    h.read_header();

    double a;
    h.read_variable("a", &a);

    char name[32];
    h.read_variable("name", name);

    string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    h.read_variable("str", &str);

    while ( h.data_to_read() ){
      h.read_next_entrie();
      cout << "\tread a = " << a << " name = " << name << " str = " <<  str << endl;
    }


  }
}


void example_save_and_open_numpy_file()
{

  {
    OutputNumpyTreeFile ho;
    ho.open("/tmp/z.npy");

    char name[32] = "wxcvb";
    ho.write_variable("name", name, 32);

    string str = "plop";
    ho.write_variable("str", &str, 32);

    double a = 2;
    ho.write_variable("a", &a);

    ho.write_header();
    for(int i = 0; i < 4; ++i)
    {
      a += i + 0.33;
      cout << "\tsore a = " << a <<  " name = " << name << "str = " << str << endl;
      ho.fill();
      strcpy(name,"plop");
      str = "beuh";
    }

    ho.close();

  }


  cout << "###########" << endl;


  {
    InputNumpyTreeFile h;

    h.open("/tmp/z.npy");
    h.read_header();

    double a;
    h.read_variable("a", &a);

    char name[32];
    h.read_variable("name", name);

    string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    h.read_variable("str", &str);

    while ( h.data_to_read() ){
      h.read_next_entrie();
      cout << "\tread a = " << a << " name = " << name << " str = " <<  str << endl;
    }


  }
}


void example_save_ascii_file()
{
  double d = 3.4;
  string s = "plop";

  char c[32] = "wxcvb";
  int noline = 0;

  OutputAsciiTreeFile h;
  h.open("/tmp/z.txt");
  h.write_variable("noline", &noline);
  h.write_variable("d", &d);
  h.write_variable("u", &d);
  h.write_variable("name", &s, 20);
  h.write_variable("c", c, 20);
  h.write_header();

  for (auto i = 0; i < 1000; ++i)
  {
    h.fill();
    s = "impair";
    d /= 2.;
    if (i%2)
      s = "pair";

    strcpy(c,"plop");
    noline ++;
  }

  h.close();

  InputAsciiTreeFile hi;
  hi.open("/tmp/z.txt");

  hi.read_header();

  int rnoline = -1;

  hi.read_variable("d", &d);
  hi.read_variable("noline", &rnoline);
  hi.read_variable("name", &s);

  for (auto i = 0; i < 10; ++i)
  {
    hi.read_next_entrie();
    cout << "##rnoline = " << rnoline << " d = " << d << " name = " << s <<endl;
  }
  hi.close();
}


void example_save_root_file()
{
  OutputRootTreeFile h;
  double d = 0;
  string s = "plop";
  h.open("/tmp/z.root");
  h.write_variable("d", &d);
  h.write_variable("s", &s, 10);
  for(auto i = 0; i < 10; ++i)
  {
    d = i / 2.;
    s = "plop_1";
//        s.assign("pplop");
    cout << "store "  << "\ti = " << i  << "\td = " << d  << " s = " << s << endl;
    h.fill();
    s = "plop_2";
    h.fill();
  }
  h.close();

  InputRootTreeFile hi;
  hi.open("/tmp/z.root");
  double dd = -1.;
  string ss = "pas plop";
//    char particleName[64];
  string particleName = "pas plop";
  hi.read_header();
  hi.read_variable("d", &dd);
  hi.read_variable("s", &particleName);


  auto i = 0;
  while( hi.data_to_read() )
  {
    hi.read_next_entrie();
    cout << "read " << "\ti = " << i << "\td = " << dd << "\t particleName = " << particleName << endl;
    ++i;
  }
  hi.close();
}

void example_save_manager()
{
  OutputTreeFileManager m;

//  OutputAsciiTreeFile h;
//  h.open("/tmp/z.txt");

//  OutputNumpyTreeFile n;
//  n.open("/tmp/z.npy");

//  OutputRootTreeFile r;
//  r.open("/tmp/z.root");




  m.add_file("/tmp/z.root", "root");
  m.add_file("/tmp/z.npy", "npy");
  m.add_file("/tmp/z.txt", "txt");

  m.set_tree_name("ROOT2");


  double d = 0;
  string s = "plop";
  m.write_variable("d", &d);
  m.write_variable("s", &s, 10);


  m.write_header();


  for(auto i = 0; i < 10; ++i)
  {
    d = i / 2.;
    s = "plop_1";
    cout << "store "  << "\ti = " << i  << "\td = " << d << endl;
    m.fill();
    s = "plop_2";
    m.fill();
  }
  m.close();
}


void example_open_manager()
{

  cout << "example_open_manager" << endl;

  {
    OutputTreeFileManager ho;
    ho.set_tree_name("merde");
    ho.add_file("/tmp/z.root", "root");
    ho.add_file("/tmp/z.npy", "npy");
    ho.add_file("/tmp/z.txt", "txt");

    char name[64] = "wxcvb";
    ho.write_variable("name", name, 64);

    string str = "plop";
    ho.write_variable("str", &str, 32);

    double a = 2;
    ho.write_variable("a", &a);

    ho.write_header();
    for(int i = 0; i < 4; ++i)
    {
      a += i + 0.33;
      cout << "\tsore a = " << a <<  " name = " << name << "str = " << str << endl;
      ho.fill();
      strcpy(name,"plop!1!2!3!4!5!6!7!8!9!10!11!12!13!14!15!16!17!18!19");
      str = "beuh";
    }

    ho.close();

  }


  cout << "###########" << endl;


  if(false){

    InputTreeFileManager h;
    h.set_tree_name("merde");



//    h.set_file("/tmp/z.npy", "npy");
    h.set_file("/tmp/z.root", "root");
//    h.set_file("/tmp/z.txt", "txt");
    h.read_header();

    double a;
    h.read_variable("a", &a);

    char name[64];
    h.read_variable("name", name, 64);

    string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    h.read_variable("str", &str);

    while ( h.data_to_read() ){
      h.read_next_entrie();
      cout << "\tread a = " << a << " name = " << name << " str = " <<  str << endl;
    }

    h.close();
  }

  {
    InputTreeFileChain hi;
    hi.set_tree_name("merde");
    hi.add_file("/tmp/z.npy", "npy");
    hi.add_file("/tmp/z.txt", "txt");
    hi.add_file("/tmp/z.root", "root");


    hi.read_header();

    char name[64];
    hi.read_variable("name", name, 64);
    double a;

    assert( hi.has_variable("a") );
    auto t = hi.get_type_of_variable("a");

    assert(t == typeid(a));

    hi.read_variable("a", &a);
    string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    hi.read_variable("str", &str);

    while ( hi.data_to_read() ){
      hi.read_entrie();
      cout << "\tread a = " << a << " name = " << name << " str = " <<  str << endl;
    }
    hi.close();
  }



  cout << "example_open_manager:done" << endl;
}

void example_save_hdf5()
{
    cout << "example_save_hdf5:start" << endl;
    OutputHDF5TreeFile h;
    h.open("/tmp/p.h5");

    int a = 5;
    int b = 10;
    double d = 0;
    float f = 0;
    std::string  s = "plop";
    std::string  cs = "plop";
//    const char *c = "wxcvb";
    char cc[32] = "plop";
    h.write_variable("a", &a);
    h.write_variable("b", &b);
    h.write_variable("d", &d);
    h.write_variable("f", &f);
    h.write_variable("s", &s, 32);
//    h.write_variable("ss", &s);
//    h.write_variable("sc", &cs);
//    h.write_variable("c", c);
//    h.write_variable("c", c);
    h.write_variable("cc", cc, 32);
    h.write_header();

    for (auto i = 0; i < 256; ++i)
    {
        b = i*10;
        d = i/2.;
        f = i/3.;
        if( i % 2 == 0)
            s = "pair";
        else
            s = "impair";

        strncpy(cc,"1234567890AZERTYUIOPQSDFGHJKLMWXCVBN", i%30 + 1);
        cc[i%30 + 2] = '\0';

        cs += "e";

        h.fill();
    }



    h.close();




    cout << "example_save_hdf5:done" << endl;

}

int main()
{
  std::cout << "Hello, World!" << std::endl;
//  example_save_ascii_file();

//  example_save_numpy_file();
//  example_save_and_open_numpy_file();

//    example_save_root_file();

//  example_save_manager();

//  example_save_and_open_numpy_file();
//  example_save_and_open_root_file();

//  example_save_manager();
//  example_open_manager();

    example_save_hdf5();

}
