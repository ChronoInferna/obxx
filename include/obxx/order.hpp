#pragma once

#include <cstdint>
#include <memory>

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
  using OrderVolume = int64_t;

  struct Order
  {
    OrderId id;
    Decimal<2> price;
    OrderSide side;
    OrderType type;
    OrderVolume quantity;
  };

  struct OrderRequest
  {
    Decimal<2> price;
    OrderSide side;
    OrderVolume quantity;
    OrderType type;
  };

  class OrderRequestBuilder
  {
   private:
    std::unique_ptr<OrderRequest> request_;

   public:
    // TODO
    OrderRequestBuilder(Decimal<2> price, OrderSide side, OrderVolume volume, OrderType type);
    // OrderRequestBuilder& method();
    OrderRequest& build();
  };

}  // namespace obxx
