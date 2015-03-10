#ifndef __HTTP_INCLUDED__
#define __HTTP_INCLUDED__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <map>
#include <utility>
#include <vector>
#include <helper.h>

#define LINE_SIZE 1024
#define BUFFER_SIZE 2048

using namespace std;

enum HTTPStatus {
  OK,
  MOVED,
  BADREQUEST,
  NOTFOUND,
  NOTSUPPORTED
};

class HTTPRequest {
  string method, uri, version;
  map<string, string> options;
  bool badRequest;

 public:
  HTTPRequest(string, string, string, map<string, string>);
  HTTPRequest();
  void printValues();
  string getMethod();
  string getURI();
  string getVersion();
  string getOption(string);
  bool isBad();
  
};

class HTTP {
  int sockfd;
  string version;
  bool persistent;
 public:
  HTTP(int);
  void setVersion(string);
  int process_request();
  void sendResource(string);
  bool persist();
  void closeSocket();
};

#endif
