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

template <llvm::Instruction::OtherOps cmpType, llvm::CmpInst::Predicate from,
          llvm::CmpInst::Predicate to>
class CmpInstPredicateReplacement : public IRMutation {
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
    auto replacement = llvm::CmpInst::Create(cmpType, to, lhs, rhs, "", basicBlock);
    instruction->replaceAllUsesWith(replacement);
    instruction->eraseFromParent();
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
using FCmpEQToNE_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OEQ, llvm::CmpInst::FCMP_ONE>;

// != / ==
using FCmpNEToEQ_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ONE, llvm::CmpInst::FCMP_OEQ>;

// >  / <=
using FCmpGTToLE_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGT, llvm::CmpInst::FCMP_OLE>;

// <= / >
using FCmpLEToGT_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLE, llvm::CmpInst::FCMP_OGT>;

// >= / <
using FCmpGEToLT_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGE, llvm::CmpInst::FCMP_OLT>;

// <  / >=
using FCmpLTToGE_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLT, llvm::CmpInst::FCMP_OGE>;

#pragma mark Floating unordered comparisons

// == / !=
using FCmpEQToNE_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UEQ, llvm::CmpInst::FCMP_UNE>;

// != / ==
using FCmpNEToEQ_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UNE, llvm::CmpInst::FCMP_UEQ>;

// >  / <=
using FCmpGTToLE_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGT, llvm::CmpInst::FCMP_ULE>;

// <= / >
using FCmpLEToGT_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULE, llvm::CmpInst::FCMP_UGT>;

// >= / <
using FCmpGEToLT_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGE, llvm::CmpInst::FCMP_ULT>;

// <  / >=
using FCmpLTToGE_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULT, llvm::CmpInst::FCMP_UGE>;

#pragma mark Floating boolean comparisons

using FCmpTrueToFalse =
    FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_TRUE, llvm::CmpInst::FCMP_FALSE>;
using FCmpFalseToTrue =
    FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_FALSE, llvm::CmpInst::FCMP_TRUE>;

#pragma mark Integer comparisons

// == / !=
using ICmpEQToNE = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_EQ, llvm::CmpInst::ICMP_NE>;

// != / ==
using ICmpNEToEQ = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_NE, llvm::CmpInst::ICMP_EQ>;

#pragma mark Unsigned integer comparisons

// >  / <=
using ICmpGTToLE_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGT, llvm::CmpInst::ICMP_ULE>;

// <= / >
using ICmpLEToGT_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULE, llvm::CmpInst::ICMP_UGT>;

// >= / <
using ICmpGEToLT_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGE, llvm::CmpInst::ICMP_ULT>;

// <  / >=
using ICmpLTToGE_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULT, llvm::CmpInst::ICMP_UGE>;

#pragma mark Signed integer comparisons

// >  / <=
using ICmpGTToLE_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGT, llvm::CmpInst::ICMP_SLE>;

// <= / >
using ICmpLEToGT_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLE, llvm::CmpInst::ICMP_SGT>;

// >= / <
using ICmpGEToLT_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGE, llvm::CmpInst::ICMP_SLT>;

// <  / >=
using ICmpLTToGE_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLT, llvm::CmpInst::ICMP_SGE>;

#pragma mark - Boundary change

///  >  | >=
using ICmpGTToGE_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGT, llvm::CmpInst::ICMP_SGE>;
using ICmpGTToGE_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGT, llvm::CmpInst::ICMP_UGE>;
using FCmpGTToGE_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGT, llvm::CmpInst::FCMP_UGE>;
using FCmpGTToGE_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGT, llvm::CmpInst::FCMP_OGE>;

///  >= | >
using ICmpGEToGT_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SGE, llvm::CmpInst::ICMP_SGT>;
using ICmpGEToGT_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_UGE, llvm::CmpInst::ICMP_UGT>;
using FCmpGEToGT_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_UGE, llvm::CmpInst::FCMP_UGT>;
using FCmpGEToGT_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OGE, llvm::CmpInst::FCMP_OGT>;

///  <  | <=
using ICmpLTToLE_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLT, llvm::CmpInst::ICMP_SLE>;
using ICmpLTToLE_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULT, llvm::CmpInst::ICMP_ULE>;
using FCmpLTToLE_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULT, llvm::CmpInst::FCMP_ULE>;
using FCmpLTToLE_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLT, llvm::CmpInst::FCMP_OLE>;

///  <= | <
using ICmpLEToLT_S = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_SLE, llvm::CmpInst::ICMP_SLT>;
using ICmpLEToLT_U = ICmpInstPredicateReplacement<llvm::CmpInst::ICMP_ULE, llvm::CmpInst::ICMP_ULT>;
using FCmpLEToLT_U = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_ULE, llvm::CmpInst::FCMP_ULT>;
using FCmpLEToLT_O = FCmpInstPredicateReplacement<llvm::CmpInst::FCMP_OLE, llvm::CmpInst::FCMP_OLT>;

} // namespace irm
