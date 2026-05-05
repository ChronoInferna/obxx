#pragma once

#include <cmath>
#include <cstdint>
#include <string>

namespace obxx
{

  /*
   * @brief RoundingMode is an enumeration that defines various rounding modes for decimal arithmetic operations. These modes
   * specify how to round a number when it cannot be represented exactly in the desired precision.
   */
  enum class RoundingMode : uint8_t
  {
    Down,      // Round towards negative infinity
    Up,        // Round towards positive infinity
    HalfUp,    // Round towards the nearest neighbor; if equidistant, round up
    HalfDown,  // Round towards the nearest neighbor; if equidistant, round down
    HalfEven,  // Round towards the nearest neighbor; if equidistant, round towards the even neighbor - banking standard
    HalfOdd,   // Round towards the nearest neighbor; if equidistant, round towards the odd neighbor
    Count      // Sentinel value to indicate the number of rounding modes
  };

  /*
   * @brief Decimal is a fixed-point decimal type that allows for precise representation of decimal numbers without the
   * issues associated with floating-point arithmetic.
   *
   * @tparam Precision The number of decimal places to represent. For example, if Precision is 2, then the Decimal can
   * represent numbers with up to 2 decimal places (e.g., 1.23). The internal representation is an integer that is scaled by
   * 10^Precision.
   */
  template<int Precision>
  class Decimal
  {
    static constexpr int MIN_DECIMAL_DIGITS =
        0;  // Minimum number of decimal digits that can be represented (0 means no decimal places, i.e., an integer)
    static constexpr int MAX_DECIMAL_DIGITS =
        18;  // Maximum number of decimal digits that can be represented without overflow in the internal representation
    static_assert(
        Precision >= MIN_DECIMAL_DIGITS && Precision <= MAX_DECIMAL_DIGITS,
        "Precision must be between 0 and 18 to avoid overflow in the internal representation.");
    using rep = int64_t;

   private:
    rep value_;

   public:
    constexpr Decimal() : value_(0)
    {
    }
    /*
     * @brief Constructs a Decimal from a raw integer representation. The `scale` parameter indicates whether the input
     * number should be treated as already scaled (i.e., it represents the internal integer value) or if it should be scaled
     * by 10^Precision. If `scale` is false, the input number is multiplied by 10^Precision to convert it to the internal
     * representation. If `scale` is true, the input number is used directly as the internal representation without
     * additional scaling. This allows for flexibility in constructing Decimal instances from both raw integer values and
     * unscaled numbers.
     *
     * @param num The input number to be converted to a Decimal. This can be either a raw integer representation (if `scale`
     * is true) or an unscaled number (if `scale` is false).
     * @param scale A boolean flag indicating whether the input number is already scaled (true) or if it should be scaled by
     * 10^Precision (false). The default value is false, meaning that the input number will be treated as an unscaled value
     * and will be multiplied by 10^Precision to convert it to the internal representation.
     */
    explicit constexpr Decimal(rep num, bool is_scaled = false)
        : value_(is_scaled ? num * static_cast<rep>(std::pow(10, Precision)) : num)
    {
    }
    explicit constexpr Decimal(double input) : value_(from_double(input).value_)
    {
    }

    static constexpr Decimal from_double(double input)
    {
      const double scaled = input * std::pow(10, Precision);
      const rep unscaled = static_cast<rep>(std::round(scaled));
      return Decimal(unscaled);
    }

    constexpr double to_double() const
    {
      return static_cast<double>(value_) / std::pow(10, Precision);
    }

    constexpr std::string to_string() const
    {
      rep unscaled = value_;
      std::string result;
      if (unscaled < 0)
      {
        result += '-';
        unscaled = -unscaled;
      }
      const rep scale = static_cast<rep>(std::pow(10, Precision));
      const rep integer_part = unscaled / scale;
      const rep fractional_part = unscaled % scale;

      result += std::to_string(integer_part);
      if (Precision > 0)
      {
        result += '.';
        std::string fractional_str = std::to_string(fractional_part);
        // Pad with leading zeros if necessary
        if (fractional_str.length() < static_cast<size_t>(Precision))
        {
          fractional_str.insert(0, static_cast<size_t>(Precision) - fractional_str.length(), '0');
        }
        result += fractional_str;
      }
      return result;
    }

    constexpr auto operator<=>(const Decimal& rhs) const = default;

    constexpr Decimal operator+(Decimal rhs) const
    {
      return Decimal(value_ + rhs.value_);
    }

    constexpr Decimal operator-(Decimal rhs) const
    {
      return Decimal(value_ - rhs.value_);
    }

    template<int ResultPrecision>
    constexpr Decimal<ResultPrecision> round(RoundingMode mode = RoundingMode::HalfEven) const
    {
      if constexpr (ResultPrecision >= Precision)
      {
        return Decimal<ResultPrecision>(value_ * static_cast<rep>(std::pow(10, ResultPrecision - Precision)));
      }
      else
      {
        const int scale_diff = Precision - ResultPrecision;
        const rep scale_factor = static_cast<rep>(std::pow(10, scale_diff));
        const rep new_unscaled_value = value_ / scale_factor;
        const rep remainder = value_ % scale_factor;

        switch (mode)
        {
          case RoundingMode::Down: return Decimal<ResultPrecision>(new_unscaled_value);
          case RoundingMode::Up: return Decimal<ResultPrecision>(new_unscaled_value + (remainder != 0 ? 1 : 0));
          case RoundingMode::HalfUp:
            return Decimal<ResultPrecision>(new_unscaled_value + (remainder >= scale_factor / 2 ? 1 : 0));
          case RoundingMode::HalfDown:
            return Decimal<ResultPrecision>(new_unscaled_value + (remainder <= scale_factor / 2 ? 0 : 1));
          case RoundingMode::HalfEven:
            [[likely]];
            return Decimal<ResultPrecision>(
                new_unscaled_value +
                (remainder > scale_factor / 2 || (remainder == scale_factor / 2 && (new_unscaled_value % 2 != 0)) ? 1 : 0));
          case RoundingMode::HalfOdd:
            return Decimal<ResultPrecision>(
                new_unscaled_value +
                (remainder > scale_factor / 2 || (remainder == scale_factor / 2 && (new_unscaled_value % 2 != 0)) ? 0 : 1));
          default:
            // Invalid rounding mode; for safety, we can choose to round down as a fallback
            return Decimal<ResultPrecision>(new_unscaled_value);
        }
      }
    }

    [[nodiscard]]
    constexpr rep raw() const
    {
      return value_;
    }
  };

}  // namespace obxx

// Don't actually know if we need multiplication and division so I'm leaving this progress here in case we need it later
// Check if the compiler supports 128-bit integers. If it does, we can use __int128_t for wide_rep multiplication results. If
// not, we will fall back to int64_t, which may lead to overflow for large values
// #ifdef __SIZEOF_INT128__
// using wide_rep = __int128_t;
// #else
// using wide_rep = int64_t;
// #endif
//
//
// TO DO checked division and multiplication with overflow detection?
// TO DO unsure of the best way to handle multiplication and division with different precisions. We could either:
// 1. Require that the result precision is specified by the caller, and perform the necessary scaling to ensure the
// result has the correct number of decimal places. This would give the caller more control over the precision of the
// result, but it would also require them to understand the scaling factors involved.
// 2. Automatically determine the result precision based on the precisions of the operands. For example, we could choose
// the result precision to be the maximum of the two operand precisions, or we could choose it to be the sum of the two
// operand precisions to ensure that we don't lose any precision in the result. This would be more convenient for the
// caller, but it might lead to results with more decimal places than the caller expects, which could be an issue for
// display or further calculations.
// Either way, need to decide if we overload the * and / operators or create mul/div functions
//
// template<int N, int base>
// static constexpr wide_rep pow()
// {
//   wide_rep result = 1;
//
// #pragma unroll 5
//   for (int i = 0; i < N; ++i)
//   {
//     result *= base;
//   }
//   return result;
// }
//
// template<int OtherPrecision, int ResultPrecision>
// constexpr Decimal<ResultPrecision> operator*(Decimal<OtherPrecision> rhs) const
// {
//   constexpr wide_rep scale = pow<ResultPrecision - Precision - OtherPrecision, 10>();
//   return Decimal<ResultPrecision>(static_cast<wide_rep>(value_) * rhs.raw() * scale);
// }
//
// template<int OtherPrecision, int ResultPrecision>
// constexpr Decimal<ResultPrecision> operator/(Decimal<OtherPrecision> rhs) const
// {
//   // TO DO rounding mode
//   constexpr wide_rep scale = pow<ResultPrecision - Precision + OtherPrecision, 10>();
//   constexpr wide_rep numerator = static_cast<wide_rep>(value_) * scale;
//   return Decimal<ResultPrecision>(numerator / rhs.raw());
// }
