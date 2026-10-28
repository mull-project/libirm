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

#include <string>

namespace llvm {
class APInt;
class Value;
class Type;
} // namespace llvm

namespace irm {

class ConstValueConstructor {
public:
  /// original: the value being replaced; its type determines the constant's type.
  virtual llvm::Value *constructValue(llvm::Value *original) = 0;
  /// Human-readable form of the constant constructValue would produce for this original.
  virtual std::string describeValue(llvm::Value *original) = 0;
  virtual ~ConstValueConstructor() = default;
};

class FloatingValueConstructor : public ConstValueConstructor {
public:
  explicit FloatingValueConstructor(double value);
  llvm::Value *constructValue(llvm::Value *original) override;
  std::string describeValue(llvm::Value *original) override;

private:
  double constantValue;
};

class IntValueConstructor : public ConstValueConstructor {
public:
  explicit IntValueConstructor(int value);
  llvm::Value *constructValue(llvm::Value *original) override;
  std::string describeValue(llvm::Value *original) override;

private:
  int constantValue;
};

/// Derives the replacement from the original value instead of using a fixed constant:
/// a constant int gets its LSB flipped (always observable, even through `trunc to i1`),
/// a dynamic bool (`i1`, or the `zext i1 -> i8` pattern of a C/C++ bool store) becomes
/// `true`, and any other dynamic value falls back to the fixed constant.
class FlippedIntValueConstructor : public ConstValueConstructor {
public:
  explicit FlippedIntValueConstructor(int fallbackValue);
  llvm::Value *constructValue(llvm::Value *original) override;
  std::string describeValue(llvm::Value *original) override;

private:
  llvm::APInt replacementValue(llvm::Value *original);
  int fallbackValue;
};

/// Derives the replacement from the original value instead of using a fixed constant:
/// a constant zero becomes 1.0 (negating zero would yield the equivalent -0.0),
/// any other constant gets its sign flipped, and a dynamic value falls back to
/// the fixed constant.
class FlippedFloatingValueConstructor : public ConstValueConstructor {
public:
  explicit FlippedFloatingValueConstructor(double fallbackValue);
  llvm::Value *constructValue(llvm::Value *original) override;
  std::string describeValue(llvm::Value *original) override;

private:
  double fallbackValue;
};

} // namespace irm