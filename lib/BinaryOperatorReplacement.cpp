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

#include "irm/Mutations/BinaryOperatorReplacement.h"

using namespace irm;

BinaryOperatorReplacement::BinaryOperatorReplacement(llvm::Instruction::BinaryOps from,
                                                     llvm::Instruction::BinaryOps to)
    : from(from), to(to) {}

bool BinaryOperatorReplacement::canMutate(llvm::Instruction *instruction) {
  return instruction->getOpcode() == from;
}

// TODO:
//  - handle NUW
//  - handle NSW
//  - handle fast math flags
void BinaryOperatorReplacement::mutate(llvm::Instruction *instruction) {
  assert(canMutate(instruction));
  assert(instruction->getParent());
  assert(instruction->getNumOperands() == 2);

  auto lhs = instruction->getOperand(0);
  auto rhs = instruction->getOperand(1);
  auto replacement = llvm::BinaryOperator::Create(to, lhs, rhs, "");
  replacement->setDebugLoc(instruction->getDebugLoc());
  replacement->insertAfter(instruction);
  instruction->replaceAllUsesWith(replacement);
  instruction->eraseFromParent();
}

static Signedness opcodeSignedness(llvm::Instruction::BinaryOps opcode) {
  switch (opcode) {
  case llvm::Instruction::SDiv:
  case llvm::Instruction::SRem:
  case llvm::Instruction::AShr:
    return Signedness::Signed;
  case llvm::Instruction::UDiv:
  case llvm::Instruction::URem:
  case llvm::Instruction::LShr:
    return Signedness::Unsigned;
  default:
    return Signedness::Any;
  }
}

/// A signed or unsigned `from` already fixes the signedness
Signedness BinaryOperatorReplacement::signedness() const {
  if (opcodeSignedness(from) != Signedness::Any) {
    return Signedness::Any;
  }
  return opcodeSignedness(to);
}
