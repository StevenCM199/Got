/* MD5
 converted to C++ class by Frank Thilo (thilo@unix-ag.org)
 for bzflag (http://www.bzflag.org)
 
   based on:
 
   md5.h and md5.c
   reference implementation of RFC 1321
 
   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.
 
License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.
 
License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.
 
RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.
 
These notices must be retained in any copies of any part of this
documentation and/or software.
 
*/
 
#ifndef BZF_MD5_H
#define BZF_MD5_H
 
#include <cstring>
#include <iostream>
 
 
// a small class for calculating MD5 hashes of strings or byte arrays
// it is not meant to be fast or secure
//
// usage: 1) feed it blocks of uchars with update()
//      2) finalize()
//      3) get hexdigest() string
//      or
//      MD5(std::string).hexdigest()
//
// assumes that char is 8 bit and int is 32 bit

//codigo de http://www.zedwood.com/article/cpp-md5-function
/**
 * @brief MD5 Algorithm class
 * @author http://www.zedwood.com/article/cpp-md5-function
 */
class MD5
{
public:
  typedef unsigned int size_type; // must be 32bit
  /**
   * @brief Construct a new MD5 object
   * @author http://www.zedwood.com/article/cpp-md5-function
   */
  MD5();
  /**
   * @brief Construct a new MD5 object
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param text to generate MD5 string
   */
  MD5(const std::string& text);
  /**
   * @brief  MD5 block update operation. Continues an MD5 message-digest operation, processing another message block
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param buf 
   * @param length size
   */
  void update(const unsigned char *buf, size_type length);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param buf 
   * @param length 
   */
  void update(const char *buf, size_type length);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @return MD5& 
   */
  MD5& finalize();
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @return std::string 
   */
  std::string hexdigest() const;
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param md5 
   * @return std::ostream& 
   */
  friend std::ostream& operator<<(std::ostream&, MD5 md5);
 
private:
/**
 * @brief 
 * @author http://www.zedwood.com/article/cpp-md5-function
 */
  void init();
  typedef unsigned char uint1; //  8bit
  typedef unsigned int uint4;  // 32bit
  enum {blocksize = 64}; // VC6 won't eat a const static int here
 /**
  * @brief 
  * @author http://www.zedwood.com/article/cpp-md5-function
  * @param block 
  */
  void transform(const uint1 block[blocksize]);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param output 
   * @param input 
   * @param len 
   */
  static void decode(uint4 output[], const uint1 input[], size_type len);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param output 
   * @param input 
   * @param len 
   */
  static void encode(uint1 output[], const uint4 input[], size_type len);
 
  bool finalized;
  uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
  uint4 count[2];   // 64bit counter for number of bits (lo, hi)
  uint4 state[4];   // digest so far
  uint1 digest[16]; // the result
 
  // low level logic operations
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param x 
   * @param y 
   * @param z 
   * @return uint4 
   */
  static inline uint4 F(uint4 x, uint4 y, uint4 z);

  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param x 
   * @param y 
   * @param z 
   * @return uint4 
   */
  static inline uint4 G(uint4 x, uint4 y, uint4 z);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param x 
   * @param y 
   * @param z 
   * @return uint4 
   */
  static inline uint4 H(uint4 x, uint4 y, uint4 z);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param x 
   * @param y 
   * @param z 
   * @return uint4 
   */
  static inline uint4 I(uint4 x, uint4 y, uint4 z);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param x 
   * @param n 
   * @return uint4 
   */
  static inline uint4 rotate_left(uint4 x, int n);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param a 
   * @param b 
   * @param c 
   * @param d 
   * @param x 
   * @param s 
   * @param ac 
   */
  static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param a 
   * @param b 
   * @param c 
   * @param d 
   * @param x 
   * @param s 
   * @param ac 
   */
  static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param a 
   * @param b 
   * @param c 
   * @param d 
   * @param x 
   * @param s 
   * @param ac 
   */
  static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  /**
   * @brief 
   * @author http://www.zedwood.com/article/cpp-md5-function
   * @param a 
   * @param b 
   * @param c 
   * @param d 
   * @param x 
   * @param s 
   * @param ac 
   */
  static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};
 /**
  * @brief transforms string to a md5 version of the string
  * @param str string to md5
  * @author http://www.zedwood.com/article/cpp-md5-function
  * @return std::string md5 of the string input
  */
std::string md5(const std::string str);
 
#endif