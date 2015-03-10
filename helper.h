#ifndef __HELPER_INCLUDED__
#define __HELPER_INCLUDED__

#include <string>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>

using namespace std;

static bool eligibleHeaderLine(string line) {
  const char *str = line.c_str();
  int ascii = (int) *str;
  if ((ascii > 40 && ascii < 91) || (ascii > 96 && ascii < 123)) {
    return true;
  }
  return false;
}

vector<string *> split(char* buffer, int len, char delim = ' ') {
  ostringstream ofs;
  vector<string *> array;
  int i=0, j=0;
  bool flag = false;

  for (int i=0; i<len; i++) {
    if (*(buffer + i) == delim) {
      if (flag) {    
        array.push_back(new string((buffer + j), i-j));
        flag = false;
      }
      continue;
    }
    if (!flag) {
      flag = true;
      j = i;
    }
  }
}

static string tmToString(struct tm* time) {
  ostringstream oss;
  switch(time->tm_wday) {
    case 0: 
      oss << "Sun, ";
      break;
    case 1:
      oss << "Mon, ";
      break;
    case 2: 
      oss << "Tue, ";
      break;
    case 3: 
      oss << "Wed, ";
      break;
    case 4: 
      oss << "Thu, ";
      break;
    case 5: 
      oss << "Fri, ";
      break;
    case 6: 
      oss << "Sat, ";
      break;
  }
  oss << setfill('0') << setw(2) << time->tm_mday;
  switch(time->tm_mon) {
    case 0:
      oss << " Jan ";
      break;
    case 1:
      oss << " Feb ";
      break;
    case 2:
      oss << " Mar ";
      break;
    case 3:
      oss << " Apr ";
      break;
    case 4:
      oss << " May ";
      break;
    case 5:
      oss << " Jun ";
      break;
    case 6:
      oss << " Jul ";
      break;
    case 7:
      oss << " Aug ";
      break;
    case 8:
      oss << " Sep ";
      break;
    case 9:
      oss << " Oct ";
      break;
    case 10:
      oss << " Nov ";
      break;
    case 11:
      oss << " Dec ";
      break;
  }
  oss << 1900 + time->tm_year;
  oss << " ";
  oss << setfill('0') << setw(2) << time->tm_hour;
  oss << ":";
  oss << setfill('0') << setw(2) << time->tm_min;
  oss << ":";
  oss << setfill('0') << setw(2) << time->tm_sec;
  oss << " GMT\r\n";
  return oss.str();
}

#endif

