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

#include <gtest/gtest.h>
#include <irm/irm.h>
#include <llvm/IR/Module.h>

using namespace irm;

TEST(ConstantReplacement, canMutate) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 40, false);
  auto add = llvm::BinaryOperator::CreateAdd(op1, op2, "add", basicBlock);

  auto fop1 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 5);
  auto fop2 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 40);
  auto fadd = llvm::BinaryOperator::CreateFAdd(fop1, fop2, "add", basicBlock);

  ConstIntReplacement mutator(42, 0);
  ASSERT_TRUE(mutator.canMutate(add));
  ASSERT_FALSE(mutator.canMutate(fadd));
}

TEST(ConstantReplacement, canMutate_severalOperands) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);

  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt8Ty(context);
  auto floatType = llvm::Type::getFloatTy(context);

  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto functionType = llvm::FunctionType::get(voidType, { intType, floatType }, false);
  auto calledFunction =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "", module);

  auto op1 = llvm::ConstantInt::get(intType, 5, false);
  auto op2 = llvm::ConstantFP::get(floatType, 40);
  auto call = llvm::CallInst::Create(calledFunction, { op1, op2 }, "", basicBlock);

  ConstIntReplacement mutator_0(42, 0);
  ConstIntReplacement mutator_1(42, 1);
  ConstIntReplacement mutator_2(42, 2);

  ASSERT_TRUE(mutator_0.canMutate(call));
  ASSERT_FALSE(mutator_1.canMutate(call));
  ASSERT_FALSE(mutator_2.canMutate(call));
}

TEST(ConstantReplacement, mutate_int) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);

  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt8Ty(context);
  auto floatType = llvm::Type::getFloatTy(context);

  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto functionType = llvm::FunctionType::get(voidType, { intType, floatType }, false);
  auto calledFunction =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "", module);

  auto op1NonZero = llvm::ConstantInt::get(intType, 5, false);
  auto op2NonZero = llvm::ConstantFP::get(floatType, 40);
  auto callNonZeros =
      llvm::CallInst::Create(calledFunction, { op1NonZero, op2NonZero }, "", basicBlock);

  auto op1Zero = llvm::ConstantInt::get(intType, 0, false);
  auto op2Zero = llvm::ConstantFP::get(floatType, 0);
  auto callZeros = llvm::CallInst::Create(calledFunction, { op1Zero, op2Zero }, "", basicBlock);

  ConstIntReplacement mutator_0(42, 0);

  mutator_0.mutate(callNonZeros);
  mutator_0.mutate(callZeros);

  auto nonZeroConstant = llvm::cast<llvm::ConstantInt>(callNonZeros->getOperand(0));
  ASSERT_TRUE(nonZeroConstant->isZero());
  auto zeroConstant = llvm::cast<llvm::ConstantInt>(callZeros->getOperand(0));
  ASSERT_FALSE(zeroConstant->isZero());
}

TEST(ConstantReplacement, mutate_float) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);

  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt8Ty(context);
  auto floatType = llvm::Type::getFloatTy(context);

  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto functionType = llvm::FunctionType::get(voidType, { intType, floatType }, false);
  auto calledFunction =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "", module);

  auto op1NonZero = llvm::ConstantInt::get(intType, 5, false);
  auto op2NonZero = llvm::ConstantFP::get(floatType, 40);
  auto callNonZeros =
      llvm::CallInst::Create(calledFunction, { op1NonZero, op2NonZero }, "", basicBlock);

  auto op1Zero = llvm::ConstantInt::get(intType, 0, false);
  auto op2Zero = llvm::ConstantFP::get(floatType, 0);
  auto callZeros = llvm::CallInst::Create(calledFunction, { op1Zero, op2Zero }, "", basicBlock);

  ConstFloatReplacement mutator_1(42, 1);

  mutator_1.mutate(callNonZeros);
  mutator_1.mutate(callZeros);

  auto nonZeroConstant = llvm::cast<llvm::ConstantFP>(callNonZeros->getOperand(1));
  ASSERT_TRUE(nonZeroConstant->isZero());
  auto zeroConstant = llvm::cast<llvm::ConstantFP>(callZeros->getOperand(1));
  ASSERT_FALSE(zeroConstant->isZero());
}
