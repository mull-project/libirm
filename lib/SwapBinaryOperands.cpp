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

#include "irm/Mutations/SwapBinaryOperands.h"

using namespace irm;

SwapBinaryOperands::SwapBinaryOperands(llvm::Instruction::BinaryOps op) : op(op) {}

bool SwapBinaryOperands::canMutate(llvm::Instruction *instruction) {
  return instruction->getOpcode() == op;
}

void SwapBinaryOperands::mutate(llvm::Instruction *instruction) {
  assert(canMutate(instruction));
  assert(instruction->getParent());
  assert(instruction->getNumOperands() == 2);

  auto lhs = instruction->getOperand(0);
  auto rhs = instruction->getOperand(1);
  instruction->setOperand(0, rhs);
  instruction->setOperand(1, lhs);
}
