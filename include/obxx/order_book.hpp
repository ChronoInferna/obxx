#pragma once

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "decimal.hpp"
#include "order.hpp"

namespace obxx
{

  struct PriceLevel
  {
    std::vector<Order> orders;
    OrderVolume volume;
    uint64_t order_quantity;

    void add_order(Order& order);
    // TODO
  };

  struct Trade
  {
    OrderId taker;
    OrderId maker;
    Decimal<2> price;
    OrderVolume volume;
  };

  struct Update
  {
    Decimal<2> price;
    OrderVolume volume;
    OrderSide side;
  };

  // TODO how to notify when order is filled?
  // TODO decide what kinds of updates to expose
  using Event = std::variant<Trade, Update>;

  class OrderBook
  {
   private:
    // TODO best data structure for this? std::flat_map? Only in GCC 15 though
    std::unordered_map<Decimal<2>, PriceLevel> bids_;
    std::unordered_map<Decimal<2>, PriceLevel> asks_;

    // TODO if order > volume at price level, we can just clear and mark all as filled
    void clear_bids();
    void clear_asks();

   public:
    // Input
    OrderId submit_order_request(OrderRequest& request);
    void cancel_order(OrderId id);

    // Ouput
    std::vector<Event> poll_events(int num_requests) const;  // How to determine how many requests to give?

    // Queries
    std::optional<Decimal<2>> best_bid() const;
    std::optional<Decimal<2>> best_ask() const;
    std::optional<Order> query_order_id(OrderId id) const;
    OrderVolume volume_at_price(Decimal<2> price) const;
  };

}  // namespace obxx
