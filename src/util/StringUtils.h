// Copyright 2011, University of Freiburg, Chair of Algorithms and Data
// Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <clocale>
#include <cstring>
#include <cwchar>
#include <string>
#include <sstream>
#include <vector>
#include <grp.h>
#include <unordered_set>

using std::string;
using std::vector;

namespace ad_utility {
//! Utility functions for string. Can possibly be changed to
//! a templated version using std::basic_string<T> instead of
//! std::string. However, it is not required so far.

//! Safe startWith function. Returns true iff prefix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool startsWith(const string& text, const char* prefix,
      size_t patternSize);

//! Safe startWith function. Returns true iff prefix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool startsWith(const string& text, const string& prefix);

//! Safe startWith function. Returns true iff prefix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool startsWith(const string& text, const char* prefix);

//! Safe endsWith function. Returns true iff suffix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool endsWith(const string& text, const char* suffix,
      size_t patternSize);

//! Safe endsWith function. Returns true iff suffix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool endsWith(const string& text, const string& suffix);

//! Safe endsWith function. Returns true iff suffix is a
//! prefix of text. Using a larger pattern than text.size()
//! will return false. Case sensitive.
  inline bool endsWith(const string& text, const char* suffix);

//! Case transformations. Should be thread safe.
  inline string getLowercase(const string& orig);

  inline string getUppercase(const string& orig);

  inline string getLowercaseUtf8(const string& orig);

  inline string getUppercaseUtf8(const string& orig);

  inline string firstCharToUpperUtf8(const string& orig);

//! Gets the last part of a string that is somehow split by the given separator.
  inline string getLastPartOfString(const string& text, const char separator);

  inline string getSecondLastPartOfString(const string& text, const char sep);

  inline string removeSpaces(const string& orig);

  inline string escapeForJson(const string& orig);

//! Strips any sequence of characters in s from the left and right of the text.
  inline string strip(const string& text, const string& s);

  inline string strip(const string& text, const char* s);

//! Strips any sequence of c from the left and right of the text.
  inline string strip(const string& text, char c);

//! Strips any sequence of c from the left of the text.
  inline string lstrip(const string& text, char c);

  inline string lstrip(const string& text, string s);

  inline string lstrip(const string& text, const char* s);


//! Strips any sequence of c from the tight of the text.
  inline string rstrip(const string& text, char c);

  inline string rstrip(const string& text, string s);

  inline string rstrip(const string& text, const char* s);


//! Splits a string at the separator, kinda like python.
  inline vector<string> split(const string& orig, const char sep);

//! Splits a string a any character inside the seps string.
  inline vector<string> splitAny(const string& orig, const char* seps);

  inline vector<string> splitAny(const string& orig, const string& seps);

  inline string decodeUrl(const string& orig);

// *****************************************************************************
// Definitions:
// *****************************************************************************

// ____________________________________________________________________________
  bool startsWith(const string& text, const char* prefix, size_t prefixSize) {
    if (prefixSize > text.size()) {
      return false;
    }
    for (size_t i = 0; i < prefixSize; ++i) {
      if (text[i] != prefix[i]) {
        return false;
      }
    }
    return true;
  }

// ____________________________________________________________________________
  bool startsWith(const string& text, const string& prefix) {
    return startsWith(text, prefix.data(), prefix.size());
  }

// ____________________________________________________________________________
  bool startsWith(const string& text, const char* prefix) {
    return startsWith(text, prefix, std::char_traits<char>::length(prefix));
  }

// ____________________________________________________________________________
  bool endsWith(const string& text, const char* suffix, size_t suffixSize) {
    if (suffixSize > text.size()) {
      return false;
    }
    for (size_t i = 0; i < suffixSize; ++i) {
      if (text[text.size() - (i + 1)] != suffix[suffixSize - (i + 1)]) {
        return false;
      }
    }
    return true;
  }

// ____________________________________________________________________________
  bool endsWith(const string& text, const string& suffix) {
    return endsWith(text, suffix.data(), suffix.size());
  }

// ____________________________________________________________________________
  bool endsWith(const string& text, const char* suffix) {
    return endsWith(text, suffix, std::char_traits<char>::length(suffix));
  }

// ____________________________________________________________________________
  string getLowercase(const string& orig) {
    string retVal;
    retVal.reserve(orig.size());
    for (size_t i = 0; i < orig.size(); ++i) {
      retVal += tolower(orig[i]);
    }
    return retVal;
  }

// ____________________________________________________________________________
  string getUppercase(const string& orig) {
    string retVal;
    retVal.reserve(orig.size());
    for (size_t i = 0; i < orig.size(); ++i) {
      retVal += toupper(orig[i]);
    }
    return retVal;
  }

// ____________________________________________________________________________
  string getLowercaseUtf8(const string& orig) {
    string retVal;
    retVal.reserve(orig.size());
    std::mbstate_t state = std::mbstate_t();
    char buf[MB_CUR_MAX + 1];
    for (size_t i = 0; i < orig.size(); ++i) {
      if (static_cast<int>(orig[i]) >= 0) {
        retVal += tolower(orig[i]);
      } else {
        wchar_t wChar;
        size_t len = mbrtowc(&wChar, &orig[i], MB_CUR_MAX, &state);
        // If this assertion fails, there is an invalid multi-byte character.
        // However, this usually means that the locale is not utf8.
        // Note that the default locale is always C. Main classes need to set them
        // To utf8, even if the system's default is utf8 already.
        assert(len > 0 && len <= static_cast<size_t>(MB_CUR_MAX));
        i += len - 1;
        size_t ret = wcrtomb(buf, static_cast<wchar_t>(towlower
            (static_cast<wint_t>(wChar))), &state);
        assert(ret > 0 && ret <= static_cast<size_t>(MB_CUR_MAX));
        buf[ret] = 0;
        retVal += buf;
      }
    }
    return retVal;
  }

// ____________________________________________________________________________
  string getUppercaseUtf8(const string& orig) {
    string retVal;
    retVal.reserve(orig.size());
    std::mbstate_t state = std::mbstate_t();
    char buf[MB_CUR_MAX + 1];
    for (size_t i = 0; i < orig.size(); ++i) {
      if (static_cast<int>(orig[i]) >= 0) {
        retVal += toupper(orig[i]);
      } else {
        wchar_t wChar;
        size_t len = mbrtowc(&wChar, &orig[i], MB_CUR_MAX, &state);
        // If this assertion fails, there is an invalid multi-byte character.
        // However, this usually means that the locale is not utf8.
        // Note that the default locale is always C. Main classes need to set them
        // To utf8, even if the system's default is utf8 already.
        assert(len > 0 && len <= static_cast<size_t>(MB_CUR_MAX));
        i += len - 1;
        size_t ret = wcrtomb(buf, static_cast<wchar_t>(towupper
            (static_cast<wint_t>(wChar))), &state);
        assert(ret > 0 && ret <= static_cast<size_t>(MB_CUR_MAX));
        buf[ret] = 0;
        retVal += buf;
      }
    }
    return retVal;
  }

// ____________________________________________________________________________
  inline string firstCharToUpperUtf8(const string& orig) {
    string retVal;
    retVal.reserve(orig.size());
    std::mbstate_t state = std::mbstate_t();
    char buf[MB_CUR_MAX + 1];
    size_t i = 0;
    if (orig.size() > 0) {
      if (orig[i] > 0) {
        retVal += toupper(orig[i]);
        ++i;
      } else {
        wchar_t wChar;
        size_t len = mbrtowc(&wChar, &orig[i], MB_CUR_MAX, &state);
        // If this assertion fails, there is an invalid multi-byte character.
        // However, this usually means that the locale is not utf8.
        // Note that the default locale is always C. Main classes need to set them
        // To utf8, even if the system's default is utf8 already.
        assert(len > 0 && len <= static_cast<size_t>(MB_CUR_MAX));
        i += len;
        size_t ret = wcrtomb(buf, static_cast<wchar_t>(towupper
            (static_cast<wint_t>(wChar))), &state);
        assert(ret > 0 && ret <= static_cast<size_t>(MB_CUR_MAX));
        buf[ret] = 0;
        retVal += buf;
      }
    }
    for (; i < orig.size(); ++i) {
      retVal += orig[i];
    }
    return retVal;
  }

// ____________________________________________________________________________
  string getLastPartOfString(const string& text, const char separator) {
    size_t pos = text.rfind(separator);
    if (pos != text.npos) {
      return text.substr(pos + 1);
    } else {
      return text;
    }
  }

// ____________________________________________________________________________
  string getSecondLastPartOfString(const string& text, const char separator) {
    size_t pos = text.rfind(separator);
    size_t pos2 = text.rfind(separator, pos - 1);
    if (pos2 != text.npos) {
      return text.substr(pos2 + 1, pos - (pos2 + 1));
    } else {
      return text;
    }
  }

// ____________________________________________________________________________
  string removeSpaces(const string& orig) {
    string ret;
    ret.reserve(orig.size());
    for (size_t i = 0; i < orig.size(); ++i) {
      if (orig[i] != ' ') {
        ret += orig[i];
      }
    }
    return ret;
  }

// ____________________________________________________________________________
  string escapeForJson(const string& orig) {
    string ret;
    for (size_t i = 0; i < orig.size(); ++i) {
      if (orig[i] == '\t') {
        ret += "\\t";
        continue;
      }
      if (orig[i] == '\v') {
        ret += "\\v";
        continue;
      }
      if (orig[i] == '\0') {
        ret += "\\0";
        continue;
      }
      if (orig[i] == '\f') {
        ret += "\\f";
        continue;
      }
      if (orig[i] == '\b') {
        ret += "\\b";
        continue;
      }
      if (orig[i] == '\n') {
        ret += "\\n";
        continue;
      }
      if (orig[i] == '\"' || /* orig[i] == '\'' || */ orig[i] == '\\') {
        ret += '\\';
      }
      ret += orig[i];
    }
    return ret;
  }

// _____________________________________________________________________________
  vector<string> split(const string& orig, const char sep) {
    vector<string> result;
    if (orig.size() > 0) {
      size_t from = 0;
      size_t sepIndex = orig.find(sep);
      while (sepIndex != string::npos) {
        result.emplace_back(orig.substr(from, sepIndex - from));
        from = sepIndex + 1;
        sepIndex = orig.find(sep, from);
      }
      result.emplace_back(orig.substr(from));
    }
    return result;
  }

// _____________________________________________________________________________
  vector<string> splitAny(const string& orig, const char* seps) {
    return splitAny(orig, string(seps));
  }

// _____________________________________________________________________________
  vector<string> splitAny(const string& orig, const string& seps) {
    std::unordered_set<char> chars;
    for (size_t i = 0; i < seps.size(); ++i) {
      chars.insert(seps[i]);
    }
    vector<string> result;
    if (orig.size() > 0) {
      size_t from = 0;
      size_t i = 0;
      while (i < orig.size()) {
        if (chars.count(orig[i]) > 0) {
          if (from < i) {
            result.emplace_back(orig.substr(from, i - from));
          }
          from = i + 1;
        }
        ++i;
      }
      if (from < orig.size()) {
        result.emplace_back(orig.substr(from));
      }
    }
    return result;
  }


// _____________________________________________________________________________
  inline string lstrip(const string& text, char c) {
    size_t i = 0;
    while (i < text.size() && text[i] == c) {++i;}
    return text.substr(i);
  }

// _____________________________________________________________________________
  inline string lstrip(const string& text, string s) {
    std::unordered_set<char> chars;
    for (size_t i = 0; i < s.size(); ++i) {
      chars.insert(s[i]);
    }
    size_t i = 0;
    while (i < text.size() && chars.count(text[i]) != 0) {++i;}
    return text.substr(i);
  }

// _____________________________________________________________________________
  inline string lstrip(const string& text, const char* s) {
    return lstrip(text, string(s));
  }

// _____________________________________________________________________________
  inline string rstrip(const string& text, char c) {
    size_t i = text.size();
    while (i > 0 && text[i - 1] == c) {--i;}
    return text.substr(0, i);
  }

// _____________________________________________________________________________
  inline string rstrip(const string& text, string s) {
    std::unordered_set<char> chars;
    for (size_t i = 0; i < s.size(); ++i) {
      chars.insert(s[i]);
    }
    size_t i = text.size();
    while (i > 0 && chars.count(text[i - 1]) != 0) {--i;}
    return text.substr(0, i);
  }

// _____________________________________________________________________________
  inline string rstrip(const string& text, const char* s) {
    return rstrip(text, string(s));
  }

// _____________________________________________________________________________
  inline string strip(const string& text, char c) {
    return rstrip(lstrip(text, c), c);
  }

// _____________________________________________________________________________
  string strip(const string& text, const char* s) {
    return strip(text, string(s));
  }

// _____________________________________________________________________________
  string strip(const string& text, const string& s) {
    return rstrip(lstrip(text, s), s);
  }

// _____________________________________________________________________________
  string decodeUrl(const string& url) {
    string decoded;
    for (size_t i = 0; i < url.size(); ++i) {
      if (url[i] == '+') {
        decoded += ' ';
      } else if (url[i] == '%' && i + 2 < url.size()) {
        char h1 = tolower(url[i + 1]);
        if (h1 >= '0' && h1 <= '9') {
          h1 = h1 - '0';
        } else if (h1 >= 'a' && h1 <= 'f') {
          h1 = h1 - 'a' + 10;
        } else {
          h1 = -1;
        }
        char h2 = tolower(url[i + 2]);
        if (h2 >= '0' && h2 <= '9') {
          h2 = h2 - '0';
        } else if (h2 >= 'a' && h2 <= 'f') {
          h2 = h2 - 'a' + 10;
        } else {
          h2 = -1;
        }
        if (h1 != -1 && h2 != -1) {
          decoded += static_cast<char>(h1 * 16 + h2);
          i += 2;
        } else {
          decoded += '%';
        }
      } else {
        decoded += url[i];
      }
    }
    return decoded;
  }

}
