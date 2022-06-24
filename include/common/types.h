// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasmedge/common/types.h - Types definition ------------------------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the enumerations of Wasm VM used types and the type
/// recognition templates.
///
//===----------------------------------------------------------------------===//
#pragma once

#include "common/enum_types.h"
#include "common/errcode.h"
#include "common/int128.h"
#include "common/variant.h"

#include <cstdint>
#include <type_traits>
#include <variant>
#include <vector>
namespace WasmEdge {

namespace {

/// Remove const, reference, and volitile.
template <typename T>
using RemoveCVRefT = std::remove_cv_t<std::remove_reference_t<T>>;

} // namespace

// >>>>>>>> Type definitions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

using Byte = uint8_t;

namespace InterType {
using S8 = int8_t;
using U8 = uint8_t;
using S16 = int16_t;
using U16 = uint16_t;
using S32 = int32_t;
using U32 = uint32_t;
using S64 = int64_t;
using U64 = uint64_t;
using FLOAT32 = float;
using FLOAT64 = double;
using CHAR = char;
using STRING = char *;
using BOOL = bool;

// Implementation of "record" type in interface types
struct RecordField {
  const char *name;
  InterfaceType ty;
};

struct RECORD {
  RecordField *field = new RecordField();
};

// Implementation of "variant" type in interface types
struct VariantCase {
  const char *name;
  InterfaceType ty;
};

struct VARIANT {
  VariantCase *cases = new VariantCase();
};

// Implementation of "tuple" type in interface types
struct TUPLE {
  InterfaceType *types = new InterfaceType();
};

// Implementation of "flags" type in interface types
struct FLAGS {
  char **names = new char *;
};

// Implementation of "enum" type in interface types
struct ENUM {
  char **names = new char *;
};

// Implentation of "union" type in interface types
struct UNION {
  InterfaceType *types = new InterfaceType();
};

// Implementation of "expected" type in interface types
struct EXPECTED {
  InterfaceType ok;
  InterfaceType err;
};

// Implementation of "List" type in interface types
struct LIST {
  InterfaceType *InternalStroage = new InterfaceType();
};

/// UnknownInter definition
struct Unknown {
  uint64_t Value = 0;
  Unknown() = default;
};

} // namespace InterType

/// SIMD types definition.
using int64x2_t [[gnu::vector_size(16)]] = int64_t;
using uint64x2_t [[gnu::vector_size(16)]] = uint64_t;
using int32x4_t [[gnu::vector_size(16)]] = int32_t;
using uint32x4_t [[gnu::vector_size(16)]] = uint32_t;
using int16x8_t [[gnu::vector_size(16)]] = int16_t;
using uint16x8_t [[gnu::vector_size(16)]] = uint16_t;
using int8x16_t [[gnu::vector_size(16)]] = int8_t;
using uint8x16_t [[gnu::vector_size(16)]] = uint8_t;
using doublex2_t [[gnu::vector_size(16)]] = double;
using floatx4_t [[gnu::vector_size(16)]] = float;

/// UnknownRef definition.
struct UnknownRef {
  uint64_t Value = 0;
  UnknownRef() = default;
};

/// FuncRef definition.
namespace Runtime::Instance {
class FunctionInstance;
}
struct FuncRef {
#if __INTPTR_WIDTH__ == 32
  const uint32_t Padding = -1;
#endif
  const Runtime::Instance::FunctionInstance *Ptr = nullptr;
  FuncRef() = default;
  FuncRef(const Runtime::Instance::FunctionInstance *P) : Ptr(P) {}
};

/// ExternRef definition.
struct ExternRef {
#if __INTPTR_WIDTH__ == 32
  const uint32_t Padding = -1;
#endif
  void *Ptr = nullptr;
  ExternRef() = default;
  template <typename T> ExternRef(T *P) : Ptr(reinterpret_cast<void *>(P)) {}
};

/// NumType and RefType variant definitions.
using RefVariant = Variant<UnknownRef, FuncRef, ExternRef>;
using ValVariant =
    Variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t,
            int64_t, float, double, uint128_t, int128_t, uint64x2_t, int64x2_t,
            uint32x4_t, int32x4_t, uint16x8_t, int16x8_t, uint8x16_t, int8x16_t,
            floatx4_t, doublex2_t, UnknownRef, FuncRef, ExternRef>;
using InterfaceVariant =
    Variant<InterType::BOOL, InterType::S8, InterType::U8, InterType::S16,
            InterType::U16, InterType::S32, InterType::U32, InterType::S64,
            InterType::U64, InterType::CHAR, InterType::FLOAT32,
            InterType::FLOAT64, InterType::STRING, InterType::RECORD,
            InterType::VARIANT, InterType::TUPLE, InterType::ENUM,
            InterType::UNION, InterType::FLAGS, InterType::EXPECTED,
            InterType::Unknown, InterType::LIST>;

/// BlockType definition.
struct BlockType {
  bool IsValType;
  union {
    ValType Type;
    uint32_t Idx;
  } Data;
  BlockType() = default;
  BlockType(ValType VType) { setData(VType); }
  BlockType(uint32_t Idx) { setData(Idx); }
  void setData(ValType VType) {
    IsValType = true;
    Data.Type = VType;
  }
  void setData(uint32_t Idx) {
    IsValType = false;
    Data.Idx = Idx;
  }
};

/// NumType and RefType conversions.
inline constexpr ValType ToValType(const NumType Val) noexcept {
  return static_cast<ValType>(Val);
}
inline constexpr ValType ToValType(const RefType Val) noexcept {
  return static_cast<ValType>(Val);
}
inline constexpr ValType ToValType(const InterfaceType Val) noexcept {
  return static_cast<ValType>(Val);
}

// <<<<<<<< Type definitions <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>> Const expressions to checking value types >>>>>>>>>>>>>>>>>>>>>>>>>>

/// Return true if Wasm unsign (uint32_t and uint64_t).
template <typename T>
struct IsWasmUnsign
    : std::bool_constant<std::is_same_v<RemoveCVRefT<T>, uint32_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint64_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint128_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint64x2_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint32x4_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint16x8_t> ||
                         std::is_same_v<RemoveCVRefT<T>, uint8x16_t>> {};
template <typename T>
inline constexpr const bool IsWasmUnsignV = IsWasmUnsign<T>::value;

/// Return true if Wasm sign (int32_t and int64_t).
template <typename T>
struct IsWasmSign
    : std::bool_constant<std::is_same_v<RemoveCVRefT<T>, int32_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int64_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int128_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int64x2_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int32x4_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int16x8_t> ||
                         std::is_same_v<RemoveCVRefT<T>, int8x16_t>> {};
template <typename T>
inline constexpr const bool IsWasmSignV = IsWasmSign<T>::value;

/// Return true if Wasm float (float and double).
template <typename T>
struct IsWasmFloat
    : std::bool_constant<std::is_same_v<RemoveCVRefT<T>, float> ||
                         std::is_same_v<RemoveCVRefT<T>, double> ||
                         std::is_same_v<RemoveCVRefT<T>, floatx4_t> ||
                         std::is_same_v<RemoveCVRefT<T>, doublex2_t>> {};
template <typename T>
inline constexpr const bool IsWasmFloatV = IsWasmFloat<T>::value;

/// Return true if Wasm reference (funcref and externref).
template <typename T>
struct IsWasmRef
    : std::bool_constant<std::is_same_v<RemoveCVRefT<T>, FuncRef> ||
                         std::is_same_v<RemoveCVRefT<T>, ExternRef>> {};
template <typename T>
inline constexpr const bool IsWasmRefV = IsWasmRef<T>::value;

/// Return true if Wasm Interface type
template <typename T>
struct IsWasmInterface
    : std::bool_constant<std::is_same_v<RemoveCVRefT<T>, InterType::BOOL> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::S8> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::U8> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::S16> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::U16> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::S32> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::U32> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::S64> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::U64> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::FLOAT32> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::FLOAT64> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::CHAR> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::STRING> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::RECORD> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::VARIANT> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::TUPLE> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::FLAGS> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::ENUM> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::UNION> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::EXPECTED> ||
                         std::is_same_v<RemoveCVRefT<T>, InterType::LIST>> {};
template <typename T>
inline constexpr const bool IsWasmInterfaceV = IsWasmInterface<T>::value;

/// Return true if Wasm int (int32_t, uint32_t, int64_t, uint64_t).
template <typename T>
struct IsWasmInt : std::bool_constant<IsWasmSignV<T> || IsWasmUnsignV<T>> {};
template <typename T>
inline constexpr const bool IsWasmIntV = IsWasmInt<T>::value;

/// Return true if Wasm int or Wasm float.
template <typename T>
struct IsWasmNum : std::bool_constant<IsWasmIntV<T> || IsWasmFloatV<T>> {};
template <typename T>
inline constexpr const bool IsWasmNumV = IsWasmNum<T>::value;

/// Return true if Wasm native num types (uint32_t, uint64_t, float, double).
template <typename T>
struct IsWasmNativeNum
    : std::bool_constant<IsWasmUnsignV<T> || IsWasmFloatV<T>> {};
template <typename T>
inline constexpr const bool IsWasmNativeNumV = IsWasmNativeNum<T>::value;

/// Return true if Wasm value types (num types and reference types).
template <typename T>
struct IsWasmVal : std::bool_constant<IsWasmNumV<T> || IsWasmRefV<T>> {};
template <typename T>
inline constexpr const bool IsWasmValV = IsWasmVal<T>::value;

/// Return signed type.
template <typename T>
using MakeWasmSignedT =
    typename std::conditional<IsWasmFloatV<T>, std::common_type<T>,
                              std::make_signed<T>>::type::type;

/// Return unsigned type.
template <typename T>
using MakeWasmUnsignedT =
    typename std::conditional<IsWasmFloatV<T>, std::common_type<T>,
                              std::make_unsigned<T>>::type::type;

/// Cast-to-signed function.
template <typename T>
typename std::enable_if_t<IsWasmNumV<T>, MakeWasmSignedT<T>> toSigned(T Val) {
  return static_cast<MakeWasmSignedT<T>>(Val);
}

/// Cast-to-unsigned function.
template <typename T>
typename std::enable_if_t<IsWasmNumV<T>, MakeWasmUnsignedT<T>>
toUnsigned(T Val) {
  return static_cast<MakeWasmUnsignedT<T>>(Val);
}

// <<<<<<<< Const expressions to checking value types <<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>> Template to get value type from type >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

template <typename T> inline ValType ValTypeFromType() noexcept;

template <> inline ValType ValTypeFromType<uint32_t>() noexcept {
  return ValType::I32;
}
template <> inline ValType ValTypeFromType<int32_t>() noexcept {
  return ValType::I32;
}
template <> inline ValType ValTypeFromType<uint64_t>() noexcept {
  return ValType::I64;
}
template <> inline ValType ValTypeFromType<int64_t>() noexcept {
  return ValType::I64;
}
template <> inline ValType ValTypeFromType<uint128_t>() noexcept {
  return ValType::V128;
}
template <> inline ValType ValTypeFromType<int128_t>() noexcept {
  return ValType::V128;
}
template <> inline ValType ValTypeFromType<float>() noexcept {
  return ValType::F32;
}
template <> inline ValType ValTypeFromType<double>() noexcept {
  return ValType::F64;
}
template <> inline ValType ValTypeFromType<FuncRef>() noexcept {
  return ValType::FuncRef;
}
template <> inline ValType ValTypeFromType<ExternRef>() noexcept {
  return ValType::ExternRef;
}

// >>>>>>>> Template to get Interface type from type
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

template <typename T> inline InterfaceType InterfaceTypeFromType() noexcept;
template <> inline InterfaceType InterfaceTypeFromType<bool>() noexcept {
  return InterfaceType::BOOL;
}
template <> inline InterfaceType InterfaceTypeFromType<int8_t>() noexcept {
  return InterfaceType::S8;
}
template <> inline InterfaceType InterfaceTypeFromType<uint8_t>() noexcept {
  return InterfaceType::U8;
}
template <> inline InterfaceType InterfaceTypeFromType<int16_t>() noexcept {
  return InterfaceType::S16;
}
template <> inline InterfaceType InterfaceTypeFromType<uint16_t>() noexcept {
  return InterfaceType::U16;
}
template <> inline InterfaceType InterfaceTypeFromType<int32_t>() noexcept {
  return InterfaceType::S32;
}
template <> inline InterfaceType InterfaceTypeFromType<uint32_t>() noexcept {
  return InterfaceType::U32;
}
template <> inline InterfaceType InterfaceTypeFromType<int64_t>() noexcept {
  return InterfaceType::S64;
}
template <> inline InterfaceType InterfaceTypeFromType<uint64_t>() noexcept {
  return InterfaceType::U64;
}
template <> inline InterfaceType InterfaceTypeFromType<float>() noexcept {
  return InterfaceType::FLOAT32;
}
template <> inline InterfaceType InterfaceTypeFromType<double>() noexcept {
  return InterfaceType::FLOAT64;
}
template <> inline InterfaceType InterfaceTypeFromType<char>() noexcept {
  return InterfaceType::CHAR;
}
template <> inline InterfaceType InterfaceTypeFromType<char *>() noexcept {
  return InterfaceType::STRING;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::RECORD>() noexcept {
  return InterfaceType::RECORD;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::VARIANT>() noexcept {
  return InterfaceType::VARIANT;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::TUPLE>() noexcept {
  return InterfaceType::TUPLE;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::FLAGS>() noexcept {
  return InterfaceType::FLAGS;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::ENUM>() noexcept {
  return InterfaceType::ENUM;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::UNION>() noexcept {
  return InterfaceType::UNION;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::EXPECTED>() noexcept {
  return InterfaceType::EXPECTED;
}
template <>
inline InterfaceType InterfaceTypeFromType<InterType::LIST>() noexcept {
  return InterfaceType::LIST;
}
// <<<<<<<< Template to get value type from type <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// >>>>>>>> Const expression to generate value from value type >>>>>>>>>>>>>>>>>

inline constexpr ValVariant ValueFromType(ValType Type) noexcept {
  switch (Type) {
  case ValType::I32:
    return uint32_t(0U);
  case ValType::I64:
    return uint64_t(0U);
  case ValType::F32:
    return float(0.0F);
  case ValType::F64:
    return double(0.0);
  case ValType::V128:
    return uint128_t(0U);
  case ValType::FuncRef:
  case ValType::ExternRef:
    return UnknownRef();
  case ValType::None:
  default:
    assumingUnreachable();
  }
}

// >>>>>>>> Const expression to generate value from Interface type
// >>>>>>>>>>>>>>>>>

inline constexpr InterfaceVariant
InterfaceTypeFromType(InterfaceType Type) noexcept {
  switch (Type) {
  case InterfaceType::BOOL:
    return true;
  case InterfaceType::S8:
    return 0;
  case InterfaceType::U8:
    return uint8_t(0U);
  case InterfaceType::S16:
    return 0;
  case InterfaceType::U16:
    return uint16_t(0U);
  case InterfaceType::S32:
    return 0;
  case InterfaceType::U32:
    return uint32_t(0U);
  case InterfaceType::S64:
    return 0;
  case InterfaceType::U64:
    return uint64_t(0U);
  case InterfaceType::FLOAT32:
    return float(0.0F);
  case InterfaceType::FLOAT64:
    return double(0.0);
  case InterfaceType::CHAR:
    return char('c');
  case InterfaceType::STRING: {
    char *s = (char *)"string";
    return s;
  }
  case InterfaceType::RECORD:
  case InterfaceType::VARIANT:
  case InterfaceType::TUPLE:
  case InterfaceType::FLAGS:
  case InterfaceType::UNION:
  case InterfaceType::ENUM:
  case InterfaceType::EXPECTED:
  case InterfaceType::LIST:
    return InterType::Unknown();
  default:
    assumingUnreachable();
  }
}

// <<<<<<<< Const expression to generate value from value type <<<<<<<<<<<<<<<<<

// >>>>>>>> Functions to retrieve reference inners >>>>>>>>>>>>>>>>>>>>>>>>>>>>>

inline constexpr bool isNullRef(const ValVariant &Val) {
  return Val.get<UnknownRef>().Value == 0;
}
inline constexpr bool isNullRef(const RefVariant &Val) {
  return Val.get<UnknownRef>().Value == 0;
}

inline const Runtime::Instance::FunctionInstance *
retrieveFuncRef(const ValVariant &Val) {
  return reinterpret_cast<const Runtime::Instance::FunctionInstance *>(
      Val.get<FuncRef>().Ptr);
}
inline const Runtime::Instance::FunctionInstance *
retrieveFuncRef(const RefVariant &Val) {
  return reinterpret_cast<const Runtime::Instance::FunctionInstance *>(
      Val.get<FuncRef>().Ptr);
}
inline const Runtime::Instance::FunctionInstance *
retrieveFuncRef(const FuncRef &Val) {
  return reinterpret_cast<const Runtime::Instance::FunctionInstance *>(Val.Ptr);
}

template <typename T> inline T &retrieveExternRef(const ValVariant &Val) {
  return *reinterpret_cast<T *>(Val.get<ExternRef>().Ptr);
}
template <typename T> inline T &retrieveExternRef(const RefVariant &Val) {
  return *reinterpret_cast<T *>(Val.get<ExternRef>().Ptr);
}
template <typename T> inline T &retrieveExternRef(const ExternRef &Val) {
  return *reinterpret_cast<T *>(Val.Ptr);
}

// <<<<<<<< Functions to retrieve reference inners <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

} // namespace WasmEdge
