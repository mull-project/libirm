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

#include "irm/ConstValues/ConstValueConstructor.h"
#include <llvm/ADT/SmallString.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

using namespace irm;

/// Clang stores a C/C++ bool as `store i8 (zext i1 %v to i8)`; loads truncate back to i1.
static bool isBoolZExt(llvm::Value *value) {
  auto zext = llvm::dyn_cast<llvm::ZExtInst>(value);
  return zext && zext->getSrcTy()->isIntegerTy(1) && zext->getDestTy()->isIntegerTy(8);
}

static llvm::APInt makeAPInt(unsigned bitWidth, uint64_t value) {
#if LLVM_VERSION_MAJOR >= 20
  return llvm::APInt(bitWidth, value, false, /*implicitTrunc=*/true);
#else
  return llvm::APInt(bitWidth, value);
#endif
}

static std::string describeAPInt(const llvm::APInt &value, bool isBool) {
  if (isBool) {
    return value.isZero() ? "false" : "true";
  }
  llvm::SmallString<32> str;
  value.toString(str, 10, /*Signed=*/true);
  return std::string(str);
}

/// Prints enough digits to round-trip the value in its own type (max_digits10), so the
/// description can be pasted back into the source (e.g. by a patch) without changing the value.
static std::string describeFloat(llvm::Value *value) {
  const llvm::APFloat &apFloat = llvm::cast<llvm::ConstantFP>(value)->getValueAPF();
  unsigned bits = llvm::APFloat::semanticsPrecision(apFloat.getSemantics());
  unsigned digits = 2 + bits * 30103 / 100000;
  llvm::SmallString<32> str;
  apFloat.toString(str, digits);
  return std::string(str);
}

FloatingValueConstructor::FloatingValueConstructor(double value) : constantValue(value) {}

llvm::Value *FloatingValueConstructor::constructValue(llvm::Value *original) {
  return llvm::ConstantFP::get(original->getType(), constantValue);
}

std::string FloatingValueConstructor::describeValue(llvm::Value *original) {
  return describeFloat(constructValue(original));
}

IntValueConstructor::IntValueConstructor(int value) : constantValue(value) {}

llvm::Value *IntValueConstructor::constructValue(llvm::Value *original) {
  auto type = original->getType();
  auto bitWidth = type->getIntegerBitWidth();
  uint64_t value = constantValue;
  /// Keep "non-zero" non-zero for i1: a plain truncation would turn any even value into false.
  if (bitWidth == 1) {
    value = constantValue == 0 ? 0 : 1;
  }
  return llvm::ConstantInt::get(type, makeAPInt(bitWidth, value));
}

std::string IntValueConstructor::describeValue(llvm::Value *original) {
  if (original->getType()->isIntegerTy(1)) {
    return constantValue == 0 ? "false" : "true";
  }
  return std::to_string(constantValue);
}

FlippedIntValueConstructor::FlippedIntValueConstructor(int fallbackValue)
    : fallbackValue(fallbackValue) {}

llvm::APInt FlippedIntValueConstructor::replacementValue(llvm::Value *original) {
  if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(original)) {
    llvm::APInt flipped = constant->getValue();
    flipped.flipBit(0);
    return flipped;
  }
  auto bitWidth = original->getType()->getIntegerBitWidth();
  if (bitWidth == 1 || isBoolZExt(original)) {
    return makeAPInt(bitWidth, 1);
  }
  return makeAPInt(bitWidth, fallbackValue);
}

llvm::Value *FlippedIntValueConstructor::constructValue(llvm::Value *original) {
  return llvm::ConstantInt::get(original->getType(), replacementValue(original));
}

std::string FlippedIntValueConstructor::describeValue(llvm::Value *original) {
  bool isBool = original->getType()->isIntegerTy(1) || isBoolZExt(original);
  return describeAPInt(replacementValue(original), isBool);
}

FlippedFloatingValueConstructor::FlippedFloatingValueConstructor(double fallbackValue)
    : fallbackValue(fallbackValue) {}

llvm::Value *FlippedFloatingValueConstructor::constructValue(llvm::Value *original) {
  if (auto constant = llvm::dyn_cast<llvm::ConstantFP>(original)) {
    if (constant->isZero()) {
      return llvm::ConstantFP::get(original->getType(), 1.0);
    }
    llvm::APFloat negated = constant->getValueAPF();
    negated.changeSign();
    return llvm::ConstantFP::get(original->getType()->getContext(), negated);
  }
  return llvm::ConstantFP::get(original->getType(), fallbackValue);
}

std::string FlippedFloatingValueConstructor::describeValue(llvm::Value *original) {
  return describeFloat(constructValue(original));
}
