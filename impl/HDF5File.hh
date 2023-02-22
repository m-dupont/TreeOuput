//
// Created by Mathieu Dupont <mdupont@cppm.in2p3.fr> on 20/02/23.
// Copyright (c) 2023 -- Aix Marseille Univ, CNRS/IN2P3, CPPM, Marseille, France -- All rights reserved.
//

#pragma once

#include <vector>
#include "H5Cpp.h"
#include "TreeFile.hh"

class HDF5Data : public Data {
public:
    HDF5Data(const void * pointer_to_data,
              const std::string _name,
              const H5::AtomType _type,
              const std::type_index type_index

    ) : Data(pointer_to_data, _name, type_index),
        m_type(_type), m_nb_characters(0), m_dataset(nullptr)
    {

    }

    virtual ~HDF5Data()
    {
//        std::cout << "~HDF5Data" << std::endl;
        delete m_dataset;
    }

    H5::AtomType m_type;
    size_t m_nb_characters;
    H5::DataSet *m_dataset;
};

class HDF5Tree : public Tree {
public:
    HDF5Tree();


protected:
    void register_variable(const std::string &name, const void *p, std::type_index t_index) override;

    void register_variable(const std::string &name, const std::string *p, size_t nb_char) override;

    void register_variable(const std::string &name, const char *p, size_t nb_char) override;

private:
    void fill_maps(std::type_index t_index, const H5::PredType &);

    template<typename T>
    void fill_maps(const H5::PredType &t)
    {
        fill_maps(std::type_index(typeid(T)), t);
    }


protected:

    template<typename T>
    void register_variable(const std::string &name, const T *p)
    {
        register_variable(name, p, typeid(T));
    }

    uint64_t m_nb_elements;
    H5::H5File m_file;
    H5::Group m_group;
    std::vector<HDF5Data> m_vector_of_pointer_to_data;
    std::unordered_map<std::type_index, H5::AtomType> m_tmap_cppToHDF5;
    const static int RANK = 1;
};


class OutputHDF5TreeFile : public HDF5Tree, public OutputTreeFile {
public:
    bool is_open() override;

    void close() override;

    void open(const std::string &s) override;

    void write_header() override;

    void fill() override;

    void write_variable(const std::string &name, const void *p, std::type_index t_index) override;

    void write_variable(const std::string &name, const std::string *p, size_t nb_char) override;

    void write_variable(const std::string &name, const char *p, size_t nb_char) override;

    template<typename T>
    void write_variable(const std::string &name, const T *p)
    {
        register_variable(name, p);
    }



private:
    hsize_t m_dims_f[HDF5Tree::RANK] = {1}; // dataset dimensions
    hsize_t m_maxdims[HDF5Tree::RANK] = {H5S_UNLIMITED};
    H5::DataSpace m_dataspace;
    bool m_write_header_called;
};




