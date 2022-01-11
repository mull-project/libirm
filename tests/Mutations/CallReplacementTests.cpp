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

TEST(CallReplacement, canMutate) {
  using Mutator = IntCallReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto intType = llvm::Type::getInt8Ty(context);
  auto intFunctionType = llvm::FunctionType::get(intType, false);
  auto intFunction =
      llvm::Function::Create(intFunctionType, llvm::Function::InternalLinkage, "call_int", module);

  auto floatType = llvm::Type::getFloatTy(context);
  auto floatFunctionType = llvm::FunctionType::get(floatType, false);
  auto floatFunction = llvm::Function::Create(
      floatFunctionType, llvm::Function::InternalLinkage, "call_float", module);

  auto callInt = llvm::CallInst::Create(intFunction, {}, "", basicBlock);
  auto callFloat = llvm::CallInst::Create(floatFunction, {}, "", basicBlock);

  Mutator mutator(0);
  ASSERT_TRUE(mutator.canMutate(callInt));
  ASSERT_FALSE(mutator.canMutate(callFloat));
}

TEST(CallReplacement, mutate) {
  using Mutator = IntCallReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto intType = llvm::Type::getInt32Ty(context);
  auto functionType = llvm::FunctionType::get(intType, false);
  auto function =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto intFunctionType = llvm::FunctionType::get(intType, false);
  auto intFunction =
      llvm::Function::Create(intFunctionType, llvm::Function::InternalLinkage, "call_int", module);

  auto callInt = llvm::CallInst::Create(intFunction, {}, "", basicBlock);
  auto retInst = llvm::ReturnInst::Create(context, callInt, basicBlock);

  Mutator mutator(15);
  mutator.mutate(callInt);

  auto constant = llvm::dyn_cast<llvm::ConstantInt>(retInst->getOperand(0));
  ASSERT_NE(constant, nullptr);
  ASSERT_EQ(constant->getType()->getTypeID(), llvm::Type::IntegerTyID);
  auto rawConstant = *(constant->getValue().getRawData());
  ASSERT_EQ(rawConstant, 15);
}
