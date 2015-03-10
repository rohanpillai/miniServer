#include <http.h>


 HTTPRequest::HTTPRequest(string mthd, string URI, string vrsn, map<string, string> opts) {
  method = mthd;
  uri = URI;
  version = vrsn;
  options = opts;
  badRequest = false;
}

HTTPRequest::HTTPRequest() {
  badRequest = true;
}

void HTTPRequest::printValues() {
  cout << "Method " << method << endl << "URI " << uri << endl << "version " << version << endl;
  cout << "Map size " << options.size() << endl;
  
  for (map<string, string>::iterator ii = options.begin(); ii != options.end(); ii++) {
    cout << (*ii).first << ": " << (*ii).second << endl;
  }
}

string HTTPRequest::getMethod() {
  return method;
}

string HTTPRequest::getURI() {
  return uri;
}

string HTTPRequest::getVersion() {
  return version;
}

string HTTPRequest::getOption(string key) {
  return options[key];
}

bool HTTPRequest::isBad() {
  return badRequest;
}

HTTP::HTTP(int socket_fd) {
  sockfd = socket_fd;
  int status = process_request();
}

void HTTP::setVersion(string vrsn) {
  version = vrsn;
}

bool HTTP::persist() {
  return persistent;
}

void HTTP::closeSocket() {
  close(sockfd);
}

HTTPRequest* process_input(const char* msg) {
  istringstream iss(msg);
  string line;
  getline(iss, line);
  ostringstream ostream;
  vector<string> request_line;
  for (string::iterator it = line.begin(); it != line.end(); it++) {
    if (*it == ' ') {
      request_line.push_back(ostream.str());
      ostream.str("");
      ostream.clear();
    } else {
      ostream << *it;
    }
  }
  request_line.push_back(ostream.str());

  if (request_line.size() != 3) {
    cout << "The request line should have the following format: METHOD SP Request-URI SP HTTP-Version CRLF" << endl;
    return (new HTTPRequest());
  }

  string method = request_line[0];
  string uri = request_line[1];
  string version  = request_line[2];

  map<string, string> options;

  while(!iss.eof()) {
    getline(iss, line);
//    if (line.empty()) {
    if (!eligibleHeaderLine(line)) {
      break;
    }
    ostringstream oss;
    vector<string> option;
    bool flag = true;
    for (string::iterator it = line.begin(); it != line.end(); it++) {
      if (*it == ':' && flag) {
        option.push_back(oss.str());
        oss.str("");
        oss.clear();
        flag = false;
        continue;
      } else {
        oss << *it;
      }
    }
    option.push_back(oss.str());
    if (option.size() != 2) {
      cout << "Options need to have the following format: FIELD_NAME: VALUE" << endl << line << endl;
      return (new HTTPRequest());
    }
    options[option[0]] = option[1];
     
  //  cout << line << endl;
  }
  return (new HTTPRequest(method, uri, version, options));
}

void HTTP::sendResource(string resource) {
  ifstream file(resource.c_str(), ifstream::in);
  if (file) {
    int length = file.tellg();
    file.seekg(0, ios::end);
    length = file.tellg() - length;
    file.seekg(0, ios::beg);

    string status_line = version + " 200 OK\r\n";
    string connection;
    if (persistent) {
      connection = "Connection: keep-alive\r\n";
    } else {
      connection = "Connection: close\r\n";
    }

    time_t t = time(0);
    struct tm * now = localtime(&t);
    string date = "Date: " + tmToString(now);
    string server = "Server: beast\r\n";

    struct tm* lastModified;
    struct stat attrib;
    stat(resource.c_str(), &attrib);
    lastModified = gmtime(&(attrib.st_mtime));

    string lastModifiedString = "Last-Modified: " + tmToString(lastModified);
    ostringstream tmpss;
    tmpss << length;
    string contentLength = "Content-Length: " + tmpss.str() + "\r\n";
    string contentType = "Content-Type: text/html\r\n";
    
    string header = status_line + connection + date + server + lastModifiedString + contentLength + contentType + "\r\n";
    cout << header; 
   
    int total_length = header.length() + length; 
    char* buffer = (char *) malloc(total_length);
    header.copy(buffer, header.length());
    file.read(buffer + header.length(), length);

    int current_ptr = 0;
//    while (current_ptr < length) {
    int bytes_send = send(sockfd, buffer + current_ptr, total_length, 0);
//      cout << "Bytes send " << bytes_send << endl;
//      current_ptr = current_ptr + bytes_send;
      
//    }
    free(buffer);
  } else {
    cout << "in 404\n";
    string status_line = "HTTP/1.1 404 Not Found\r\n";
    string connection = "Connection: close\r\n";
    time_t t = time(0);
    struct tm* now = localtime(&t);
    string date = "Date: " + tmToString(now);
    string server = "Server: beast\r\n";
    string header = status_line + connection + date + server + "\r\n";
    int bytes_send = send(sockfd, header.c_str(), header.length(), 0);
  }
}


int HTTP::process_request() {

  char *buffer = (char *) malloc (BUFFER_SIZE);
  do {
    int len = BUFFER_SIZE;
    int read_len = recv(sockfd, (void *)buffer, len, 0);
  
    if (read_len < 0) {
      cout << "Error occured while reading. Closing application\n";
      return -1;
    }
    if (read_len == 0) {
      cout << "Connection closed by client\n";
      return -2;
    }
  
    HTTPRequest* request = process_input(buffer);
    if (request->isBad()) {
        cout << "Send bad request\n";
        return -3; 
    }

    string connection = request->getOption("Connection");
    cout << endl << connection << endl;
    if (connection.empty()) {
      persistent = true;
    } else {
      if (connection == " keep-alive\r") {
        persistent = true;
      } else {
        if (connection == " close\r") {
          persistent = false;
        } else {
          cout << "Bad value for connection. Expected: Connection: SP close | keep-alive";
          return -4;
        }
      }
    }
    setVersion(request->getVersion());
    string resource = request->getURI();
    if (request->getMethod() == "GET") {
        sendResource(resource);
    } else {
      cout << "Option not available yet\n";
    }
  } while (persist());
  closeSocket();
  free(buffer);
  return 0;
}

