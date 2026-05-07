#pragma once

#include <cstdint>

#include "decimal.hpp"

namespace obxx
{

  enum class OrderSide : uint8_t
  {
    Buy,
    Sell
  };

  enum class OrderType : uint8_t
  {
    Market,
    Limit,
    Stop,
    StopLimit,
    Count
  };

  using OrderId = uint64_t;
  using OrderQuantity = int64_t;

  struct Order
  {
    OrderId id;
    Decimal<2> price;
    OrderSide side;
    OrderType type;
    OrderQuantity quantity;
  };

  struct OrderRequest
  {
    Decimal<2> price;
    OrderSide side;
    OrderQuantity quantity;
    OrderType type;
  };

  class OrderRequestBuilder
  {
   private:
    OrderRequest* request_;

   public:
    OrderRequest& build();
  };

}  // namespace obxx
