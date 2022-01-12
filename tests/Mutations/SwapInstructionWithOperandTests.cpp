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
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

using namespace irm;

TEST(SwapInstructionWithOperandTests, canMutate) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);
  auto add = llvm::BinaryOperator::CreateAdd(op1, op2, "add", basicBlock);

  SwapAddWithOperand_0 swapAdd;
  ASSERT_TRUE(swapAdd.canMutate(add));

  SwapInstructionWithOperand swapAddOOB(llvm::Instruction::BinaryOps::Add, 15);
  ASSERT_FALSE(swapAddOOB.canMutate(add));

  auto op3 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 40.2);
  auto fneg = llvm::UnaryOperator::CreateFNeg(op3, "fneg", basicBlock);
  ASSERT_FALSE(swapAdd.canMutate(fneg));
  ASSERT_FALSE(swapAddOOB.canMutate(fneg));

  SwapFNegWithOperand swapFneg;
  ASSERT_FALSE(swapFneg.canMutate(add));
  ASSERT_TRUE(swapFneg.canMutate(fneg));
}

TEST(SwapInstructionWithOperandTests, mutate) {
  llvm::LLVMContext context;
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto op1 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 5, false);
  auto op2 = llvm::ConstantInt::get(llvm::IntegerType::get(context, 8), 40, false);
  auto add = llvm::BinaryOperator::CreateAdd(op1, op2, "add", basicBlock);

  auto addUser = llvm::BinaryOperator::CreateAdd(add, op2, "add_user", basicBlock);

  SwapAddWithOperand_0 swapAdd;
  swapAdd.mutate(add);
  ASSERT_EQ(addUser->getOperand(0), op1);

  auto op3 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 40.2);
  auto fneg = llvm::UnaryOperator::CreateFNeg(op3, "fneg", basicBlock);
  auto fnegUser = llvm::BinaryOperator::CreateFAdd(fneg, op3, "fneg_user", basicBlock);
  SwapFNegWithOperand swapFneg;
  swapFneg.mutate(fneg);
  ASSERT_EQ(fnegUser->getOperand(0), op3);
}