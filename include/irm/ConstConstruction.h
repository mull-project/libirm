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

namespace irm {

template <typename LLVMInstType, typename LLVMConstType, typename ConstValueType>
struct ConstValueConstruction {
  typedef LLVMInstType InstType;
  typedef LLVMConstType ConstType;
  typedef ConstValueType ValueType;
};

template <typename LLVMConstType, typename ConstValueType> struct ConstValueConstructor {
  LLVMConstType getConstValue(ConstValueType value, llvm::Type *returnType) {
    return LLVMConstType();
  }
};

template <typename ConstValueType> struct ConstValueConstructor<llvm::APInt, ConstValueType> {
  llvm::APInt getConstValue(ConstValueType value, llvm::Type *type) {
    return llvm::APInt(type->getIntegerBitWidth(), value);
  }
};

template <typename ConstValueType> struct ConstValueConstructor<llvm::APFloat, ConstValueType> {
  llvm::APFloat getConstValue(ConstValueType value, llvm::Type *) {
    return llvm::APFloat(value);
  }
};

typedef ConstValueConstruction<llvm::ConstantInt, llvm::APInt, int> ConstIntegerConstruct;
typedef ConstValueConstruction<llvm::ConstantFP, llvm::APFloat, float> ConstFloatConstruct;
typedef ConstValueConstruction<llvm::ConstantFP, llvm::APFloat, double> ConstDoubleConstruct;

} // namespace irm
