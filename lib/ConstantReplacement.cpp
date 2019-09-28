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

#include "irm/Mutations/ConstantReplacement.h"
#include <llvm/IR/Instruction.h>

using namespace irm;

ConstantReplacement::ConstantReplacement(ConstValueConstructor *constConstructor,
                                         ConstValueConstructor *zeroConstConstructor,
                                         llvm::Value::ValueTy valueType, unsigned operandPosition)
    : constConstructor(constConstructor), zeroConstConstructor(zeroConstConstructor),
      valueType(valueType), operandPosition(operandPosition) {}

bool ConstantReplacement::canMutate(llvm::Instruction *instruction) {
  assert(instruction);

  if (instruction->getNumOperands() <= operandPosition) {
    return false;
  }
  /// TODO: shall we also mutate allocas? it is a normal instruction with a constant value...
  if (instruction->getOpcode() == llvm::Instruction::Alloca) {
    return false;
  }

  auto operand = instruction->getOperand(operandPosition);
  return operand->getValueID() == valueType;
}

void ConstantReplacement::mutate(llvm::Instruction *instruction) {
  assert(instruction);
  assert(canMutate(instruction));

  auto constant = llvm::cast<llvm::Constant>(instruction->getOperand(operandPosition));
  auto constructor = constant->isZeroValue() ? constConstructor.get() : zeroConstConstructor.get();
  auto replacement = constructor->constructValue(constant->getType());
  instruction->setOperand(operandPosition, replacement);
}
