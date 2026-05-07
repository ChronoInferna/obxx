#pragma once

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "decimal.hpp"
#include "order.hpp"

namespace obxx
{

  class PriceLevel
  {
  };

  struct Trade
  {
    OrderId taker;
    OrderId maker;
    Decimal<2> price;
    OrderQuantity quantity;
  };

  struct Update
  {
    Decimal<2> price;
    OrderQuantity quantity;
    OrderSide side;
  };

  using Event = std::variant<Trade, Update>;

  class OrderBook
  {
   private:
    // TODO best data structure for this? std::flat_map? Only in GCC 15 though
    std::unordered_map<Decimal<2>, PriceLevel> bids_;
    std::unordered_map<Decimal<2>, PriceLevel> asks_;

   public:
    // Input
    OrderId submit_order_request(OrderRequest& request);

    // Ouput
    std::vector<Event> poll_events(int num_requests);  // How to determine how many requests to give?

    // Queries
    std::optional<Decimal<2>> best_bid() const;
    std::optional<Decimal<2>> best_ask() const;
  };

}  // namespace obxx
