#pragma once
#include "Lib/stdlib.h"
#include "utils/assert.h"
#include "utils/types.h"

class String
{
  private:
	size_t m_size = 0;
	char* m_data = nullptr;
	void cleanup();

  public:
	String(const char* str);
	String(String&& other);
	String(const String& other);
	String& operator=(String&& other);
	String& operator=(const String& other);
	~String();

	String& operator+=(const String& other);
	String& operator+=(const char* other);
	String& operator+=(char other);
	String operator+(const String& other);
	char operator[](size_t off);

	String& push_back(char c);
	String& pop_back(char c);
	String& insert(size_t pos, const String& str);
	String& insert(size_t pos, const String& str, size_t subpos, size_t sublen);
	String& insert(size_t pos, const char* s);
	String& insert(size_t pos, const char* s, size_t len);

	String substr(size_t pos = 0, size_t len = 0) const;
	size_t find(const String& str, size_t pos = 0) const;
	size_t find(const char* s, size_t pos = 0) const;
	size_t find(char c, size_t pos = 0) const;
	size_t rfind(const String& str, size_t pos = 0) const;
	size_t rfind(const char* s, size_t pos = 0) const;
	size_t rfind(char c, size_t pos = 0) const;
	inline const char* c_str() const;
	inline size_t length() const;
};