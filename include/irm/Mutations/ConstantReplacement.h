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
#include <llvm/IR/Value.h>
#include <memory>

namespace irm {

class ConstantReplacement : public IRMutation {
public:
  ConstantReplacement(ConstValueConstructor *constConstructor,
                      ConstValueConstructor *zeroConstConstructor, llvm::Value::ValueTy valueType,
                      unsigned operandPosition);
  bool canMutate(llvm::Instruction *instruction) override;
  void mutate(llvm::Instruction *instruction) override;

private:
  std::unique_ptr<ConstValueConstructor> constConstructor;
  std::unique_ptr<ConstValueConstructor> zeroConstConstructor;
  llvm::Value::ValueTy valueType;
  unsigned operandPosition;
};

class ConstIntReplacement : public ConstantReplacement {
public:
  ConstIntReplacement(int value, unsigned position)
      : ConstantReplacement(new IntValueConstructor(value), new IntValueConstructor(0),
                            llvm::Value::ConstantIntVal, position) {}
};

class ConstFloatReplacement : public ConstantReplacement {
public:
  ConstFloatReplacement(float value, unsigned position)
      : ConstantReplacement(new FloatingValueConstructor(value), new FloatingValueConstructor(0),
                            llvm::Value::ConstantFPVal, position) {}
};

} // namespace irm
