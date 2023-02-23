//
// Created by Mathieu Dupont <mdupont@cppm.in2p3.fr> on 20/02/23.
// Copyright (c) 2023 -- Aix Marseille Univ, CNRS/IN2P3, CPPM, Marseille, France -- All rights reserved.
//

#include <cstring>
#include "HDF5File.hh"

using namespace H5;
using namespace std;


HDF5Tree::HDF5Tree() : m_nb_elements(0)
{
//    cout << "HDF5Tree" << endl;
//    fill_maps<int>(PredType::NATIVE_INT);
    fill_maps<uint8_t>(PredType::NATIVE_UINT8);
    fill_maps<uint16_t>(PredType::NATIVE_UINT16);
    fill_maps<uint32_t>(PredType::NATIVE_UINT32);
    fill_maps<uint64_t>(PredType::NATIVE_UINT64);


    fill_maps<int8_t>(PredType::NATIVE_INT8);
    fill_maps<int16_t>(PredType::NATIVE_INT16);
    fill_maps<int32_t>(PredType::NATIVE_INT32);
    fill_maps<int64_t>(PredType::NATIVE_INT64);

    H5::StrType t(H5::PredType::C_S1, H5T_VARIABLE);
    m_tmap_cppToHDF5.emplace(typeid(std::string), t);
    m_tmap_cppToHDF5.emplace(typeid(char *), t);
    m_tmap_cppToHDF5.emplace(typeid(char[]), t);

//    cout << "t char[] = " << typeid(char[]).name() << endl;
//    cout << "t char* = " << typeid(char*).name() << endl;
//    cout << "t char = " << typeid(char).name() << endl;

    fill_maps<double>(PredType::NATIVE_DOUBLE);
    fill_maps<long double>(PredType::NATIVE_LDOUBLE);
    fill_maps<float>(PredType::NATIVE_FLOAT);

    fill_maps<bool>(PredType::NATIVE_HBOOL);
//    fill_maps<short>(PredType::NATIVE_SHORT);
//    fill_maps<long>(PredType::NATIVE_LONG);
}

bool OutputHDF5TreeFile::is_open()
{
    return false;
}

void OutputHDF5TreeFile::close()
{

    for (auto d: m_vector_of_pointer_to_data) {
        empty_buffer_to_file(d);
        delete d;
    }
    m_vector_of_pointer_to_data.clear();
    m_file.close();

}

void OutputHDF5TreeFile::open(const std::string &s)
{
//    H5File file( s, H5F_ACC_TRUNC );
    File::open(s, std::ofstream::binary | std::fstream::out);
    m_file = H5File(s.c_str(), H5F_ACC_TRUNC);
    m_group = m_file.createGroup("table");


    m_dataspace = DataSpace(RANK, m_dims_f, m_maxdims);

//    cout << "OutputHDF5TreeFile::open" << m_file.is <<

}

void OutputHDF5TreeFile::write_header()
{
//    cout << "OutputHDF5TreeFile::write_header start" << endl;

    for (auto d: m_vector_of_pointer_to_data) {
//        cout << "\tOutputHDF5TreeFile::write_header d = " << d.name() << endl;
//        cout << "\tOutputHDF5TreeFile::fill d (id) = " << d.m_dataset_id << endl;

        //Modify dataset creation properties, i.e. enable chunking.
        DSetCreatPropList cparms;
        hsize_t chunk_dims[RANK] = {256};
        cparms.setChunk(RANK, chunk_dims);
        if (d->m_nb_characters > 0)
            cparms.setDeflate(4);
        if (
                d->m_type_index == typeid(uint64_t) || d->m_type_index == typeid(int64_t) ||
                d->m_type_index == typeid(uint32_t) || d->m_type_index == typeid(int32_t) ||
                d->m_type_index == typeid(uint16_t) || d->m_type_index == typeid(int16_t) ||
                d->m_type_index == typeid(uint8_t) || d->m_type_index == typeid(int8_t) ||
                d->m_type_index == typeid(bool) || d->m_type_index == typeid(char)
                )
            cparms.setDeflate(4);

        auto dataset = m_group.createDataSet(d->name(), d->m_type, m_dataspace, cparms);
//        cout << "\tOutputHDF5TreeFile::fill d (id) = " << d.m_dataset_id << endl;
        d->m_dataset = new DataSet(dataset);

    }

    m_write_header_called = true;



//    cout << "OutputHDF5TreeFile::write_header done" << endl;

}


void OutputHDF5TreeFile::empty_buffer_to_file(HDF5Data *d)
{

//    cout << "\t\t\tOutputHDF5TreeFile::empty_buffer_to_file m_buffer_len= " << d->m_buffer_len << endl;
    hsize_t size[RANK];
    auto s = d->m_data_len_already_saved + d->m_buffer_len;
    size[0] = s;

//    cout << "\t\t\tOutputHDF5TreeFile:: resize to  " << s  << endl;

    auto dataset = d->m_dataset;
    dataset->extend(size);
    DataSpace fspace = dataset->getSpace();
    hsize_t offset[RANK];
    offset[0] = d->m_data_len_already_saved;
    hsize_t dims[RANK] = {d->m_buffer_len};            /* data dimensions */
    fspace.selectHyperslab(H5S_SELECT_SET, dims, offset);

    hsize_t dims_f[HDF5Tree::RANK] = {d->m_buffer_len};
    auto dataspace = DataSpace(RANK, dims_f, m_maxdims);

    dataset->write(d->m_buffer, d->m_type, dataspace, fspace);
    d->m_data_len_already_saved += d->m_buffer_len;
    d->m_buffer_len = 0;

}


void OutputHDF5TreeFile::fill()
{
//    cout << "OutputHDF5TreeFile::fill start" << endl;

    if (!m_write_header_called)
        throw std::logic_error("write_header not called");


    if (m_vector_of_pointer_to_data.empty())
        return;


    for (auto d: m_vector_of_pointer_to_data) {
//        cout << "\tOutputHDF5TreeFile::fill d = " << d->name() << endl;

//        if (d->m_nb_characters == 0)

        if (d->m_buffer_len == d->MAX_BUFFER_LEN) {
            empty_buffer_to_file(d);
        }

        if (d->m_buffer_len < d->MAX_BUFFER_LEN) {
            d->fill_one_buffer();
        }
    }
    m_nb_elements++;

//    cout << "OutputHDF5TreeFile::fill done" << endl;


}

void OutputHDF5TreeFile::write_variable(const std::string &name, const void *p, std::type_index t_index)
{
    this->register_variable(name, p, t_index);
}

void OutputHDF5TreeFile::write_variable(const std::string &name, const std::string *p, size_t nb_char)
{
    this->register_variable(name, p, nb_char);
}

void OutputHDF5TreeFile::write_variable(const std::string &name, const char *p, size_t nb_char)
{
//    std::cout << "write_variable " << name << " const char *p" << std::endl;
    this->register_variable(name, p, nb_char);
}


void HDF5Tree::fill_maps(std::type_index t_index, const H5::PredType &t)
{
    m_tmap_cppToHDF5.emplace(t_index, t);

}

void HDF5Tree::register_variable(const std::string &name, const void *p, std::type_index t_index)
{
//    cout << "HDF5Tree::register_variable: name = '" << name << "' index = '" << t_index.name() << "'" << endl;
    HDF5Data *d = new HDF5Data(p, name,
                               m_tmap_cppToHDF5.at(t_index),
                               t_index, 0,
                               m_tmapOfSize.at(t_index));


    m_vector_of_pointer_to_data.push_back(d);
}

void HDF5Tree::register_variable(const std::string &name, const std::string *p, size_t nb_char)
{

//    cout << "HDF5Tree::register_variable: name = '" << name << " 'string'" << endl;
    H5::StrType t(H5::PredType::C_S1, nb_char);
    type_index t_index = typeid(string);
    auto d = new HDF5Data(p, name, t, t_index, nb_char, 0);
    m_vector_of_pointer_to_data.push_back(d);

}

void HDF5Tree::register_variable(const std::string &name, const char *p, size_t nb_char)
{
    if (!nb_char)
        throw std::out_of_range("nb_char == 0 does not make any sense");

    H5::StrType t(H5::PredType::C_S1, nb_char);
    type_index t_index = typeid(char *);
    HDF5Data *d = new HDF5Data(p, name, t, t_index, nb_char, 0);
    m_vector_of_pointer_to_data.push_back(d);
}


void HDF5Data::fill_one_buffer()
{

    if (m_buffer_len >= MAX_BUFFER_LEN)
        throw std::out_of_range("m_buffer_len >=  MAX_BUFFER_LEN");


    if (this->m_nb_characters == 0) {
        memcpy((char *) m_buffer + m_buffer_len * m_size_one_element,
               m_pointer_to_data,
               m_size_one_element);
    } else {
        if (this->m_type_index == typeid(string))
            memcpy((char *) m_buffer + m_buffer_len * m_nb_characters,
                   ((string *) m_pointer_to_data)->data(),
                   m_nb_characters);
        else {
            memcpy((char *) m_buffer + m_buffer_len * m_nb_characters,
                   m_pointer_to_data,
                   m_nb_characters);
        }
    }

    ++m_buffer_len;
}
