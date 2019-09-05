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

namespace irm {

template <typename ConstValueConstruct, llvm::Type::TypeID typeID, int value>
class StoreValueReplacement : public IRMutation {
public:
  bool canMutate(llvm::Instruction *instruction) override {
    assert(instruction);
    auto store = llvm::dyn_cast<llvm::StoreInst>(instruction);
    if (!store) {
      return false;
    }
    return store->getOperand(0)->getType()->getTypeID() == typeID;
  }
  void mutate(llvm::Instruction *instruction) override {
    assert(instruction);
    assert(canMutate(instruction));

    using LLVMInstType = typename ConstValueConstruct::InstType;
    using LLVMConstType = typename ConstValueConstruct::ConstType;
    using ValueType = typename ConstValueConstruct::ValueType;

    ConstValueConstructor<LLVMConstType, ValueType> constructor;

    auto constType = instruction->getOperand(0)->getType();
    auto constValue = constructor.getConstValue(ValueType(value), constType);
    auto replacement = LLVMInstType::get(constType, constValue);

    instruction->setOperand(0, replacement);
  }

private:
};

typedef StoreValueReplacement<ConstIntegerConstruct, llvm::Type::IntegerTyID, 42>
    StoreIntReplacement_42;
typedef StoreValueReplacement<ConstFloatConstruct, llvm::Type::FloatTyID, 42>
    StoreFloatReplacement_42;
typedef StoreValueReplacement<ConstDoubleConstruct, llvm::Type::DoubleTyID, 42>
    StoreDoubleReplacement_42;

} // namespace irm
