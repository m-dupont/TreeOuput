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
              const std::type_index type_index,
              const size_t nb_characters,
              const size_t size

    ) : Data(pointer_to_data, _name, type_index),
        m_type(_type), m_nb_characters(nb_characters),
        m_dataset(nullptr),
        m_size_one_element(size), m_buffer(nullptr),
        m_buffer_len(0), m_data_len_already_saved(0)
    {
        if(nb_characters == 0)
        {
            m_buffer = malloc(m_size_one_element * MAX_BUFFER_LEN) ;
        }
        else
        {
            m_buffer = malloc(nb_characters * MAX_BUFFER_LEN) ;
        }


    }

    void fill_one_buffer();

    virtual ~HDF5Data()
    {
//        std::cout << "~HDF5Data" << std::endl;
        delete m_dataset;
        if(m_buffer)
            free(m_buffer);
    }

    H5::AtomType m_type;
    size_t m_nb_characters;
    H5::DataSet *m_dataset;
    size_t m_size_one_element;
    void *m_buffer;
    const size_t MAX_BUFFER_LEN = 256;
    size_t m_buffer_len;
    size_t m_data_len_already_saved;
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
    std::vector<HDF5Data*> m_vector_of_pointer_to_data;
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

    void empty_buffer_to_file(HDF5Data *data);



private:
    hsize_t m_dims_f[HDF5Tree::RANK] = {0}; // dataset dimensions
    hsize_t m_maxdims[HDF5Tree::RANK] = {H5S_UNLIMITED};
    H5::DataSpace m_dataspace;
    bool m_write_header_called;
};




