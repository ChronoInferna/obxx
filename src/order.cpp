#include "obxx/order.hpp"

namespace obxx
{

  // TODO: Implement a more robust ID generator that can handle overflow and persistence - also a better ID system
  OrderId OrderIdGenerator::next()
  {
    return current_id_++;
  }

  Order::Order(const OrderRequest& request)
      : status(OrderStatus::Unfilled),
        side(request.side),
        quantity(request.quantity),
        type(request.type),
        price(request.price)
  {
  }

  OrderRequestBuilder::OrderRequestBuilder(OrderSide side, OrderQuantity quantity, OrderType type)
  {
    request_ = std::make_unique<OrderRequest>(side, quantity, type);
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

  OrderRequest& OrderRequestBuilder::price(Decimal<2> price)
  {
    request_->price = price;
    return *request_;
  }

}  // namespace obxx
