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

TEST(SwapBinaryOperands, canMutate) {
  using Mutator = SwapAddOperands;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);
  auto add = llvm::BinaryOperator::CreateAdd(op1, op2, "add", basicBlock);
  auto sub = llvm::BinaryOperator::CreateSub(op1, op2, "sub", basicBlock);

  Mutator mutator;
  ASSERT_TRUE(mutator.canMutate(add));
  ASSERT_FALSE(mutator.canMutate(sub));
}

TEST(SwapBinaryOperands, mutate) {
  using Mutator = SwapAddOperands;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);
  auto add = llvm::BinaryOperator::CreateAdd(op1, op2, "add", basicBlock);

  Mutator mutator;
  mutator.mutate(add);

  auto &sub = basicBlock->front();
  ASSERT_EQ(sub.getOpcode(), llvm::Instruction::Add);
  ASSERT_EQ(sub.getOperand(0), op2);
  ASSERT_EQ(sub.getOperand(1), op1);
}