//
// Created by mdupont on 04/04/19.
//

#include <functional>
#include <chrono>
#include <string>
#include <iostream>
#include <random>
#include <numeric>
#include <algorithm>

#include "impl/NumpyFile.hh"
#include "impl/RootTreeFile.hh"
#include "impl/AsciiFile.hh"
#include "tests/helpers.hh"
#include "HDF5File.hh"

using namespace std;
using namespace std::chrono;

typedef const std::function<void()> benchmark_f;


class Benchmark
{
 public:
  Benchmark(const string &name, benchmark_f &f):
  m_name {name},  m_f(f) {}

  void run()
  {
//      cout << "[" << m_name <<"]" << "Start execute function" << endl;

      std::vector<double> values;


      auto start = high_resolution_clock::now();
      for (auto i = 0; i < 20; ++i)
      {

        auto start_loop = high_resolution_clock::now();
        m_f();
        auto stop_loop = high_resolution_clock::now();
        auto duration_loop = duration_cast<microseconds>(stop_loop - start_loop);
        values.push_back(duration_loop.count()/1000.);
      }


      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<milliseconds>(stop - start);

      auto sum = std::accumulate(values.begin(), values.end(), 0.0);
      double mean = sum / values.size();

      std::vector<double> diff(values.size());
      std::transform(values.begin(), values.end(), diff.begin(), [mean](double x) { return x - mean; });
      auto sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
      auto stdev = std::sqrt(sq_sum / values.size());

      double min = *std::min_element(values.begin(), values.end());
      double max = *std::max_element(values.begin(), values.end());



//      cout << "[" << m_name <<"]" << "Start execute function done, time = " <<  duration.count() / 10. << "ms" << endl;
      cout << "[" << m_name <<"]" << "Start execute function done, time = " <<
      mean << "ms, std = " <<
      stdev << " ms " << "(min = " << min << ", max = "<< max << " )"  << endl;
  }

 private:
  string m_name;
  benchmark_f m_f;

};

template <typename T>
void f()
{
    T ho;
    ho.open("/tmp/z");

    uint32_t j = 0;
    double d = 2.;
    double d2 = 1.;
    string s = "plop";
    string s2 = "plop";

    ho.write_variable("integer", &j);
    ho.write_variable("double", &d);
    ho.write_variable("string", &s, 32);

    ho.write_variable("integer2", &j);
    ho.write_variable("double2", &d2);
    ho.write_variable("string2", &s2, 16);

    ho.write_header();

    std::vector<string> choices = {"lundi", "mardi", "mercredi", "jeudi", "vendredi"};

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,choices.size() - 1);

    for (auto i = 0; i < 10000; ++i)
    {
        j++;
        d = i*2.;
        d2 = 1./((double)i);
        s = choices.at(distribution(generator));
        s2 = choices.at(distribution(generator));
        ho.fill();
    }
    ho.close();
}



void g()
{

//  helpers::remove_file("/tmp/z.npy");

  OutputTreeFileManager ho;

//  OutputAsciiTreeFile h;
//  h.open("/tmp/z.txt");
//
//  OutputNumpyTreeFile n;
//  n.open("/tmp/z.npy");
//
//  OutputRootTreeFile r;
//  r.open("/tmp/z.root");

//  ho.add_file(&h);
//  ho.add_file(&n);
//  ho.add_file(&r);

  ho.add_file("/tmp/z.root", "root");
  ho.add_file("/tmp/z.npy", "npy");
  ho.add_file("/tmp/z.txt", "txt");

  uint32_t j = 0;
  double d = 2.;
  double d2 = 1.;
  string s = "plop";
  string s2 = "plop";

  ho.write_variable("integer", &j);
  ho.write_variable("double", &d);
  ho.write_variable("string", &s, 16);

  ho.write_variable("integer2", &j);
  ho.write_variable("double2", &d2);
  ho.write_variable("string2", &s2, 16);

  ho.write_header();

  std::vector<string> choices = {"lundi", "mardi", "mercredi", "jeudi", "vendredi"};

  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0,choices.size() - 1);

  for (auto i = 0; i < 100000; ++i)
  {
    j++;
    d = i*2.;
    d2 = 1./((double)i);
    s = choices.at(distribution(generator));
    s2 = choices.at(distribution(generator));
    ho.fill();
  }
  ho.close();
}

int main()
{

    helpers::remove_file("/tmp/z");
    Benchmark a("OutputHDF5TreeFile",  f<OutputHDF5TreeFile>);
    a.run();
    helpers::remove_file("/tmp/z");

    helpers::remove_file("/tmp/z");
    Benchmark b("OutputNumpyTreeFile",  f<OutputNumpyTreeFile>);
    b.run();
    helpers::remove_file("/tmp/z");
//
    Benchmark c("OutputRootTreeFile",  f<OutputRootTreeFile>);
    c.run();
    helpers::remove_file("/tmp/z");
//
    Benchmark d("OutputAsciiTreeFile",  f<OutputAsciiTreeFile>);
    d.run();
    helpers::remove_file("/tmp/z");
//
    Benchmark e("TreeFileManager",  g);
    e.run();

    return 0;
}