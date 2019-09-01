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

template <llvm::Instruction::OtherOps cmpType, llvm::CmpInst::Predicate from,
          llvm::CmpInst::Predicate to>
class CmpInstPredicateReplacement : public _CmpInstPredicateReplacementBase {
  static_assert(from != to, "Noop?");
public:
  bool canMutate(llvm::Instruction *instruction) override {
    assert(instruction);
    auto cmp = llvm::dyn_cast<llvm::CmpInst>(instruction);
    if (!cmp) {
      return false;
    }
    return cmp->getPredicate() == from;
  }
  void mutate(llvm::Instruction *instruction) override {
    assert(canMutate(instruction));
    assert(instruction->getParent());
    assert(instruction->getNumOperands() == 2);

    auto basicBlock = instruction->getParent();
    auto lhs = instruction->getOperand(0);
    auto rhs = instruction->getOperand(1);
    auto replacement = llvm::CmpInst::Create(cmpType, to, lhs, rhs, "");
    replacement->insertAfter(instruction);
    instruction->replaceAllUsesWith(replacement);
    instruction->eraseFromParent();
  }

  llvm::CmpInst::Predicate _getFrom() override {
    return from;
  }
  llvm::CmpInst::Predicate _getTo() override {
    return to;
  }
private:
};

template <llvm::CmpInst::Predicate from, llvm::CmpInst::Predicate to>
using FCmpInstPredicateReplacement = CmpInstPredicateReplacement<llvm::Instruction::FCmp, from, to>;

template <llvm::CmpInst::Predicate from, llvm::CmpInst::Predicate to>
using ICmpInstPredicateReplacement = CmpInstPredicateReplacement<llvm::Instruction::ICmp, from, to>;

#pragma mark - Negation

#pragma mark Floating ordered comparisons

// == / !=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OEQ, llvm::CmpInst::FCMP_ONE> FCmpEQToNE_O;

// != / ==
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ONE, llvm::CmpInst::FCMP_OEQ> FCmpNEToEQ_O;

// >  / <=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGT, llvm::CmpInst::FCMP_OLE> FCmpGTToLE_O;

// <= / >
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLE, llvm::CmpInst::FCMP_OGT> FCmpLEToGT_O;

// >= / <
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGE, llvm::CmpInst::FCMP_OLT> FCmpGEToLT_O;

// <  / >=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLT, llvm::CmpInst::FCMP_OGE> FCmpLTToGE_O;

#pragma mark Floating unordered comparisons

// == / !=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UEQ, llvm::CmpInst::FCMP_UNE> FCmpEQToNE_U;

// != / ==
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UNE, llvm::CmpInst::FCMP_UEQ> FCmpNEToEQ_U;

// >  / <=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGT, llvm::CmpInst::FCMP_ULE> FCmpGTToLE_U;

// <= / >
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULE, llvm::CmpInst::FCMP_UGT> FCmpLEToGT_U;

// >= / <
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGE, llvm::CmpInst::FCMP_ULT> FCmpGEToLT_U;

// <  / >=
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULT, llvm::CmpInst::FCMP_UGE> FCmpLTToGE_U;

#pragma mark Floating boolean comparisons

typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_TRUE, llvm::CmpInst::FCMP_FALSE>
    FCmpTrueToFalse;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_FALSE, llvm::CmpInst::FCMP_TRUE>
    FCmpFalseToTrue;

#pragma mark Integer comparisons

// == / !=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_EQ, llvm::CmpInst::ICMP_NE> ICmpEQToNE;

// != / ==
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_NE, llvm::CmpInst::ICMP_EQ> ICmpNEToEQ;

#pragma mark Unsigned integer comparisons

// >  / <=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGT, llvm::CmpInst::ICMP_ULE> ICmpGTToLE_U;

// <= / >
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULE, llvm::CmpInst::ICMP_UGT> ICmpLEToGT_U;

// >= / <
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGE, llvm::CmpInst::ICMP_ULT> ICmpGEToLT_U;

// <  / >=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULT, llvm::CmpInst::ICMP_UGE> ICmpLTToGE_U;

#pragma mark Signed integer comparisons

// >  / <=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGT, llvm::CmpInst::ICMP_SLE> ICmpGTToLE_S;

// <= / >
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLE, llvm::CmpInst::ICMP_SGT> ICmpLEToGT_S;

// >= / <
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGE, llvm::CmpInst::ICMP_SLT> ICmpGEToLT_S;

// <  / >=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLT, llvm::CmpInst::ICMP_SGE> ICmpLTToGE_S;

#pragma mark - Boundary change

///  >  | >=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGT, llvm::CmpInst::ICMP_SGE> ICmpGTToGE_S;
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGT, llvm::CmpInst::ICMP_UGE> ICmpGTToGE_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGT, llvm::CmpInst::FCMP_UGE> FCmpGTToGE_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGT, llvm::CmpInst::FCMP_OGE> FCmpGTToGE_O;

///  >= | >
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGE, llvm::CmpInst::ICMP_SGT> ICmpGEToGT_S;
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGE, llvm::CmpInst::ICMP_UGT> ICmpGEToGT_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGE, llvm::CmpInst::FCMP_UGT> FCmpGEToGT_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGE, llvm::CmpInst::FCMP_OGT> FCmpGEToGT_O;

///  <  | <=
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLT, llvm::CmpInst::ICMP_SLE> ICmpLTToLE_S;
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULT, llvm::CmpInst::ICMP_ULE> ICmpLTToLE_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULT, llvm::CmpInst::FCMP_ULE> FCmpLTToLE_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLT, llvm::CmpInst::FCMP_OLE> FCmpLTToLE_O;

///  <= | <
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLE, llvm::CmpInst::ICMP_SLT> ICmpLEToLT_S;
typedef ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULE, llvm::CmpInst::ICMP_ULT> ICmpLEToLT_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULE, llvm::CmpInst::FCMP_ULT> FCmpLEToLT_U;
typedef FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLE, llvm::CmpInst::FCMP_OLT> FCmpLEToLT_O;

} // namespace irm
