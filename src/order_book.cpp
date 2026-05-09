#include "obxx/order_book.hpp"

namespace obxx
{

  void PriceLevel::add_order(Order& order)
  {
    orders.emplace_back(std::move(order));
  }

}  // namespace obxx
