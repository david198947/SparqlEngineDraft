// Copyright 2011, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold <buchholb>

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../util/Socket.h"
#include "../util/Timer.h"
#include "../parser/SparqlParser.h"
#include "../index/Index.h"
#include "../engine/Engine.h"
#include "../engine/QueryGraph.h"
#include "../parser/ParseException.h"

using std::string;
using std::vector;
using std::unordered_map;

using ad_utility::Socket;

//! The HTTP Sever used.
class Server {
public:
  explicit Server(const int port)
      :
      _serverSocket(), _port(port), _index(), _engine(), _initialized(false) {
  }

  typedef unordered_map<string, string> ParamValueMap;

  // Initialize the server.
  void initialize(const string& ontologyBaseName, bool useText);

  //! Loop, wait for requests and trigger processing.
  void run();

private:
  Socket _serverSocket;
  int _port;
  Index _index;
  Engine _engine;

  bool _initialized;

  void process(Socket* client, QueryExecutionContext* qec) const;
  void serveFile(Socket* client, const string& requestedFile) const;

  ParamValueMap parseHttpRequest(const string& request) const;

  string createQueryFromHttpParams(const ParamValueMap& params) const;

  string createHttpResponse(const string& content,
      const string& contentType) const;

  string composeResponseJson(const ParsedQuery& query,
      const QueryExecutionTree& qet) const;

  string composeResponseJson(const string& query,
      const ad_semsearch::Exception& e) const;

  string composeResponseJson(const string& query,
      const ParseException& e) const;



  mutable ad_utility::Timer _requestProcessingTimer;
};