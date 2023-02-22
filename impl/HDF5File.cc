//
// Created by Mathieu Dupont <mdupont@cppm.in2p3.fr> on 20/02/23.
// Copyright (c) 2023 -- Aix Marseille Univ, CNRS/IN2P3, CPPM, Marseille, France -- All rights reserved.
//

#include <cstring>
#include "HDF5File.hh"
using namespace H5;
using namespace std;

bool OutputHDF5TreeFile::is_open()
{
    return false;
}

void OutputHDF5TreeFile::close()
{

    m_vector_of_pointer_to_data.clear();
    m_file.close();

}

void OutputHDF5TreeFile::open(const std::string &s)
{
//    H5File file( s, H5F_ACC_TRUNC );
    File::open(s, std::ofstream::binary | std::fstream::out);
    m_file = H5File(s.c_str(),  H5F_ACC_TRUNC);
    m_group = m_file.createGroup("table");


    m_dataspace = DataSpace(RANK, m_dims_f, m_maxdims);

//    cout << "OutputHDF5TreeFile::open" << m_file.is <<

}

void OutputHDF5TreeFile::write_header()
{
//    cout << "OutputHDF5TreeFile::write_header start" << endl;

    for (auto&& d : m_vector_of_pointer_to_data) // access by const reference
    {
//        cout << "\tOutputHDF5TreeFile::write_header d = " << d.name() << endl;
//        cout << "\tOutputHDF5TreeFile::fill d (id) = " << d.m_dataset_id << endl;

        //Modify dataset creation properties, i.e. enable chunking.
        DSetCreatPropList cparms;
        hsize_t      chunk_dims[RANK] ={128};
        cparms.setChunk( RANK, chunk_dims );

        auto dataset = m_group.createDataSet(d.name(), d.m_type, m_dataspace, cparms);
//        cout << "\tOutputHDF5TreeFile::fill d (id) = " << d.m_dataset_id << endl;
        d.m_dataset = new DataSet(dataset);

    }

    m_write_header_called = true;



//    cout << "OutputHDF5TreeFile::write_header done" << endl;

}

void OutputHDF5TreeFile::fill()
{
//    cout << "OutputHDF5TreeFile::fill start" << endl;

    if(!m_write_header_called)
        throw std::logic_error("write_header not called");


    if (m_vector_of_pointer_to_data.empty())
        return;

    hsize_t size[RANK];
    size[0] = m_nb_elements + 1;

    for (auto&& d : m_vector_of_pointer_to_data) // access by const reference
    {
//        cout << "\tOutputHDF5TreeFile::fill d = " << d.name() << endl;
//        cout << "\tOutputHDF5TreeFile::fill d (id) = " << d.m_dataset_id << endl;

//        auto dataset = m_group.openDataSet(d.name());
        auto dataset = d.m_dataset;

//        auto dataset = DataSet(d.m_dataset_id);
        dataset->extend(size);

        DataSpace fspace = dataset->getSpace();
        hsize_t     offset[RANK];
        offset[0] = m_nb_elements ;
        hsize_t      dims[RANK] = { 1};            /* data dimensions */
        fspace.selectHyperslab( H5S_SELECT_SET, dims, offset );

//        DataSpace dataspace( RANK, m_dims_f );

        if (d.m_nb_characters == 0)
        {
            dataset->write(d.m_pointer_to_data, d.m_type, m_dataspace, fspace);
        } else
        {
            char *p_data = (char*)d.m_pointer_to_data;
            auto current_nb_characters = strlen(p_data);
//            cout << " p_data = " << p_data << endl;
//            for(size_t i = current_nb_characters; i < d.m_nb_characters; ++i)
//                p_data[i] = '\0';
//            cout << " p_data = " << p_data << endl;


            string s(p_data); // copy of the data, :-(
            s.resize(current_nb_characters , '\0');


            dataset->write(s, d.m_type, m_dataspace, fspace);
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
    this->register_variable(name, p);
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
    HDF5Data d(p, name, m_tmap_cppToHDF5.at(t_index), t_index);


    m_vector_of_pointer_to_data.push_back(d);
}

void HDF5Tree::register_variable(const std::string &name, const std::string *p, size_t nb_char)
{

}

void HDF5Tree::register_variable(const std::string &name, const char *p, size_t nb_char)
{
    if(!nb_char)
        throw std::out_of_range("nb_char == 0 does not make any sense");

    register_variable(name, p, typeid(char*));
    auto&& data = m_vector_of_pointer_to_data.back();
    data.m_nb_characters = nb_char;
}

HDF5Tree::HDF5Tree() :  m_nb_elements(0)
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
    m_tmap_cppToHDF5.emplace(typeid(char*), t);
    m_tmap_cppToHDF5.emplace(typeid(char[]), t);

//    cout << "t char[] = " << typeid(char[]).name() << endl;
//    cout << "t char* = " << typeid(char*).name() << endl;
//    cout << "t char = " << typeid(char).name() << endl;

    fill_maps<double>(PredType::NATIVE_DOUBLE);
    fill_maps<long double>(PredType::NATIVE_LDOUBLE);
    fill_maps<float>(PredType::NATIVE_FLOAT);
//    fill_maps<short>(PredType::NATIVE_SHORT);
//    fill_maps<long>(PredType::NATIVE_LONG);
}
