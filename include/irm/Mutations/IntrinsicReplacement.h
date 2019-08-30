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
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Intrinsics.h>

namespace irm {

template <llvm::Intrinsic::ID from, llvm::Intrinsic::ID to>
class IntrinsicReplacement : public IRMutation {
public:
  bool canMutate(llvm::Instruction *instruction) override {
    if (instruction->getOpcode() != llvm::Instruction::Call) {
      return false;
    }
    auto intrinsic = llvm::dyn_cast<llvm::IntrinsicInst>(instruction);
    return intrinsic->getIntrinsicID() == from;
  }

  void mutate(llvm::Instruction *instruction) override {
    assert(instruction);
    assert(canMutate(instruction));
    auto intrinsic = llvm::cast<llvm::IntrinsicInst>(instruction);
    auto replacement = llvm::Intrinsic::getDeclaration(
        instruction->getModule(), to, { intrinsic->getFunctionType()->getParamType(0) });

    auto op1 = intrinsic->getOperand(0);
    auto op2 = intrinsic->getOperand(1);
    llvm::CallInst::Create(replacement, { op1, op2 }, "", instruction->getParent());
    instruction->eraseFromParent();
  }

private:
};

using SAddToSSub =
    IntrinsicReplacement<llvm::Intrinsic::sadd_with_overflow, llvm::Intrinsic::ssub_with_overflow>;
using SSubToSAdd =
    IntrinsicReplacement<llvm::Intrinsic::ssub_with_overflow, llvm::Intrinsic::sadd_with_overflow>;

using SAddSatToSSubSat = IntrinsicReplacement<llvm::Intrinsic::sadd_sat, llvm::Intrinsic::ssub_sat>;
using SSubSatToSAddSat = IntrinsicReplacement<llvm::Intrinsic::ssub_sat, llvm::Intrinsic::sadd_sat>;

} // namespace irm
