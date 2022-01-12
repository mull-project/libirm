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
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Module.h>

using namespace irm;

TEST(IntrinsicReplacement, canMutate) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto intrinsicType = llvm::Type::getInt8Ty(context);
  auto sadd = llvm::Intrinsic::getDeclaration(
      &module, llvm::Intrinsic::sadd_with_overflow, { intrinsicType });
  auto ssub = llvm::Intrinsic::getDeclaration(
      &module, llvm::Intrinsic::ssub_with_overflow, { intrinsicType });

  auto op1 = llvm::ConstantInt::get(intrinsicType, 5, false);
  auto op2 = llvm::ConstantInt::get(intrinsicType, 40, false);

  auto callSAdd = llvm::CallInst::Create(sadd, { op1, op2 }, "", basicBlock);
  auto callSSub = llvm::CallInst::Create(ssub, { op1, op2 }, "", basicBlock);

  sadd_with_overflowTossub_with_overflow mutator;
  ASSERT_TRUE(mutator.canMutate(callSAdd));
  ASSERT_FALSE(mutator.canMutate(callSSub));
}

TEST(IntrinsicReplacement, mutate) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function =
      llvm::Function::Create(functionType, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto intrinsicType = llvm::Type::getInt8Ty(context);
  auto sadd = llvm::Intrinsic::getDeclaration(
      &module, llvm::Intrinsic::sadd_with_overflow, { intrinsicType });

  auto op1 = llvm::ConstantInt::get(intrinsicType, 5, false);
  auto op2 = llvm::ConstantInt::get(intrinsicType, 40, false);

  auto callSad = llvm::CallInst::Create(sadd, { op1, op2 }, "", basicBlock);
  auto resultUse = llvm::ExtractValueInst::Create(callSad, { 1 }, "", basicBlock);
  sadd_with_overflowTossub_with_overflow mutator;
  mutator.mutate(callSad);

  auto ssub = llvm::dyn_cast<llvm::IntrinsicInst>(&basicBlock->front());

  ASSERT_EQ(resultUse->getAggregateOperand(), ssub);
  ASSERT_NE(ssub, nullptr);
  ASSERT_EQ(ssub->getIntrinsicID(), llvm::Intrinsic::ssub_with_overflow);
  ASSERT_EQ(ssub->getOperand(0), op1);
  ASSERT_EQ(ssub->getOperand(1), op2);
}
