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

TEST(StoreValueReplacement, canMutate) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt16Ty(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto value = llvm::ConstantInt::get(intType, 5, false);
  auto address = new llvm::AllocaInst(intType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);
  auto load = new llvm::LoadInst(intType, address, "", basicBlock);

  StoreIntReplacement mutator(0);
  ASSERT_TRUE(mutator.canMutate(store));
  ASSERT_FALSE(mutator.canMutate(load));
}

TEST(StoreValueReplacement, mutate) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt16Ty(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto value = llvm::ConstantInt::get(intType, 5, false);
  auto address = new llvm::AllocaInst(intType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);

  auto originalValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_FALSE(originalValue->isZero());

  StoreIntReplacement mutator(0);
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_TRUE(mutatedValue->isZero());
}
