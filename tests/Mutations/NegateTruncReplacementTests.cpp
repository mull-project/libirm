//
//  Copyright 2020 Mull Project
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

TEST(NegateTruncReplacement, canMutate) {
  using Mutator = NegateTruncReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  llvm::Type *i1type = llvm::IntegerType::get(context, 1);
  llvm::Type *i32type = llvm::IntegerType::get(context, 32);

  auto i32Value = llvm::ConstantInt::get(i32type, 1, false);
  auto *allocaInst = new llvm::AllocaInst(i32type, 0, "", basicBlock);

  /// This instruction plays no role in this test. It is here only to create
  /// a realistic layout.
  new llvm::StoreInst(i32Value, allocaInst, basicBlock);

  auto *loadInst = new llvm::LoadInst(i32type, allocaInst, "", basicBlock);
  auto *truncInst = new llvm::TruncInst(loadInst, i1type, "", basicBlock);

  Mutator mutator;
  ASSERT_TRUE(mutator.canMutate(truncInst));
}

TEST(NegateTruncReplacement, mutate) {
  using Mutator = NegateTruncReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);

  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  llvm::Type *i1type = llvm::IntegerType::get(context, 1);
  llvm::Type *i32type = llvm::IntegerType::get(context, 32);

  auto i32Value = llvm::ConstantInt::get(i32type, 1, false);
  auto *allocaInst = new llvm::AllocaInst(i32type, 0, "", basicBlock);

  /// This instruction plays no role in this test. It is here only to create
  /// a realistic layout.
  new llvm::StoreInst(i32Value, allocaInst, basicBlock);

  auto *loadInst = new llvm::LoadInst(i32type, allocaInst, "", basicBlock);
  auto *truncInst = new llvm::TruncInst(loadInst, i1type, "", basicBlock);
  auto *retInst = llvm::ReturnInst::Create(context, truncInst, basicBlock);

  Mutator mutator;

  ASSERT_EQ(retInst->getReturnValue(), truncInst);

  /// Before mutation:
  /// %0 = alloca i32
  /// store i32 1, i32* %0
  /// %1 = load i32, i32* %0
  /// %2 = trunc i32 %1 to i1
  /// ret i1 %2
  ///
  /// After mutation:
  /// %0 = alloca i32
  /// store i32 1, i32* %0
  /// %1 = load i32, i32* %0
  /// %2 = trunc i32 %1 to i1
  /// %not = xor i1 %2, true
  /// ret i1 %not

  mutator.mutate(truncInst);

  ASSERT_EQ(truncInst->getOpcode(), llvm::Instruction::Trunc);

  auto notXorInstruction = truncInst->getNextNode();
  ASSERT_EQ(notXorInstruction->getOpcode(), llvm::Instruction::Xor);
  ASSERT_EQ(retInst->getReturnValue(), notXorInstruction);
}
