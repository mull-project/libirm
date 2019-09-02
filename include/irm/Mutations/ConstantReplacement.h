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

#include "irm/ConstConstruction.h"
#include "irm/IRMutation.h"
#include <llvm/IR/Type.h>

namespace irm {

template <typename ConstValueConstruct, int value, int operandPosition>
class ConstantReplacement : public IRMutation {
public:
  bool canMutate(llvm::Instruction *instruction) override {
    assert(instruction);

    if (instruction->getNumOperands() <= operandPosition) {
      return false;
    }
    /// TODO: shall we also mutate allocas? it is a normal instruction with a constant value...
    if (instruction->getOpcode() == llvm::Instruction::Alloca) {
      return false;
    }

    auto operand = instruction->getOperand(operandPosition);
    return llvm::isa<typename ConstValueConstruct::InstType>(operand);
  }
  void mutate(llvm::Instruction *instruction) override {
    assert(instruction);
    assert(canMutate(instruction));

    using LLVMInstType = typename ConstValueConstruct::InstType;
    using LLVMConstType = typename ConstValueConstruct::ConstType;
    using ValueType = typename ConstValueConstruct::ValueType;

    ConstValueConstructor<LLVMConstType, ValueType> constructor;

    auto constant = llvm::cast<LLVMInstType>(instruction->getOperand(operandPosition));
    uint64_t replacementValue = constant->isZero() ? value : 0;
    auto constValue = constructor.getConstValue(ValueType(replacementValue), constant->getType());

    auto replacement = LLVMInstType::get(constant->getType(), constValue);
    instruction->setOperand(operandPosition, replacement);
  }

private:
};

typedef ConstantReplacement<ConstIntegerConstruct, 42, 0> ConstIntReplacement_42_0;
typedef ConstantReplacement<ConstIntegerConstruct, 42, 1> ConstIntReplacement_42_1;
typedef ConstantReplacement<ConstIntegerConstruct, 42, 2> ConstIntReplacement_42_2;
typedef ConstantReplacement<ConstIntegerConstruct, 42, 3> ConstIntReplacement_42_3;
typedef ConstantReplacement<ConstIntegerConstruct, 42, 4> ConstIntReplacement_42_4;

typedef ConstantReplacement<ConstFloatConstruct, 42, 0> ConstFloatReplacement_42_0;
typedef ConstantReplacement<ConstFloatConstruct, 42, 1> ConstFloatReplacement_42_1;
typedef ConstantReplacement<ConstFloatConstruct, 42, 2> ConstFloatReplacement_42_2;
typedef ConstantReplacement<ConstFloatConstruct, 42, 3> ConstFloatReplacement_42_3;
typedef ConstantReplacement<ConstFloatConstruct, 42, 4> ConstFloatReplacement_42_4;

} // namespace irm
