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

#pragma once

#include "irm/ConstValues/ConstValueConstructor.h"
#include "irm/IRMutation.h"
#include <llvm/IR/Type.h>
#include <memory>

namespace irm {

class StoreValueReplacement : public IRMutation {
public:
  StoreValueReplacement(ConstValueConstructor *constConstructor, llvm::Type::TypeID typeID);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  std::unique_ptr<ConstValueConstructor> constConstructor;
  llvm::Type::TypeID typeID;
};

class StoreIntReplacement : public StoreValueReplacement {
public:
  explicit StoreIntReplacement(int value)
      : StoreValueReplacement(new IntValueConstructor(value), llvm::Type::IntegerTyID) {}
};

class StoreFloatReplacement : public StoreValueReplacement {
public:
  explicit StoreFloatReplacement(float value)
      : StoreValueReplacement(new FloatingValueConstructor(value), llvm::Type::FloatTyID) {}
};

class StoreDoubleReplacement : public StoreValueReplacement {
public:
  explicit StoreDoubleReplacement(double value)
      : StoreValueReplacement(new FloatingValueConstructor(value), llvm::Type::DoubleTyID) {}
};

} // namespace irm
