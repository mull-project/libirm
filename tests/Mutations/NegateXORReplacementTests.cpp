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

TEST(NegateXORReplacement, canMutate) {
  using Mutator = NegateXORReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 1, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0, false);
  auto xorInstruction = llvm::BinaryOperator::CreateXor(op1, op2, "xor", basicBlock);

  Mutator mutator;
  ASSERT_TRUE(mutator.canMutate(xorInstruction));
}

TEST(NegateXORReplacement, mutate) {
  using Mutator = NegateXORReplacement;

  llvm::LLVMContext context;
  llvm::Module module("test", context);

  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 1, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 1), 0, false);
  auto xorInstruction = llvm::BinaryOperator::CreateXor(op1, op2, "xor", basicBlock);

  Mutator mutator;
  mutator.mutate(xorInstruction);

  /// We expect to have 2 xor instructions instead of one. See the implementation
  /// details of NegateXorReplacement.mutate().
  /// Before:
  /// entry:
  /// %xor = xor i1 true, false
  ///
  /// After:
  /// entry:
  /// %not = xor i1 false, true
  /// %xor = xor i1 true, %not
  ASSERT_EQ(xorInstruction->getOpcode(), llvm::Instruction::Xor);
  ASSERT_EQ(xorInstruction->getOperand(0), op1);
  ASSERT_EQ(xorInstruction->getOperand(1), xorInstruction->getPrevNode());
  ASSERT_EQ(xorInstruction->getPrevNode()->getOpcode(), llvm::Instruction::Xor);
  ASSERT_EQ(xorInstruction->getPrevNode()->getOperand(0), op2);
}
