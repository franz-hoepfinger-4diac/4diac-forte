/*******************************************************************************
 * Copyright (c) 2026 HR Agrartechnik GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Franz Höpfinger
 *      - initial API and implementation and/or initial documentation
 *******************************************************************************/

#pragma once

#include "forte/iec61131_functions/func_ADD.h"
#include "forte/iec61131_functions/func_SUB.h"
#include "forte/iec61131_functions/func_MUL.h"
#include "forte/iec61131_functions/func_DIV.h"

#include <cmath>
#include <limits>
#include <type_traits>

// Saturating (clamping) arithmetic used by SAFE_ADD/SAFE_SUB/SAFE_MUL/SAFE_DIV.
//
// Every SAFE_* function block is generic (ANY_MAGNITUDE / ANY_NUM, like the
// standard F_ADD/F_SUB/F_MUL/F_DIV), but reports a LIMIT_HIT flag and clamps
// the result into the valid range of the result type instead of silently
// wrapping around (see Uebung_011b3: UDINT#1 - UDINT#12 wraps to
// UDINT#4294967285 with plain F_SUB).
//
// Only plain numeric result types (integral or floating point TValueType) get
// the saturating treatment here; TIME/DATE/TOD/DT-pair results (e.g. via
// AddOperation<CIEC_TIME_OF_DAY, CIEC_TIME>) fall back to the unmodified
// func_ADD/func_SUB behavior, since those are out of scope for this first
// version of the library.
namespace forte::SafeArithmetic::arithmetic::detail {

  // GCC/Clang get the compiler-builtin overflow checks; every other compiler
  // (MSVC in particular, which has no __builtin_{add,sub,mul}_overflow) gets
  // a portable, UB-free fallback built from plain comparisons against
  // std::numeric_limits, at the cost of a few extra branches/divisions.
#if defined(__GNUC__) || defined(__clang__)
  template<typename V>
  bool add_overflow(V paIN1, V paIN2, V &paResult) {
    return __builtin_add_overflow(paIN1, paIN2, &paResult);
  }

  template<typename V>
  bool sub_overflow(V paIN1, V paIN2, V &paResult) {
    return __builtin_sub_overflow(paIN1, paIN2, &paResult);
  }

  template<typename V>
  bool mul_overflow(V paIN1, V paIN2, V &paResult) {
    return __builtin_mul_overflow(paIN1, paIN2, &paResult);
  }
#else
  template<typename V>
  bool add_overflow(V paIN1, V paIN2, V &paResult) {
    static_assert(std::is_integral_v<V>);
    if constexpr (std::is_signed_v<V>) {
      if ((paIN2 > V(0) && paIN1 > std::numeric_limits<V>::max() - paIN2) ||
          (paIN2 < V(0) && paIN1 < std::numeric_limits<V>::min() - paIN2)) {
        return true;
      }
    } else {
      if (paIN1 > std::numeric_limits<V>::max() - paIN2) {
        return true;
      }
    }
    paResult = static_cast<V>(paIN1 + paIN2);
    return false;
  }

  template<typename V>
  bool sub_overflow(V paIN1, V paIN2, V &paResult) {
    static_assert(std::is_integral_v<V>);
    if constexpr (std::is_signed_v<V>) {
      if ((paIN2 < V(0) && paIN1 > std::numeric_limits<V>::max() + paIN2) ||
          (paIN2 > V(0) && paIN1 < std::numeric_limits<V>::min() + paIN2)) {
        return true;
      }
    } else {
      if (paIN1 < paIN2) {
        return true;
      }
    }
    paResult = static_cast<V>(paIN1 - paIN2);
    return false;
  }

  template<typename V>
  bool mul_overflow(V paIN1, V paIN2, V &paResult) {
    static_assert(std::is_integral_v<V>);
    if (paIN1 == V(0) || paIN2 == V(0)) {
      paResult = V(0);
      return false;
    }
    if constexpr (std::is_signed_v<V>) {
      const V maxV = std::numeric_limits<V>::max();
      const V minV = std::numeric_limits<V>::min();
      // Classic pre-multiply range check: divide instead of multiplying, so
      // the check itself can never overflow (or trigger the INT_MIN / -1 UB
      // case, since every division below has a numerator of maxV or a minV
      // divided by an operand of magnitude >= 1 whose sign keeps it != -1).
      if (paIN1 > V(0)) {
        if (paIN2 > V(0)) {
          if (paIN1 > maxV / paIN2) return true;
        } else {
          if (paIN2 < minV / paIN1) return true;
        }
      } else {
        if (paIN2 > V(0)) {
          if (paIN1 < minV / paIN2) return true;
        } else {
          if (paIN1 < maxV / paIN2) return true;
        }
      }
    } else {
      if (paIN1 > std::numeric_limits<V>::max() / paIN2) return true;
    }
    paResult = static_cast<V>(paIN1 * paIN2);
    return false;
  }
#endif

  // Common floating-point result handling for safe_{add,sub,mul,div}_native: a
  // non-finite result is a limit condition regardless of whether it came from
  // finite operands overflowing or from an already non-finite operand (e.g.
  // REAL#INF + 1, or REAL#INF - REAL#INF producing NaN) - both cases must be
  // reported via paLimitHit rather than silently leaking inf/NaN to OUT.
  template<typename V>
  V clamp_non_finite_result(V paResult, bool &paLimitHit) {
    if (std::isfinite(paResult)) {
      return paResult;
    }
    paLimitHit = true;
    if (std::isnan(paResult)) {
      return V(0);
    }
    return (paResult > V(0)) ? std::numeric_limits<V>::max() : std::numeric_limits<V>::lowest();
  }

} // namespace forte::SafeArithmetic::arithmetic::detail

namespace forte::SafeArithmetic::arithmetic {

  template<typename V>
  V safe_add_native(V paIN1, V paIN2, bool &paLimitHit) {
    if constexpr (std::is_integral_v<V>) {
      V result;
      if (detail::add_overflow(paIN1, paIN2, result)) {
        paLimitHit = true;
        if constexpr (std::is_signed_v<V>) {
          return (paIN2 >= 0) ? std::numeric_limits<V>::max() : std::numeric_limits<V>::min();
        } else {
          return std::numeric_limits<V>::max();
        }
      }
      return result;
    } else {
      return detail::clamp_non_finite_result(paIN1 + paIN2, paLimitHit);
    }
  }

  template<typename V>
  V safe_sub_native(V paIN1, V paIN2, bool &paLimitHit) {
    if constexpr (std::is_integral_v<V>) {
      V result;
      if (detail::sub_overflow(paIN1, paIN2, result)) {
        paLimitHit = true;
        if constexpr (std::is_signed_v<V>) {
          return (paIN2 >= 0) ? std::numeric_limits<V>::min() : std::numeric_limits<V>::max();
        } else {
          // Unsigned subtraction can only ever underflow (go below 0).
          return std::numeric_limits<V>::min();
        }
      }
      return result;
    } else {
      return detail::clamp_non_finite_result(paIN1 - paIN2, paLimitHit);
    }
  }

  template<typename V>
  V safe_mul_native(V paIN1, V paIN2, bool &paLimitHit) {
    if constexpr (std::is_integral_v<V>) {
      V result;
      if (detail::mul_overflow(paIN1, paIN2, result)) {
        paLimitHit = true;
        if constexpr (std::is_signed_v<V>) {
          const bool negative = (paIN1 < 0) != (paIN2 < 0);
          return negative ? std::numeric_limits<V>::min() : std::numeric_limits<V>::max();
        } else {
          return std::numeric_limits<V>::max();
        }
      }
      return result;
    } else {
      return detail::clamp_non_finite_result(paIN1 * paIN2, paLimitHit);
    }
  }

  template<typename V>
  V safe_div_native(V paIN1, V paIN2, bool &paLimitHit) {
    if constexpr (std::is_integral_v<V>) {
      if (paIN2 == V(0)) {
        paLimitHit = true;
        return V(0);
      }
      if constexpr (std::is_signed_v<V>) {
        // INT_MIN / -1 is the only integer division that can overflow.
        if (paIN1 == std::numeric_limits<V>::min() && paIN2 == V(-1)) {
          paLimitHit = true;
          return std::numeric_limits<V>::max();
        }
      }
      return static_cast<V>(paIN1 / paIN2);
    } else {
      if (paIN2 == V(0)) {
        paLimitHit = true;
        return V(0);
      }
      return detail::clamp_non_finite_result(paIN1 / paIN2, paLimitHit);
    }
  }

  template<typename T, typename U>
  auto safe_add(const T &paIN1, const U &paIN2, bool &paLimitHit) -> typename mpl::get_add_operator_result_type_t<T, U> {
    using resultType = typename mpl::get_add_operator_result_type_t<T, U>;
    if constexpr (std::is_arithmetic_v<typename resultType::TValueType>) {
      using V = typename resultType::TValueType;
      const auto native1 = static_cast<V>(static_cast<typename T::TValueType>(paIN1));
      const auto native2 = static_cast<V>(static_cast<typename U::TValueType>(paIN2));
      return resultType(safe_add_native<V>(native1, native2, paLimitHit));
    } else {
      return func_ADD(paIN1, paIN2);
    }
  }

  template<typename T, typename U>
  auto safe_sub(const T &paIN1, const U &paIN2, bool &paLimitHit) -> typename mpl::get_sub_operator_result_type_t<T, U> {
    using resultType = typename mpl::get_sub_operator_result_type_t<T, U>;
    if constexpr (std::is_arithmetic_v<typename resultType::TValueType>) {
      using V = typename resultType::TValueType;
      const auto native1 = static_cast<V>(static_cast<typename T::TValueType>(paIN1));
      const auto native2 = static_cast<V>(static_cast<typename U::TValueType>(paIN2));
      return resultType(safe_sub_native<V>(native1, native2, paLimitHit));
    } else {
      return func_SUB(paIN1, paIN2);
    }
  }

  template<typename T, typename U>
  auto safe_mul(const T &paIN1, const U &paIN2, bool &paLimitHit) -> typename mpl::get_mul_operator_result_type_t<T, U> {
    using resultType = typename mpl::get_mul_operator_result_type_t<T, U>;
    using V = typename resultType::TValueType;
    static_assert(std::is_arithmetic_v<V>, "SAFE_MUL result type must be numeric (ANY_NUM)");
    const auto native1 = static_cast<V>(static_cast<typename T::TValueType>(paIN1));
    const auto native2 = static_cast<V>(static_cast<typename U::TValueType>(paIN2));
    return resultType(safe_mul_native<V>(native1, native2, paLimitHit));
  }

  template<typename T, typename U>
  auto safe_div(const T &paIN1, const U &paIN2, bool &paLimitHit) -> typename mpl::get_div_operator_result_type_t<T, U> {
    using resultType = typename mpl::get_div_operator_result_type_t<T, U>;
    using V = typename resultType::TValueType;
    static_assert(std::is_arithmetic_v<V>, "SAFE_DIV result type must be numeric (ANY_NUM)");
    const auto native1 = static_cast<V>(static_cast<typename T::TValueType>(paIN1));
    const auto native2 = static_cast<V>(static_cast<typename U::TValueType>(paIN2));
    return resultType(safe_div_native<V>(native1, native2, paLimitHit));
  }

} // namespace forte::SafeArithmetic::arithmetic
