#pragma once

#include <atomic>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

#include "decimal.hpp"

namespace obxx
{

  enum class OrderStatus : uint8_t
  {
    Unfilled,
    PartiallyFilled,
    Filled,
    Canceled,
    Rejected,
    Expired
  };

  std::string_view to_string(OrderStatus status);

  using OrderId = uint64_t;
  class OrderIdGenerator
  {
   private:
    inline static std::atomic<OrderId> current_id_ = 0;

   public:
    static OrderId next();
  };

  enum class OrderType : uint8_t
  {
    Market,
    Limit,
    Stop,
    StopLimit,
    Count
  };

  enum class OrderSide : uint8_t
  {
    Buy,
    Sell
  };

  using OrderQuantity = int64_t;

  class OrderRequestBuilder;

  struct OrderRequest
  {
    OrderType type;
    OrderSide side;
    OrderQuantity quantity;

    // Optional fields
    std::optional<Decimal<2>> price;

   private:
    OrderRequest(OrderType request_type, OrderSide request_side, OrderQuantity request_quantity);

    friend class OrderRequestBuilder;  // Allow builder to access private constructor
  };

  class OrderBuilder;

  struct Order
  {
    OrderStatus status;

    OrderType type;
    OrderSide side;
    OrderQuantity quantity;

    // Optional fields
    std::optional<Decimal<2>> price;

   private:
    Order(const OrderRequest& request);  // Private constructor to enforce creation from OrderRequest

    friend class OrderBuilder;  // Allow builder to access private constructor
  };

  class OrderRequestBuilder
  {
   private:
    std::unique_ptr<OrderRequest> request_;

   public:
    OrderRequestBuilder(OrderType type, OrderSide side, OrderQuantity quantity);
    OrderRequestBuilder& price(Decimal<2> price);
    std::expected<OrderRequest, std::string> build();
  };

  class OrderBuilder
  {
   public:
    static std::pair<OrderId, std::unique_ptr<Order>> build(const OrderRequest& request);

   private:
    OrderBuilder() = default;  // Private constructor to prevent instantiation
  };

}  // namespace obxx
