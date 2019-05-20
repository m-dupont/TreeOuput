//
// Created by mdupont on 04/04/19.
//

#include "FileExceptions.hh"
FileException::FileException(const std::string &arg) : runtime_error(arg)
{}
ClosedFileException::ClosedFileException(const std::string &arg) : FileException(arg)
{}
HeaderException::HeaderException(const std::string &arg) : FileException(arg)
{}
MissingHeaderException::MissingHeaderException(const std::string &arg) : HeaderException(arg)
{}
MalFormedHeaderException::MalFormedHeaderException(const std::string &arg) : HeaderException(arg)
{}
KeyAlreadyExistsException::KeyAlreadyExistsException(const std::string &arg) : HeaderException(arg)
{}
ModeFileException::ModeFileException(const std::string &arg) : FileException(arg)
{}

ManagerException::ManagerException(const std::string &arg) : FileException(arg)
{}

UnknownKindManagerException::UnknownKindManagerException(const std::string &arg) : ManagerException(arg)
{}

KeyNotFoundInHeaderException::KeyNotFoundInHeaderException(const std::string &arg) : HeaderException(arg)
{}

TypeMismatchHeaderException::TypeMismatchHeaderException(const std::string &arg) : HeaderException(arg)
{}

NoTypeInHeaderException::NoTypeInHeaderException(const std::string &arg) : HeaderException(arg)
{}
