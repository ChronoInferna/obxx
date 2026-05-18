#pragma once

#include <map>
#include <unordered_map>
#include <variant>
#include <vector>

#include "decimal.hpp"
#include "order.hpp"

namespace obxx
{

  struct PriceLevel
  {
    std::vector<OrderId> orders;
    OrderQuantity volume;
    uint64_t trade_quantity;
    std::unordered_map<OrderId, std::unique_ptr<Order>>*
        orders_map_ptr;  // Pointer for O(1) - TODO could this be some sort of view?

    void add_order(OrderId id);
    OrderQuantity fill_quantity(OrderQuantity quantity);
    OrderQuantity fill_quantity_single(OrderId id, OrderQuantity quantity);
    void remove_order(OrderId id);
  };

  // TODO how to notify when order is filled?
  // TODO decide what kinds of updates to expose
  // TODO move out of this file
  struct Trade
  {
    OrderId taker;
    OrderId maker;
    Decimal<2> price;
    OrderQuantity volume;
  };
  struct Update
  {
    Decimal<2> price;
    OrderQuantity volume;
    OrderSide side;
  };
  using Event = std::variant<Trade, Update>;

  class OrderBook
  {
   private:
    std::map<Decimal<2>, PriceLevel, std::greater<>> bids_;
    std::map<Decimal<2>, PriceLevel, std::less<>> asks_;
    std::unordered_map<OrderId, std::unique_ptr<Order>> orders_;  // Pointer to orders for O(1) cancel

   public:
    // Input
    OrderId submit_order_request(OrderRequest& request);
    bool cancel_order(OrderId id);

    // Ouput
    std::vector<Event> poll_events(int num_requests) const;  // TODO How to determine how many requests to give?

    // Queries
    std::expected<Decimal<2>, std::string> query_best_bid() const;
    std::expected<Decimal<2>, std::string> query_best_ask() const;
    std::expected<Order, std::string> query_order_id(OrderId id) const;
    std::expected<OrderQuantity, std::string> query_volume_at_price(Decimal<2> price) const;

   private:
    OrderId create_order(OrderRequest& request);
    void add_order_to_bids(OrderId id);
    void add_order_to_asks(OrderId id);

    OrderId submit_buy_request(OrderRequest& request);
    OrderId submit_limit_buy_request(OrderRequest& request);
    OrderId submit_market_buy_request(OrderRequest& request);
    // TODO add more order types

    OrderId submit_sell_request(OrderRequest& request);
    OrderId submit_limit_sell_request(OrderRequest& request);
    OrderId submit_market_sell_request(OrderRequest& request);
    // TODO add more order types
  };

}  // namespace obxx
