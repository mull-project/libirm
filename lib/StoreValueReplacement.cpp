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

#include "irm/Mutations/StoreValueReplacement.h"
#include <llvm/IR/Instructions.h>

using namespace irm;

StoreValueReplacement::StoreValueReplacement(ConstValueConstructor *constConstructor,
                                             llvm::Type::TypeID typeID)
    : constConstructor(constConstructor), typeID(typeID) {}

bool StoreValueReplacement::canMutate(llvm::Instruction *instruction) {
  assert(instruction);
  auto store = llvm::dyn_cast<llvm::StoreInst>(instruction);
  if (!store) {
    return false;
  }
  assert(store->getValueOperand());
  assert(store->getValueOperand()->getType());
  return store->getValueOperand()->getType()->getTypeID() == typeID;
}

void StoreValueReplacement::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  assert(canMutate(instruction));

  auto store = llvm::dyn_cast<llvm::StoreInst>(instruction);
  auto replacement = constConstructor->constructValue(store->getValueOperand()->getType());
  instruction->setOperand(0, replacement);
}
