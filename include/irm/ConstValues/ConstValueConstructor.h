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

namespace llvm {
class Value;
class Type;
} // namespace llvm

namespace irm {
class ConstValueConstructor__ {
public:
  virtual llvm::Value *constructValue(llvm::Type *type) = 0;
  virtual ~ConstValueConstructor__() = default;
};

class FloatValueConstructor : public ConstValueConstructor__ {
public:
  explicit FloatValueConstructor(float value);
  llvm::Value *constructValue(llvm::Type *type) override;

private:
  float constantValue;
};

class DoubleValueConstructor : public ConstValueConstructor__ {
public:
  explicit DoubleValueConstructor(double value);
  llvm::Value *constructValue(llvm::Type *type) override;

private:
  double constantValue;
};

class IntValueConstructor : public ConstValueConstructor__ {
public:
  explicit IntValueConstructor(int value);
  llvm::Value *constructValue(llvm::Type *type) override;

private:
  int constantValue;
};

} // namespace irm