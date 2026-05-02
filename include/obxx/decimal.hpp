#pragma once

#include <cmath>
#include <cstdint>
#include <string_view>

// Check if the compiler supports 128-bit integers. If it does, we can use __int128_t for wide_rep multiplication results. If
// not, we will fall back to int64_t, which may lead to overflow for large values
#ifdef __SIZEOF_INT128__
using wide_rep = __int128_t;
#else
using wide_rep = int64_t;
#endif

namespace obxx
{

  enum class RoundingMode : uint8_t
  {
    Down,      // Round towards negative infinity
    Up,        // Round towards positive infinity
    HalfUp,    // Round towards the nearest neighbor; if equidistant, round up
    HalfDown,  // Round towards the nearest neighbor; if equidistant, round down
    HalfEven,  // Round towards the nearest neighbor; if equidistant, round towards the even neighbor - banking standard
    HalfOdd    // Round towards the nearest neighbor; if equidistant, round towards the odd neighbor
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
    explicit constexpr Decimal(rep unscaled) : value_(unscaled)
    {
    }

    static constexpr Decimal from_double(double input);
    static constexpr Decimal from_string(std::string_view input);

    constexpr bool operator==(Decimal rhs) const
    {
      return value_ == rhs.value_;
    }

    constexpr Decimal operator+(Decimal rhs) const
    {
      return Decimal(value_ + rhs.value_);
    }

    constexpr Decimal operator-(Decimal rhs) const
    {
      return Decimal(value_ - rhs.value_);
    }

    /*
     * @brief Compute base^N at compile time. This is used to calculate the scaling factor for the Decimal type. For
     * example, if Precision is 2, we need to scale by 10^2 = 100 to represent the decimal number as an integer.
     * @tparam N The exponent to which the base is raised.
     * @tparam base The base to be raised to the power of N. For our Decimal type, this will typically be 10, since we are
     * working with decimal numbers.
     * @return The result of base^N as a wide_rep integer type to prevent overflow during calculations.
     */
    template<int N, int base>
    static constexpr wide_rep pow()
    {
      wide_rep result = 1;

#pragma unroll 5
      for (int i = 0; i < N; ++i)
      {
        result *= base;
      }
      return result;
    }

    // TODO checked division and multiplication with overflow detection?
    // TODO unsure of the best way to handle multiplication and division with different precisions. We could either:
    // 1. Require that the result precision is specified by the caller, and perform the necessary scaling to ensure the
    // result has the correct number of decimal places. This would give the caller more control over the precision of the
    // result, but it would also require them to understand the scaling factors involved.
    // 2. Automatically determine the result precision based on the precisions of the operands. For example, we could choose
    // the result precision to be the maximum of the two operand precisions, or we could choose it to be the sum of the two
    // operand precisions to ensure that we don't lose any precision in the result. This would be more convenient for the
    // caller, but it might lead to results with more decimal places than the caller expects, which could be an issue for
    // display or further calculations.
    // Either way, need to decide if we overload the * and / operators or create mul/div functions
    template<int OtherPrecision, int ResultPrecision>
    constexpr Decimal<ResultPrecision> operator*(Decimal<OtherPrecision> rhs) const
    {
      constexpr wide_rep scale = pow<ResultPrecision - Precision - OtherPrecision, 10>();
      return Decimal<ResultPrecision>(static_cast<wide_rep>(value_) * rhs.raw() * scale);
    }

    template<int OtherPrecision, int ResultPrecision>
    constexpr Decimal<ResultPrecision> operator/(Decimal<OtherPrecision> rhs) const
    {
      // TODO rounding mode
      constexpr wide_rep scale = pow<ResultPrecision - Precision + OtherPrecision, 10>();
      constexpr wide_rep numerator = static_cast<wide_rep>(value_) * scale;
      return Decimal<ResultPrecision>(numerator / rhs.raw());
    }

    template<int ResultPrecision>
    constexpr Decimal<ResultPrecision> round(RoundingMode mode) const
    {  // TODO implement rounding modes
    }

    [[nodiscard]]
    constexpr rep raw() const
    {
      return value_;
    }
  };

}  // namespace obxx
