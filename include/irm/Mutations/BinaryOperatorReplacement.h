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
#include <llvm/IR/Instructions.h>

namespace irm {

// TODO:
//  - handle NUW
//  - handle NSW
//  - handle fast math flags
template <llvm::Instruction::BinaryOps from, llvm::Instruction::BinaryOps to>
class BinaryOperatorReplacement : public IRMutation {
public:
  bool canMutate(llvm::Instruction *instruction) override {
    return instruction->getOpcode() == from;
  }
  void mutate(llvm::Instruction *instruction) override {
    assert(canMutate(instruction));
    assert(instruction->getParent());
    assert(instruction->getNumOperands() == 2);

    auto basicBlock = instruction->getParent();
    auto lhs = instruction->getOperand(0);
    auto rhs = instruction->getOperand(1);
    auto replacement = llvm::BinaryOperator::Create(to, lhs, rhs, "sub", basicBlock);
    instruction->replaceAllUsesWith(replacement);
    instruction->eraseFromParent();
  }

private:
};

using AddToSub = BinaryOperatorReplacement<llvm::Instruction::Add, llvm::Instruction::Sub>;
using FAddToFSub = BinaryOperatorReplacement<llvm::Instruction::FAdd, llvm::Instruction::FSub>;

using SubToAdd = BinaryOperatorReplacement<llvm::Instruction::Sub, llvm::Instruction::Add>;
using FSubToFAdd = BinaryOperatorReplacement<llvm::Instruction::FSub, llvm::Instruction::FAdd>;

using MulToSDiv = BinaryOperatorReplacement<llvm::Instruction::Mul, llvm::Instruction::SDiv>;
using MulToUDiv = BinaryOperatorReplacement<llvm::Instruction::Mul, llvm::Instruction::UDiv>;
using FMulToFDiv = BinaryOperatorReplacement<llvm::Instruction::FMul, llvm::Instruction::FDiv>;

using UDivToURem = BinaryOperatorReplacement<llvm::Instruction::UDiv, llvm::Instruction::URem>;
using SDivToSRem = BinaryOperatorReplacement<llvm::Instruction::SDiv, llvm::Instruction::SRem>;
using FDivToFRem = BinaryOperatorReplacement<llvm::Instruction::FDiv, llvm::Instruction::FRem>;

using UDivToMul = BinaryOperatorReplacement<llvm::Instruction::UDiv, llvm::Instruction::Mul>;
using SDivToMul = BinaryOperatorReplacement<llvm::Instruction::SDiv, llvm::Instruction::Mul>;
using FDivToFMul = BinaryOperatorReplacement<llvm::Instruction::FDiv, llvm::Instruction::FMul>;

using URemToUDiv = BinaryOperatorReplacement<llvm::Instruction::URem, llvm::Instruction::UDiv>;
using SRemToSDiv = BinaryOperatorReplacement<llvm::Instruction::SRem, llvm::Instruction::SDiv>;
using FRemToFDiv = BinaryOperatorReplacement<llvm::Instruction::FRem, llvm::Instruction::FDiv>;

} // namespace irm
