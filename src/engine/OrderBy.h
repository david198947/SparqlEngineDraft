// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)
#pragma once

#include <list>
#include <utility>
#include <vector>
#include <unordered_map>
#include "./Operation.h"
#include "./IndexScan.h"
#include "./QueryExecutionTree.h"

using std::list;
using std::unordered_map;
using std::pair;
using std::vector;

class OrderBy : public Operation {
  public:
    virtual size_t getResultWidth() const;

  public:

    OrderBy(QueryExecutionContext *qec, const QueryExecutionTree& subtree,
            const vector<pair<size_t, bool>>& sortIndices);

    OrderBy(const OrderBy& other);

    OrderBy& operator=(const OrderBy& other);

    virtual ~OrderBy();

    virtual string asString() const;

    virtual size_t resultSortedOn() const {
      return std::numeric_limits<size_t>::max();
    }

    virtual void setTextLimit(size_t limit) {
      _subtree->setTextLimit(limit);
    }

    virtual size_t getSizeEstimate() const {
      return _subtree->getSizeEstimate();
    }

    virtual size_t getCostEstimate() const {
      return size_t(getSizeEstimate() * logb(getSizeEstimate()))
             + _subtree->getCostEstimate();
    }


  private:
    QueryExecutionTree *_subtree;
    vector<pair<size_t, bool>> _sortIndices;

    virtual void computeResult(ResultTable *result) const;
};
