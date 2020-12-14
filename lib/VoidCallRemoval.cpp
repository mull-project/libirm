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

#include "irm/Mutations/VoidCallRemoval.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>

using namespace irm;

VoidCallRemoval::VoidCallRemoval(bool onlyIntrinsics) : onlyIntrinsics(onlyIntrinsics) {}

bool VoidCallRemoval::canMutate(llvm::Instruction *instruction) {
  assert(instruction);
  if (instruction->getOpcode() != llvm::Instruction::Call) {
    return false;
  }
  llvm::CallInst *call = llvm::dyn_cast<llvm::CallInst>(instruction);
  /// Do not touch indirect calls
  if (!call->getCalledFunction()) {
    return false;
  }
  assert(call->getFunctionType()->getReturnType());
  if (call->getFunctionType()->getReturnType()->getTypeID() != llvm::Type::VoidTyID) {
    return false;
  }

  assert(call->getCalledFunction());
  bool isIntrinsic = call->getCalledFunction()->getIntrinsicID() != llvm::Intrinsic::not_intrinsic;
  if (onlyIntrinsics) {
    return isIntrinsic;
  }
  return !isIntrinsic;
}

void VoidCallRemoval::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  assert(canMutate(instruction));
  instruction->eraseFromParent();
}
