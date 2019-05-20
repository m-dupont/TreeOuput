//
// Created by mdupont on 04/04/19.
//

#pragma once

#include <stdexcept>

class FileException : public std::runtime_error {
 public:
  explicit FileException(const std::string &arg);

};


class ClosedFileException: public FileException {
 public:
  explicit ClosedFileException(const std::string &arg);

};

class ModeFileException: public FileException {
 public:
  explicit ModeFileException(const std::string &arg);
};

class HeaderException: public FileException {
 public:
  explicit HeaderException(const std::string &arg);

};

class MissingHeaderException: public HeaderException {
 public:
  explicit MissingHeaderException(const std::string &arg);

};

class MalFormedHeaderException: public HeaderException {
 public:
  explicit MalFormedHeaderException(const std::string &arg);

};


class KeyAlreadyExistsException: public HeaderException {
 public:
  explicit KeyAlreadyExistsException(const std::string &arg);

};

class KeyNotFoundInHeaderException: public HeaderException {
public:
  explicit KeyNotFoundInHeaderException(const std::string &arg);
};

class TypeMismatchHeaderException: public HeaderException {
public:
  explicit TypeMismatchHeaderException(const std::string &arg);
};


class NoTypeInHeaderException: public HeaderException {
public:
  explicit NoTypeInHeaderException(const std::string &arg);

};



class ManagerException: public FileException {
public:
  explicit ManagerException(const std::string &arg);
};

class UnknownKindManagerException: public ManagerException {
public:
  explicit UnknownKindManagerException(const std::string &arg);
};



