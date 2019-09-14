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
#include <llvm/IR/Constants.h>

using namespace irm;

FloatValueConstructor::FloatValueConstructor(float value) : constantValue(value) {}

llvm::Value *FloatValueConstructor::constructValue(llvm::Type *type) {
  llvm::APFloat floatValue(constantValue);
  return llvm::ConstantFP::get(type, floatValue);
}

DoubleValueConstructor::DoubleValueConstructor(double value) : constantValue(value) {}

llvm::Value *DoubleValueConstructor::constructValue(llvm::Type *type) {
  llvm::APFloat floatValue(constantValue);
  return llvm::ConstantFP::get(type, floatValue);
}

IntValueConstructor::IntValueConstructor(int value) : constantValue(value) {}

llvm::Value *IntValueConstructor::constructValue(llvm::Type *type) {
  llvm::APInt intValue(type->getIntegerBitWidth(), constantValue);
  return llvm::ConstantInt::get(type, intValue);
}
