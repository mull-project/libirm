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

#include "irm/Mutations/CallReplacement.h"
#include <llvm/IR/Instructions.h>

using namespace irm;

CallReplacement::CallReplacement(ConstValueConstructor *constConstructor,
                                 llvm::Type::TypeID returnTypeId)
    : constConstructor(constConstructor), returnTypeId(returnTypeId) {}

bool CallReplacement::canMutate(llvm::Instruction *instruction) {
  assert(instruction);
  if (instruction->getOpcode() != llvm::Instruction::Call) {
    return false;
  }
  auto *call = llvm::dyn_cast<llvm::CallInst>(instruction);
  if (!call) {
    return false;
  }

  /// Do not touch indirect calls
  if (!call->getCalledFunction()) {
    return false;
  }
  /// Do not touch intrinsics
  if (call->getCalledFunction()->getIntrinsicID() != llvm::Intrinsic::ID(0)) {
    return false;
  }
  assert(call->getFunctionType());
  assert(call->getFunctionType()->getReturnType());
  auto returnType = call->getFunctionType()->getReturnType();
  if (returnType->getTypeID() != returnTypeId) {
    return false;
  }

  return true;
}

void CallReplacement::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  auto *call = llvm::dyn_cast<llvm::CallInst>(instruction);
  auto returnType = call->getFunctionType()->getReturnType();
  auto replacement = constConstructor->constructValue(returnType);
  instruction->replaceAllUsesWith(replacement);
  instruction->eraseFromParent();
}
