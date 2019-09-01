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
#include <llvm/IR/CallSite.h>

namespace irm {

template <typename ConstType, typename ConstValueType, typename ReplaceValueType>
struct Replacement {
  typedef ConstType Type;
  typedef ConstValueType ValueType;
  typedef ReplaceValueType ReplaceType;
};

template <typename ConstValueType, typename ReplaceValueType> struct ConstValueConstructor {
  ConstValueType getConstValue(ReplaceValueType value, llvm::Type *returnType) {
    return ConstValueType();
  }
};
template <typename ReplaceValueType> struct ConstValueConstructor<llvm::APInt, ReplaceValueType> {
  llvm::APInt getConstValue(ReplaceValueType value, llvm::Type *returnType) {
    return llvm::APInt(returnType->getIntegerBitWidth(), value);
  }
};

template <typename ReplaceValueType> struct ConstValueConstructor<llvm::APFloat, ReplaceValueType> {
  llvm::APFloat getConstValue(ReplaceValueType value, llvm::Type *) {
    return llvm::APFloat(value);
  }
};

template <typename ReplacementType, llvm::Type::TypeID returnTypeId, int value>
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

    ConstValueConstructor<typename ReplacementType::ValueType,
                          typename ReplacementType::ReplaceType>
        constValueConstructor;

    auto constValue = constValueConstructor.getConstValue(
        typename ReplacementType::ReplaceType(value), returnType);

    auto replacement = ReplacementType::Type::get(context, constValue);
    instruction->replaceAllUsesWith(replacement);
    instruction->eraseFromParent();
  }

private:
};

typedef Replacement<llvm::ConstantInt, llvm::APInt, int> IntegerReplacement;
typedef Replacement<llvm::ConstantFP, llvm::APFloat, float> FloatReplacement;
typedef Replacement<llvm::ConstantFP, llvm::APFloat, double> DoubleReplacement;

template <int value>
using IntCallReplacement = CallReplacement<IntegerReplacement, llvm::Type::IntegerTyID, value>;
template <int value>
using FloatCallReplacement = CallReplacement<FloatReplacement, llvm::Type::FloatTyID, value>;
template <int value>
using DoubleCallReplacement = CallReplacement<DoubleReplacement, llvm::Type::DoubleTyID, value>;

typedef IntCallReplacement<0> CallIntToZero;
typedef FloatCallReplacement<0> CallFloatToZero;
typedef DoubleCallReplacement<0> CallDoubleToZero;

typedef IntCallReplacement<42> CallIntTo42;
typedef FloatCallReplacement<42> CallFloatTo42;
typedef DoubleCallReplacement<42> CallDoubleTo42;

} // namespace irm
