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

#include "irm/Mutations/CmpInstPredicateReplacement.h"

using namespace irm;

CmpInstPredicateReplacement::CmpInstPredicateReplacement(llvm::Instruction::OtherOps cmpType,
                                                         llvm::CmpInst::Predicate from,
                                                         llvm::CmpInst::Predicate to)
    : cmpType(cmpType), from(from), to(to) {}

bool CmpInstPredicateReplacement::canMutate(llvm::Instruction *instruction) {
  assert(instruction);
  auto cmp = llvm::dyn_cast<llvm::CmpInst>(instruction);
  if (!cmp) {
    return false;
  }
  return cmp->getPredicate() == from;
}

void CmpInstPredicateReplacement::mutate(llvm::Instruction *instruction) {
  assert(canMutate(instruction));
  assert(instruction->getParent());
  assert(instruction->getNumOperands() == 2);

  auto lhs = instruction->getOperand(0);
  auto rhs = instruction->getOperand(1);
  auto replacement = llvm::CmpInst::Create(cmpType, to, lhs, rhs, "");
  replacement->insertAfter(instruction);
  instruction->replaceAllUsesWith(replacement);
  instruction->eraseFromParent();
}
