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

  StoreIntReplacement mutator(42);
  ASSERT_TRUE(mutator.canMutate(store));
  ASSERT_FALSE(mutator.canMutate(load));
}

TEST(StoreValueReplacement, mutate_constantInt_flipsLSB) {
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

  StoreIntReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "4");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_EQ(mutatedValue->getZExtValue(), 4ULL);
}

TEST(StoreValueReplacement, mutate_constantBool_flipsValue) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto boolStorageType = llvm::Type::getInt8Ty(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  /// `bool b = false` at -O0: store i8 0
  auto value = llvm::ConstantInt::get(boolStorageType, 0, false);
  auto address = new llvm::AllocaInst(boolStorageType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);

  StoreIntReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "1");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  /// Observable through `trunc i8 ... to i1`, unlike the old constant 42
  ASSERT_EQ(mutatedValue->getZExtValue(), 1ULL);
}

TEST(StoreValueReplacement, mutate_dynamicBool_storesTrue) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto boolType = llvm::Type::getInt1Ty(context);
  auto boolStorageType = llvm::Type::getInt8Ty(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, { boolType }, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  /// `bool b = x` at -O0: store i8 (zext i1 %x to i8)
  auto value = new llvm::ZExtInst(function->getArg(0), boolStorageType, "", basicBlock);
  auto address = new llvm::AllocaInst(boolStorageType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);

  StoreIntReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "true");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_EQ(mutatedValue->getZExtValue(), 1ULL);
}

TEST(StoreValueReplacement, mutate_dynamicInt_fallsBackToConstant) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto intType = llvm::Type::getInt16Ty(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, { intType }, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto address = new llvm::AllocaInst(intType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(function->getArg(0), address, "", basicBlock);

  StoreIntReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "42");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantInt>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_EQ(mutatedValue->getZExtValue(), 42ULL);
}

TEST(StoreValueReplacement, mutate_constantFloat_negates) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto floatType = llvm::Type::getFloatTy(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto value = llvm::ConstantFP::get(floatType, 2.5);
  auto address = new llvm::AllocaInst(floatType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);

  StoreFloatReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "-2.5");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantFP>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_TRUE(mutatedValue->isExactlyValue(-2.5));
}

TEST(StoreValueReplacement, mutate_constantFloatZero_becomesOne) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto floatType = llvm::Type::getFloatTy(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  /// Negating 0.0 would produce -0.0: an equivalent mutant
  auto value = llvm::ConstantFP::get(floatType, 0.0);
  auto address = new llvm::AllocaInst(floatType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(value, address, "", basicBlock);

  StoreFloatReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "1");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantFP>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_TRUE(mutatedValue->isExactlyValue(1.0));
}

TEST(StoreValueReplacement, mutate_dynamicDouble_fallsBackToConstant) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto doubleType = llvm::Type::getDoubleTy(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, { doubleType }, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto address = new llvm::AllocaInst(doubleType, 0, "", basicBlock);
  auto store = new llvm::StoreInst(function->getArg(0), address, "", basicBlock);

  StoreDoubleReplacement mutator(42);
  ASSERT_EQ(mutator.describeReplacement(store), "42");
  mutator.mutate(store);

  auto mutatedValue = llvm::dyn_cast<llvm::ConstantFP>(store->getOperand(0));
  ASSERT_NE(mutatedValue, nullptr);
  ASSERT_TRUE(mutatedValue->isExactlyValue(42.0));
}

TEST(StoreValueReplacement, describe_roundTripsFloatingValues) {
  llvm::LLVMContext context;
  auto voidType = llvm::Type::getVoidTy(context);
  auto doubleType = llvm::Type::getDoubleTy(context);
  auto floatType = llvm::Type::getFloatTy(context);
  llvm::Module module("test", context);
  auto type = llvm::FunctionType::get(voidType, false);
  auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
  auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);

  auto doubleAddress = new llvm::AllocaInst(doubleType, 0, "", basicBlock);
  auto doubleStore = new llvm::StoreInst(
      llvm::ConstantFP::get(doubleType, 3.14159265358979), doubleAddress, "", basicBlock);
  StoreDoubleReplacement doubleMutator(42);
  ASSERT_EQ(std::stod(doubleMutator.describeReplacement(doubleStore)), -3.14159265358979);

  auto floatAddress = new llvm::AllocaInst(floatType, 0, "", basicBlock);
  auto floatStore = new llvm::StoreInst(
      llvm::ConstantFP::get(floatType, 3.14159274f), floatAddress, "", basicBlock);
  StoreFloatReplacement floatMutator(42);
  ASSERT_EQ(std::stof(floatMutator.describeReplacement(floatStore)), -3.14159274f);
}
