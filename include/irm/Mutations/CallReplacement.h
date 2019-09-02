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
#include <llvm/IR/CallSite.h>

namespace irm {

template <typename ConstValueConstruct, llvm::Type::TypeID returnTypeId, int value>
class CallReplacement : public IRMutation {
public:
  bool canMutate(llvm::Instruction *instruction) override {
    assert(instruction);
    if (instruction->getOpcode() != llvm::Instruction::Call) {
      return false;
    }
    llvm::CallSite call(instruction);
    /// Do not touch intrinsics
    if (call.getIntrinsicID() != llvm::Intrinsic::ID(0)) {
      return false;
    }
    auto returnType = call.getFunctionType()->getReturnType();
    if (returnType->getTypeID() != returnTypeId) {
      return false;
    }

    return true;
  }
  void mutate(llvm::Instruction *instruction) override {
    assert(instruction);

    auto &context = instruction->getContext();
    llvm::CallSite call(instruction);
    auto returnType = call.getFunctionType()->getReturnType();

    using LLVMInstType = typename ConstValueConstruct::InstType;
    using LLVMConstType = typename ConstValueConstruct::ConstType;
    using ValueType = typename ConstValueConstruct::ValueType;

    ConstValueConstructor<LLVMConstType, ValueType> constructor;

    auto constValue = constructor.getConstValue(ValueType(value), returnType);

    auto replacement = LLVMInstType::get(context, constValue);
    instruction->replaceAllUsesWith(replacement);
    instruction->eraseFromParent();
  }

private:
};

template <int value>
using IntCallReplacement = CallReplacement<ConstIntegerConstruct, llvm::Type::IntegerTyID, value>;
template <int value>
using FloatCallReplacement = CallReplacement<ConstFloatConstruct, llvm::Type::FloatTyID, value>;
template <int value>
using DoubleCallReplacement = CallReplacement<ConstDoubleConstruct, llvm::Type::DoubleTyID, value>;

typedef IntCallReplacement<0> CallIntToZero;
typedef FloatCallReplacement<0> CallFloatToZero;
typedef DoubleCallReplacement<0> CallDoubleToZero;

typedef IntCallReplacement<42> CallIntTo42;
typedef FloatCallReplacement<42> CallFloatTo42;
typedef DoubleCallReplacement<42> CallDoubleTo42;

} // namespace irm
