//
//  Copyright 2019 Mull Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#pragma once

#include "irm/IRMutation.h"
#include <llvm/IR/InstrTypes.h>

namespace irm {

class _CmpInstPredicateReplacementBase : public IRMutation {
public:
  virtual llvm::CmpInst::Predicate _getFrom() = 0;
  virtual llvm::CmpInst::Predicate _getTo() = 0;
};

class CmpInstPredicateReplacement : public _CmpInstPredicateReplacementBase {
public:
  CmpInstPredicateReplacement(llvm::Instruction::OtherOps cmpType, llvm::CmpInst::Predicate from,
                              llvm::CmpInst::Predicate to);

  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

  llvm::CmpInst::Predicate _getFrom() override {
    return from;
  }
  llvm::CmpInst::Predicate _getTo() override {
    return to;
  }

private:
  llvm::Instruction::OtherOps cmpType;
  llvm::CmpInst::Predicate from;
  llvm::CmpInst::Predicate to;
};

#define FCMP_PREDICATE_REPLACEMENT(FROM, TO)                                                       \
  class FROM##To##TO : public CmpInstPredicateReplacement {                                        \
  public:                                                                                          \
    FROM##To##TO()                                                                                 \
        : CmpInstPredicateReplacement(llvm::Instruction::FCmp, llvm::CmpInst::FROM,                \
                                      llvm::CmpInst::TO) {}                                        \
  };

#define ICMP_PREDICATE_REPLACEMENT(FROM, TO)                                                       \
  class FROM##To##TO : public CmpInstPredicateReplacement {                                        \
  public:                                                                                          \
    FROM##To##TO()                                                                                 \
        : CmpInstPredicateReplacement(llvm::Instruction::ICmp, llvm::CmpInst::FROM,                \
                                      llvm::CmpInst::TO) {}                                        \
  };

/// Negation

// == / !=
FCMP_PREDICATE_REPLACEMENT(FCMP_OEQ, FCMP_ONE)
FCMP_PREDICATE_REPLACEMENT(FCMP_UEQ, FCMP_UNE)
ICMP_PREDICATE_REPLACEMENT(ICMP_EQ, ICMP_NE)

// != / ==
FCMP_PREDICATE_REPLACEMENT(FCMP_ONE, FCMP_OEQ)
FCMP_PREDICATE_REPLACEMENT(FCMP_UNE, FCMP_UEQ)
ICMP_PREDICATE_REPLACEMENT(ICMP_NE, ICMP_EQ)

// >  / <=
FCMP_PREDICATE_REPLACEMENT(FCMP_OGT, FCMP_OLE)
FCMP_PREDICATE_REPLACEMENT(FCMP_UGT, FCMP_ULE)
ICMP_PREDICATE_REPLACEMENT(ICMP_SGT, ICMP_SLE)
ICMP_PREDICATE_REPLACEMENT(ICMP_UGT, ICMP_ULE)

// <= / >
FCMP_PREDICATE_REPLACEMENT(FCMP_OLE, FCMP_OGT)
FCMP_PREDICATE_REPLACEMENT(FCMP_ULE, FCMP_UGT)
ICMP_PREDICATE_REPLACEMENT(ICMP_SLE, ICMP_SGT)
ICMP_PREDICATE_REPLACEMENT(ICMP_ULE, ICMP_UGT)

// >= / <
FCMP_PREDICATE_REPLACEMENT(FCMP_OGE, FCMP_OLT)
FCMP_PREDICATE_REPLACEMENT(FCMP_UGE, FCMP_ULT)
ICMP_PREDICATE_REPLACEMENT(ICMP_SGE, ICMP_SLT)
ICMP_PREDICATE_REPLACEMENT(ICMP_UGE, ICMP_ULT)

// <  / >=
FCMP_PREDICATE_REPLACEMENT(FCMP_OLT, FCMP_OGE)
FCMP_PREDICATE_REPLACEMENT(FCMP_ULT, FCMP_UGE)
ICMP_PREDICATE_REPLACEMENT(ICMP_SLT, ICMP_SGE)
ICMP_PREDICATE_REPLACEMENT(ICMP_ULT, ICMP_UGE)

// true <> false
FCMP_PREDICATE_REPLACEMENT(FCMP_TRUE, FCMP_FALSE)
FCMP_PREDICATE_REPLACEMENT(FCMP_FALSE, FCMP_TRUE)

/// Boundary change

///  >  | >=
FCMP_PREDICATE_REPLACEMENT(FCMP_UGT, FCMP_UGE)
FCMP_PREDICATE_REPLACEMENT(FCMP_OGT, FCMP_OGE)
ICMP_PREDICATE_REPLACEMENT(ICMP_UGT, ICMP_UGE)
ICMP_PREDICATE_REPLACEMENT(ICMP_SGT, ICMP_SGE)

///  >= | >
FCMP_PREDICATE_REPLACEMENT(FCMP_UGE, FCMP_UGT)
FCMP_PREDICATE_REPLACEMENT(FCMP_OGE, FCMP_OGT)
ICMP_PREDICATE_REPLACEMENT(ICMP_UGE, ICMP_UGT)
ICMP_PREDICATE_REPLACEMENT(ICMP_SGE, ICMP_SGT)

///  <  | <=
FCMP_PREDICATE_REPLACEMENT(FCMP_ULT, FCMP_ULE)
FCMP_PREDICATE_REPLACEMENT(FCMP_OLT, FCMP_OLE)
ICMP_PREDICATE_REPLACEMENT(ICMP_ULT, ICMP_ULE)
ICMP_PREDICATE_REPLACEMENT(ICMP_SLT, ICMP_SLE)

///  <= | <
FCMP_PREDICATE_REPLACEMENT(FCMP_ULE, FCMP_ULT)
FCMP_PREDICATE_REPLACEMENT(FCMP_OLE, FCMP_OLT)
ICMP_PREDICATE_REPLACEMENT(ICMP_ULE, ICMP_ULT)
ICMP_PREDICATE_REPLACEMENT(ICMP_SLE, ICMP_SLT)

} // namespace irm
