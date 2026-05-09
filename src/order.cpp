#include "obxx/order.hpp"

namespace obxx
{

  OrderRequestBuilder::OrderRequestBuilder(Decimal<2> price, OrderSide side, OrderVolume volume, OrderType type)
  {
    request_ = std::make_unique<OrderRequest>(price, side, volume, type);
  }

}  // namespace obxx
