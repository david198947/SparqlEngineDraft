// Copyright 2014, University of Freiburg, Chair of Algorithms and Data
// Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)

#include <gtest/gtest.h>
#include "../src/parser/SparqlParser.h"
#include "../src/util/Exception.h"

TEST(ParserTest, testParse) {
  try {
    ParsedQuery pq = SparqlParser::parse("SELECT ?x WHERE {?x ?y ?z}");
    ASSERT_GT(pq.asString().size(), 0);
    ASSERT_EQ(0, pq._prefixes.size());
    ASSERT_EQ(1, pq._selectedVariables.size());
    ASSERT_EQ(1, pq._whereClauseTriples.size());

    pq = SparqlParser::parse(
        "PREFIX : <http://rdf.myprefix.com/>\n"
            "PREFIX ns: <http://rdf.myprefix.com/ns/>\n"
            "PREFIX xxx: <http://rdf.myprefix.com/xxx/>\n"
            "SELECT ?x ?z \n "
            "WHERE \t {?x :myrel ?y. ?y ns:myrel ?z.?y nsx:rel2 <http://abc.de>}");
    ASSERT_EQ(3, pq._prefixes.size());
    ASSERT_EQ(2, pq._selectedVariables.size());
    ASSERT_EQ(3, pq._whereClauseTriples.size());

    ASSERT_EQ("", pq._prefixes[0]._prefix);
    ASSERT_EQ("<http://rdf.myprefix.com/>", pq._prefixes[0]._uri);
    ASSERT_EQ("ns", pq._prefixes[1]._prefix);
    ASSERT_EQ("<http://rdf.myprefix.com/ns/>", pq._prefixes[1]._uri);
    ASSERT_EQ("?x", pq._selectedVariables[0]);
    ASSERT_EQ("?z", pq._selectedVariables[1]);
    ASSERT_EQ("?x", pq._whereClauseTriples[0]._s);
    ASSERT_EQ(":myrel", pq._whereClauseTriples[0]._p);
    ASSERT_EQ("?y", pq._whereClauseTriples[0]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[1]._s);
    ASSERT_EQ("ns:myrel", pq._whereClauseTriples[1]._p);
    ASSERT_EQ("?z", pq._whereClauseTriples[1]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[2]._s);
    ASSERT_EQ("nsx:rel2", pq._whereClauseTriples[2]._p);
    ASSERT_EQ("<http://abc.de>", pq._whereClauseTriples[2]._o);
    ASSERT_EQ("", pq._limit);
    ASSERT_EQ("", pq._offset);

    pq = SparqlParser::parse(
        "PREFIX : <http://rdf.myprefix.com/>\n"
            "PREFIX ns: <http://rdf.myprefix.com/ns/>\n"
            "PREFIX xxx: <http://rdf.myprefix.com/xxx/>\n"
            "SELECT ?x ?z \n "
            "WHERE \t {\n?x :myrel ?y. ?y ns:myrel ?z.\n?y nsx:rel2 <http://abc.de>\n}");
    ASSERT_EQ(3, pq._prefixes.size());
    ASSERT_EQ(2, pq._selectedVariables.size());
    ASSERT_EQ(3, pq._whereClauseTriples.size());

    ASSERT_EQ("", pq._prefixes[0]._prefix);
    ASSERT_EQ("<http://rdf.myprefix.com/>", pq._prefixes[0]._uri);
    ASSERT_EQ("ns", pq._prefixes[1]._prefix);
    ASSERT_EQ("<http://rdf.myprefix.com/ns/>", pq._prefixes[1]._uri);
    ASSERT_EQ("?x", pq._selectedVariables[0]);
    ASSERT_EQ("?z", pq._selectedVariables[1]);
    ASSERT_EQ("?x", pq._whereClauseTriples[0]._s);
    ASSERT_EQ(":myrel", pq._whereClauseTriples[0]._p);
    ASSERT_EQ("?y", pq._whereClauseTriples[0]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[1]._s);
    ASSERT_EQ("ns:myrel", pq._whereClauseTriples[1]._p);
    ASSERT_EQ("?z", pq._whereClauseTriples[1]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[2]._s);
    ASSERT_EQ("nsx:rel2", pq._whereClauseTriples[2]._p);
    ASSERT_EQ("<http://abc.de>", pq._whereClauseTriples[2]._o);
    ASSERT_EQ("", pq._limit);
    ASSERT_EQ("", pq._offset);

    pq = SparqlParser::parse(
        "PREFIX ns: <http://ns/>"
            "SELECT ?x ?z \n "
            "WHERE \t {\n?x <Directed_by> ?y. ?y ns:myrel.extend ?z.\n"
            "?y nsx:rel2 \"Hello... World\"}");
    ASSERT_EQ(1, pq._prefixes.size());
    ASSERT_EQ(2, pq._selectedVariables.size());
    ASSERT_EQ(3, pq._whereClauseTriples.size());

    pq.expandPrefixes();

    ASSERT_EQ("?x", pq._selectedVariables[0]);
    ASSERT_EQ("?z", pq._selectedVariables[1]);
    ASSERT_EQ("?x", pq._whereClauseTriples[0]._s);
    ASSERT_EQ("<Directed_by>", pq._whereClauseTriples[0]._p);
    ASSERT_EQ("?y", pq._whereClauseTriples[0]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[1]._s);
    ASSERT_EQ("<http://ns/myrel.extend>", pq._whereClauseTriples[1]._p);
    ASSERT_EQ("?z", pq._whereClauseTriples[1]._o);
    ASSERT_EQ("?y", pq._whereClauseTriples[2]._s);
    ASSERT_EQ("nsx:rel2", pq._whereClauseTriples[2]._p);
    ASSERT_EQ("\"Hello... World\"", pq._whereClauseTriples[2]._o);
    ASSERT_EQ("", pq._limit);
    ASSERT_EQ("", pq._offset);


    pq = SparqlParser::parse(
        "SELECT ?x ?y WHERE {?x is-a Actor .  FILTER(?x != ?y)."
            "?y is-a Actor . FILTER(?y < ?x)} LIMIT 10");
    pq.expandPrefixes();
    ASSERT_EQ(2, pq._filters.size());
    ASSERT_EQ("?x", pq._filters[0]._lhs);
    ASSERT_EQ("?y", pq._filters[0]._rhs);
    ASSERT_EQ(SparqlFilter::FilterType::NE, pq._filters[0]._type);
    ASSERT_EQ("?y", pq._filters[1]._lhs);
    ASSERT_EQ("?x", pq._filters[1]._rhs);
    ASSERT_EQ(SparqlFilter::FilterType::LT, pq._filters[1]._type);
    ASSERT_EQ(2, pq._whereClauseTriples.size());

    pq = SparqlParser::parse(
        "SELECT ?x ?y WHERE {?x is-a Actor .  FILTER(?x != ?y)."
            "?y is-a Actor} LIMIT 10");
    pq.expandPrefixes();
    ASSERT_EQ(1, pq._filters.size());
    ASSERT_EQ("?x", pq._filters[0]._lhs);
    ASSERT_EQ("?y", pq._filters[0]._rhs);
    ASSERT_EQ(SparqlFilter::FilterType::NE, pq._filters[0]._type);
    ASSERT_EQ(2, pq._whereClauseTriples.size());


    pq = SparqlParser::parse(
        "SELECT ?x ?y WHERE {?x is-a Actor .  FILTER(?x != ?y)."
            "?y is-a Actor. ?x <in-context> ?c."
            "?c <in-context> coca* abuse} LIMIT 10");
    pq.expandPrefixes();
    ASSERT_EQ(1, pq._filters.size());
    ASSERT_EQ("?x", pq._filters[0]._lhs);
    ASSERT_EQ("?y", pq._filters[0]._rhs);
    ASSERT_EQ(SparqlFilter::FilterType::NE, pq._filters[0]._type);
    ASSERT_EQ(4, pq._whereClauseTriples.size());
    ASSERT_EQ("?x", pq._whereClauseTriples[2]._s);
    ASSERT_EQ("<in-context>", pq._whereClauseTriples[2]._p);
    ASSERT_EQ("?c", pq._whereClauseTriples[2]._o);
    ASSERT_EQ("?c", pq._whereClauseTriples[3]._s);
    ASSERT_EQ("<in-context>", pq._whereClauseTriples[3]._p);
    ASSERT_EQ("coca* abuse", pq._whereClauseTriples[3]._o);

    pq = SparqlParser::parse(
        "PREFIX : <>\n"
            "SELECT ?x ?y ?z TEXT(?c) SCORE(?c) ?c WHERE {\n"
            "?x :is-a :Politician .\n"
            "?x :in-context ?c .\n"
            "?c :in-context friend .\n"
            "?c :in-context ?y .\n"
            "?y :is-a :Scientist .\n"
            "FILTER(?x != ?y) .\n"
            "} ORDER BY ?c");
    pq.expandPrefixes();
    ASSERT_EQ(1, pq._filters.size());
  }
  catch (const ad_semsearch::Exception& e) {
    FAIL() << e.getFullErrorMessage();
  }

};

TEST(ParserTest, testExpandPrefixes) {
  ParsedQuery pq = SparqlParser::parse(
      "PREFIX : <http://rdf.myprefix.com/>\n"
          "PREFIX ns: <http://rdf.myprefix.com/ns/>\n"
          "PREFIX xxx: <http://rdf.myprefix.com/xxx/>\n"
          "SELECT ?x ?z \n "
          "WHERE \t {?x :myrel ?y. ?y ns:myrel ?z.?y nsx:rel2 <http://abc.de>}");
  pq.expandPrefixes();
  ASSERT_EQ(3, pq._prefixes.size());
  ASSERT_EQ(2, pq._selectedVariables.size());
  ASSERT_EQ(3, pq._whereClauseTriples.size());
  ASSERT_EQ("", pq._prefixes[0]._prefix);
  ASSERT_EQ("<http://rdf.myprefix.com/>", pq._prefixes[0]._uri);
  ASSERT_EQ("ns", pq._prefixes[1]._prefix);
  ASSERT_EQ("<http://rdf.myprefix.com/ns/>", pq._prefixes[1]._uri);
  ASSERT_EQ("?x", pq._selectedVariables[0]);
  ASSERT_EQ("?z", pq._selectedVariables[1]);
  ASSERT_EQ("?x", pq._whereClauseTriples[0]._s);
  ASSERT_EQ("<http://rdf.myprefix.com/myrel>", pq._whereClauseTriples[0]._p);
  ASSERT_EQ("?y", pq._whereClauseTriples[0]._o);
  ASSERT_EQ("?y", pq._whereClauseTriples[1]._s);
  ASSERT_EQ("<http://rdf.myprefix.com/ns/myrel>", pq._whereClauseTriples[1]
      ._p);
  ASSERT_EQ("?z", pq._whereClauseTriples[1]._o);
  ASSERT_EQ("?y", pq._whereClauseTriples[2]._s);
  ASSERT_EQ("nsx:rel2", pq._whereClauseTriples[2]._p);
  ASSERT_EQ("<http://abc.de>", pq._whereClauseTriples[2]._o);
  ASSERT_EQ("", pq._limit);
  ASSERT_EQ("", pq._offset);
}

TEST(ParserTest, testSolutionModifiers) {
  ParsedQuery pq = SparqlParser::parse(
      "SELECT ?x WHERE \t {?x :myrel ?y}");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(1, pq._selectedVariables.size());
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("", pq._limit);
  ASSERT_EQ("", pq._offset);
  ASSERT_EQ(size_t(0), pq._orderBy.size());
  ASSERT_FALSE(pq._distinct);
  ASSERT_FALSE(pq._reduced);


  pq = SparqlParser::parse(
      "SELECT ?x WHERE \t {?x :myrel ?y} LIMIT 10");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(1, pq._selectedVariables.size());
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("10", pq._limit);
  ASSERT_EQ("", pq._offset);
  ASSERT_EQ(size_t(0), pq._orderBy.size());
  ASSERT_FALSE(pq._distinct);
  ASSERT_FALSE(pq._reduced);

  pq = SparqlParser::parse(
      "SELECT ?x WHERE \t {?x :myrel ?y}\n"
          "LIMIT 10 OFFSET 15");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(1, pq._selectedVariables.size());
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("10", pq._limit);
  ASSERT_EQ("15", pq._offset);
  ASSERT_EQ(size_t(0), pq._orderBy.size());
  ASSERT_FALSE(pq._distinct);
  ASSERT_FALSE(pq._reduced);

  pq = SparqlParser::parse(
      "SELECT DISTINCT ?x ?y WHERE \t {?x :myrel ?y}\n"
          "ORDER BY ?y LIMIT 10 OFFSET 15");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(2, pq._selectedVariables.size());
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("10", pq._limit);
  ASSERT_EQ("15", pq._offset);
  ASSERT_EQ(size_t(1), pq._orderBy.size());
  ASSERT_EQ("?y", pq._orderBy[0]._key);
  ASSERT_FALSE(pq._orderBy[0]._desc);
  ASSERT_TRUE(pq._distinct);
  ASSERT_FALSE(pq._reduced);

  pq = SparqlParser::parse(
      "SELECT DISTINCT ?x SCORE(?x|?c) ?y WHERE \t {?x :myrel ?y}\n"
          "ORDER BY ASC(?y) DESC(SCORE(?x|?c)) LIMIT 10 OFFSET 15");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(3, pq._selectedVariables.size());
  ASSERT_EQ("SCORE(?x|?c)", pq._selectedVariables[1]);
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("10", pq._limit);
  ASSERT_EQ("15", pq._offset);
  ASSERT_EQ(size_t(2), pq._orderBy.size());
  ASSERT_EQ("?y", pq._orderBy[0]._key);
  ASSERT_FALSE(pq._orderBy[0]._desc);
  ASSERT_EQ("SCORE(?x|?c)", pq._orderBy[1]._key);
  ASSERT_TRUE(pq._orderBy[1]._desc);
  ASSERT_TRUE(pq._distinct);
  ASSERT_FALSE(pq._reduced);

  pq = SparqlParser::parse(
      "SELECT REDUCED ?x ?y WHERE \t {?x :myrel ?y}\n"
          "ORDER BY DESC(?x) ASC(?y) LIMIT 10 OFFSET 15");
  pq.expandPrefixes();
  ASSERT_EQ(0, pq._prefixes.size());
  ASSERT_EQ(2, pq._selectedVariables.size());
  ASSERT_EQ(1, pq._whereClauseTriples.size());
  ASSERT_EQ("10", pq._limit);
  ASSERT_EQ("15", pq._offset);
  ASSERT_EQ(size_t(2), pq._orderBy.size());
  ASSERT_EQ("?x", pq._orderBy[0]._key);
  ASSERT_TRUE(pq._orderBy[0]._desc);
  ASSERT_EQ("?y", pq._orderBy[1]._key);
  ASSERT_FALSE(pq._orderBy[1]._desc);
  ASSERT_FALSE(pq._distinct);
  ASSERT_TRUE(pq._reduced);

  pq = SparqlParser::parse(
      "SELECT ?x ?y WHERE {?x is-a Actor} LIMIT 10");
  pq.expandPrefixes();
  ASSERT_EQ("10", pq._limit);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}