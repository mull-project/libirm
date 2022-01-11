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

TEST(CallRemoval, canMutate) {
  using MutatorNoIntrinsics = RemoveVoidFunctionCall;
  using MutatorOnlyIntrinsics = RemoveVoidIntrinsicsCall;

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

  auto voidType = llvm::Type::getVoidTy(context);
  auto voidFunctionType = llvm::FunctionType::get(voidType, false);
  auto voidFunction = llvm::Function::Create(
      voidFunctionType, llvm::Function::InternalLinkage, "call_void", module);

  auto intrinsicFunction = llvm::Intrinsic::getDeclaration(&module, llvm::Intrinsic::vastart);

  auto callInt = llvm::CallInst::Create(intFunction, {}, "", basicBlock);
  auto callVoid = llvm::CallInst::Create(voidFunction, {}, "", basicBlock);
  auto null = llvm::ConstantPointerNull::get(intType->getPointerTo(0));
  auto callIntrinsic = llvm::CallInst::Create(intrinsicFunction, { null }, "", basicBlock);

  MutatorNoIntrinsics mutatorNoIntrinsics;
  ASSERT_TRUE(mutatorNoIntrinsics.canMutate(callVoid));
  ASSERT_FALSE(mutatorNoIntrinsics.canMutate(callIntrinsic));
  ASSERT_FALSE(mutatorNoIntrinsics.canMutate(callInt));

  MutatorOnlyIntrinsics mutatorOnlyIntrinsics;
  ASSERT_FALSE(mutatorOnlyIntrinsics.canMutate(callVoid));
  ASSERT_TRUE(mutatorOnlyIntrinsics.canMutate(callIntrinsic));
  ASSERT_FALSE(mutatorOnlyIntrinsics.canMutate(callInt));
}

TEST(CallRemoval, mutate) {
  using Mutator = RemoveVoidFunctionCall;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "test", module);

  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto voidType = llvm::Type::getVoidTy(context);
  auto voidFunctionType = llvm::FunctionType::get(voidType, false);
  auto voidFunction = llvm::Function::Create(
      voidFunctionType, llvm::Function::InternalLinkage, "call_void", module);

  auto callVoid = llvm::CallInst::Create(voidFunction, {}, "", basicBlock);

  Mutator mutator;
  mutator.mutate(callVoid);
  ASSERT_EQ(basicBlock->getInstList().size(), size_t(0));
}
