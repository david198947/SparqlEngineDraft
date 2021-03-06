// Copyright 2011, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold <buchholb>

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string>


using std::string;

namespace ad_utility {
static const int MAX_NOF_CONNECTIONS = 500;
static const int RECIEVE_BUFFER_SIZE = 10000;

//! Basic Socket class used by the server code of the semantic search.
//! Wraps low-level socket calls should possibly be replaced by
//! different implementations and wrap them instead.
class Socket {
  public:

    // Default ctor.
    Socket() :
        _fd(-1) {
    }

    // Destructor, close the socket if open.
    ~Socket() {
      if (isOpen()) ::close(_fd);
    }

    //! Create the socket.
    bool create() {
      _fd = socket(AF_INET, SOCK_STREAM, 0);
      if (!isOpen()) return false;
      // Make sockets reusable immediately after closing
      // Copied from CompletionServer code.
      int rc; /* return code */
      int on = 1;
      rc = setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
      if (rc < 0) perror("! WARNING: setsockopt(SO_REUSEADDR) failed");
      return true;
    }

    //! Bind the socket to the given port.
    bool bind(const int port) {
      if (!isOpen()) return false;

      _address.sin_family = AF_INET;
      _address.sin_addr.s_addr = INADDR_ANY;
      _address.sin_port = htons(port);

      return ::bind(_fd, (struct sockaddr *) &_address,
          sizeof(_address)) != -1;
    }

    //! Make it a listening socket.
    bool listen() const {
      if (!isOpen()) return false;
      return ::listen(_fd, MAX_NOF_CONNECTIONS) != -1;
    }

    //! Accept a connection.
    bool acceptClient(Socket* other) {
      int addressSize = sizeof(_address);
      other->_fd = ::accept(_fd, reinterpret_cast<sockaddr*>(&_address),
          reinterpret_cast<socklen_t *>(&addressSize));

      return other->isOpen();
    }

    //! State if the socket's file descriptor is valid.
    bool isOpen() const {
      return _fd != -1;
    }

    //! Send some string.
    bool send(const std::string& data) const {
      return ::send(_fd, data.c_str(), data.size(), MSG_NOSIGNAL) != -1;
    }

    //! Recieve something.
    int recieve(std::string* data) const {
      char buf[RECIEVE_BUFFER_SIZE + 1];
      int status = ::recv(_fd, buf, RECIEVE_BUFFER_SIZE, 0);
      if (status > 0) {
        *data = buf;
      }
      return status;
    }

    // Copied from online sources. Might be useful in the future.
//    void setNonBlocking(const bool val)
//    {
//      int opts = fcntl(_fd, F_GETFL);
//      if (opts < 0)
//      {
//        return;
//      }
//
//      if (val) opts = (opts | O_NONBLOCK);
//      else opts = (opts & ~O_NONBLOCK);
//
//      fcntl(_fd, F_SETFL, opts);
//    }

  private:
    sockaddr_in _address;
    int _fd;
};
}


