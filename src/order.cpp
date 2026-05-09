#include "obxx/order.hpp"

namespace obxx
{

  OrderRequestBuilder::OrderRequestBuilder(Decimal<2> price, OrderSide side, OrderVolume volume, OrderType type)
  {
    request_ = std::make_unique<OrderRequest>(price, side, volume, type);
  }

  std::expected<OrderRequest, std::string> OrderRequestBuilder::build()
  {
    auto request = *request_;

    if (request.price < 0)
    {
      return std::unexpected("Price is negative");
    }
    if (request.volume < 0)
    {
      return std::unexpected("Volume is negative");
    }

    return request;
  }

}  // namespace obxx
