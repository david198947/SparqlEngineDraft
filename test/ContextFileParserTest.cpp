// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include "../src/parser/ContextFileParser.h"

TEST(ContextFileParserTest, getLineTest) {

  char* locale = setlocale(LC_CTYPE, "en_US.utf8");
  std::cout << "Set locale LC_CTYPE to: " << locale << std::endl;

  std::fstream f("_testtmp.contexts.tsv", std::ios_base::out);
  f << "Foo\t0\t0\t2\n"
      "foo\t0\t0\t2\n"
      "Bär\t1\t0\t1\n"
      "Äü\t0\t0\t1\n"
      "X\t0\t1\t1\n";

  f.close();
  ContextFileParser p("_testtmp.contexts.tsv");
  ContextFileParser::Line a;
  ASSERT_TRUE(p.getLine(a));
  ASSERT_EQ( "foo", a._word);
  ASSERT_FALSE(a._isEntity);
  ASSERT_EQ(0, a._contextId);
  ASSERT_EQ(2, a._score);

  ASSERT_TRUE(p.getLine(a));
  ASSERT_EQ( "foo", a._word);
  ASSERT_FALSE(a._isEntity);
  ASSERT_EQ(0, a._contextId);
  ASSERT_EQ(2, a._score);

  ASSERT_TRUE(p.getLine(a));
  ASSERT_EQ( "Bär", a._word);
  ASSERT_TRUE(a._isEntity);
  ASSERT_EQ(0, a._contextId);
  ASSERT_EQ(1, a._score);

  ASSERT_TRUE(p.getLine(a));
  ASSERT_EQ( "äü", a._word);
  ASSERT_FALSE(a._isEntity);
  ASSERT_EQ(0, a._contextId);
  ASSERT_EQ(1, a._score);

  ASSERT_TRUE(p.getLine(a));
  ASSERT_EQ( "x", a._word);
  ASSERT_FALSE(a._isEntity);
  ASSERT_EQ(1, a._contextId);
  ASSERT_EQ(1, a._score);

  ASSERT_FALSE(p.getLine(a));
  remove("_testtmp.contexts.tsv");
};


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

