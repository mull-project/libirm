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

TEST(BinaryOperatorReplacement, canMutate) {
  using Mutator = AddToSub;

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

TEST(BinaryOperatorReplacement, mutate) {
  using Mutator = AddToSub;

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
  ASSERT_EQ(sub.getOpcode(), llvm::Instruction::Sub);
  ASSERT_EQ(sub.getOperand(0), op1);
  ASSERT_EQ(sub.getOperand(1), op2);
}
TEST(BinaryOperatorReplacement, mutateShlToAShr) {
  using Mutator = ShlToAShr;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 1, false);
  auto shl = llvm::BinaryOperator::CreateShl(op1, op2, "shl", basicBlock);

  Mutator mutator;
  ASSERT_TRUE(mutator.canMutate(shl));
  mutator.mutate(shl);

  auto &ashr = basicBlock->front();
  ASSERT_EQ(ashr.getOpcode(), llvm::Instruction::AShr);
  ASSERT_EQ(ashr.getOperand(0), op1);
  ASSERT_EQ(ashr.getOperand(1), op2);
}

TEST(BinaryOperatorReplacement, signedness) {
  ASSERT_EQ(ShlToAShr().signedness(), Signedness::Signed);
  ASSERT_EQ(ShlToLShr().signedness(), Signedness::Unsigned);
  ASSERT_EQ(MulToSDiv().signedness(), Signedness::Signed);
  ASSERT_EQ(MulToUDiv().signedness(), Signedness::Unsigned);
  /// The original instruction already has the signedness
  ASSERT_EQ(SRemToSDiv().signedness(), Signedness::Any);
  ASSERT_EQ(URemToUDiv().signedness(), Signedness::Any);
  ASSERT_EQ(AShrToShl().signedness(), Signedness::Any);
  ASSERT_EQ(AddToSub().signedness(), Signedness::Any);
}
