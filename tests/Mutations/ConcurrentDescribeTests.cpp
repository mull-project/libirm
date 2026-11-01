//
//  Copyright 2026 Mull Project
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
#include <atomic>
#include <thread>
#include <vector>

using namespace irm;

/// Mull calls describeReplacement from its parallel mutant search, so it must not write to the
/// LLVMContext. The ConstantFP map only races while growing, hence many small contexts. A failure
/// is a crash or a hang of the whole test binary, not an assertion.
TEST(ConcurrentDescribe, storeDouble_doesNotRaceOnContext) {
  const unsigned threadsCount = 8;
  const unsigned storesPerThread = 64;
  const unsigned rounds = 500;

  for (unsigned round = 0; round < rounds; round++) {
    llvm::LLVMContext context;
    auto voidType = llvm::Type::getVoidTy(context);
    auto doubleType = llvm::Type::getDoubleTy(context);
    llvm::Module module("test", context);
    auto type = llvm::FunctionType::get(voidType, false);
    auto function = llvm::Function::Create(type, llvm::Function::InternalLinkage, "test", module);
    auto basicBlock = llvm::BasicBlock::Create(context, "entry", function);
    auto address = new llvm::AllocaInst(doubleType, 0, "", basicBlock);

    std::vector<std::vector<llvm::Instruction *>> stores(threadsCount);
    for (unsigned thread = 0; thread < threadsCount; thread++) {
      for (unsigned i = 0; i < storesPerThread; i++) {
        double value = 1.0 + thread + i * 0.25 / storesPerThread;
        auto constant = llvm::ConstantFP::get(doubleType, value);
        stores[thread].push_back(new llvm::StoreInst(constant, address, "", basicBlock));
      }
    }

    std::atomic<unsigned> ready{ 0 };
    std::vector<std::thread> threads;
    for (unsigned thread = 0; thread < threadsCount; thread++) {
      threads.emplace_back([&, thread]() {
        StoreDoubleReplacement mutator(42);
        ready++;
        while (ready.load() != threadsCount) {
        }
        for (auto store : stores[thread]) {
          mutator.describeReplacement(store);
        }
      });
    }
    for (auto &thread : threads) {
      thread.join();
    }
  }
}
