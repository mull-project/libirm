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
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

using namespace irm;

TEST(CmpInstPredicateReplacement, canMutate) {
  using Mutator = ICMP_EQToICMP_NE;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);

  auto eq = llvm::CmpInst::Create(
      llvm::Instruction::ICmp, llvm::CmpInst::ICMP_EQ, op1, op2, "add", basicBlock);
  auto ne = llvm::CmpInst::Create(
      llvm::Instruction::ICmp, llvm::CmpInst::ICMP_NE, op1, op2, "add", basicBlock);

  Mutator mutator;
  ASSERT_TRUE(mutator.canMutate(eq));
  ASSERT_FALSE(mutator.canMutate(ne));
}

TEST(CmpInstPredicateReplacement, mutate) {
  using Mutator = ICMP_EQToICMP_NE;

  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);

  auto eq = llvm::CmpInst::Create(
      llvm::Instruction::ICmp, llvm::CmpInst::ICMP_EQ, op1, op2, "add", basicBlock);

  Mutator mutator;
  mutator.mutate(eq);
  auto ne = llvm::dyn_cast<llvm::CmpInst>(&basicBlock->front());
  ASSERT_EQ(ne->getOpcode(), llvm::Instruction::ICmp);
  ASSERT_EQ(ne->getPredicate(), llvm::CmpInst::ICMP_NE);
  ASSERT_EQ(ne->getOperand(0), op1);
  ASSERT_EQ(ne->getOperand(1), op2);
}
