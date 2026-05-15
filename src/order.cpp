#include "obxx/order.hpp"

namespace obxx
{

  std::string_view to_string(OrderStatus status)
  {
    switch (status)
    {
      case OrderStatus::Unfilled: return "Unfilled";
      case OrderStatus::PartiallyFilled: return "PartiallyFilled";
      case OrderStatus::Filled: return "Filled";
      case OrderStatus::Canceled: return "Canceled";
      case OrderStatus::Rejected: return "Rejected";
      case OrderStatus::Expired: return "Expired";
      default: return "Unknown";
    }
  }

  // TODO: Implement a more robust ID generator that can handle overflow and persistence - also a better ID system
  OrderId OrderIdGenerator::next()
  {
    return current_id_++;
  }

  Order::Order(const OrderRequest& request)
      : status(OrderStatus::Unfilled),
        type(request.type),
        side(request.side),
        quantity(request.quantity),
        price(request.price)
  {
  }

  OrderRequestBuilder::OrderRequestBuilder(OrderType type, OrderSide side, OrderQuantity quantity)
  {
    request_ = std::make_unique<OrderRequest>(type, side, quantity);
  }

  std::expected<OrderRequest, std::string> OrderRequestBuilder::build()
  {
    // TODO
    if (request_->side != OrderSide::Buy && request_->side != OrderSide::Sell)
    {
      return std::unexpected("Invalid order type");
    }

    if (request_->quantity < 0)
    {
      return std::unexpected("Volume is negative");
    }

    if (request_->type == OrderType::Market && request_->price.has_value())
    {
      return std::unexpected("Market orders do not require a price");
    }

    return *request_;
  }

  OrderRequestBuilder& OrderRequestBuilder::price(Decimal<2> price)
  {
    request_->price = price;
    return *this;
  }

}  // namespace obxx
