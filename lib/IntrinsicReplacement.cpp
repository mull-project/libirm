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

#include "irm/Mutations/IntrinsicReplacement.h"
#include <llvm/IR/Instructions.h>
#include <vector>

using namespace irm;

IntrinsicReplacement::IntrinsicReplacement(llvm::Intrinsic::ID from, llvm::Intrinsic::ID to)
    : from(from), to(to) {}

bool IntrinsicReplacement::canMutate(llvm::Instruction *instruction) {
  if (instruction->getOpcode() != llvm::Instruction::Call) {
    return false;
  }
  auto intrinsic = llvm::dyn_cast<llvm::IntrinsicInst>(instruction);
  if (!intrinsic) {
    return false;
  }
  return intrinsic->getIntrinsicID() == from;
}

void IntrinsicReplacement::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  assert(canMutate(instruction));
  auto intrinsic = llvm::cast<llvm::IntrinsicInst>(instruction);
  auto replacement = llvm::Intrinsic::getDeclaration(
      instruction->getModule(), to, { intrinsic->getFunctionType()->getParamType(0) });

  std::vector<llvm::Value *> arguments;
  for (auto &arg : intrinsic->args()) {
    arguments.push_back(arg.get());
  }

  auto call = llvm::CallInst::Create(replacement, arguments, "");
  call->insertAfter(instruction);
  instruction->replaceAllUsesWith(call);
  instruction->eraseFromParent();
}
