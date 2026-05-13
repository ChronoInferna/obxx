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
    std::vector<OrderId> orders;
    OrderQuantity volume;
    uint64_t trade_quantity;
    std::unordered_map<OrderId, std::unique_ptr<Order>>* orders_ptr;  // Pointer for O(1)

    void add_order(OrderId id);
    // std::optional<Order> fill_order(Order& order);
    // TODO
  };

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

  // TODO how to notify when order is filled?
  // TODO decide what kinds of updates to expose
  // TODO move out of this file
  using Event = std::variant<Trade, Update>;

  class OrderBook
  {
   private:
    std::unordered_map<Decimal<2>, PriceLevel> bids_;
    std::unordered_map<Decimal<2>, PriceLevel> asks_;
    // Pointer to orders for O(1) cancel
    std::unordered_map<OrderId, std::unique_ptr<Order>> orders_;

   public:
    // Input
    std::expected<OrderId, std::string> submit_order_request(OrderRequest& request);
    std::expected<void, std::string> cancel_order(OrderId id);

    // Ouput
    std::vector<Event> poll_events(int num_requests) const;  // How to determine how many requests to give?

    // Queries
    std::optional<Decimal<2>> best_bid() const;
    std::optional<Decimal<2>> best_ask() const;
    std::optional<Order> query_order_id(OrderId id) const;
    std::optional<OrderQuantity> volume_at_price(Decimal<2> price) const;

   private:
    OrderId create_order(OrderRequest& request);
    void add_order_to_bids(OrderId id);
    void add_order_to_asks(OrderId id);

    // TODO if order > volume at price level, we can just clear and mark all as filled
    void clear_bids(Decimal<2> price);
    void clear_asks(Decimal<2> price);

    std::expected<OrderId, std::string> submit_buy_request(OrderRequest& request);
    std::expected<OrderId, std::string> submit_market_buy_request(OrderRequest& request);
    // TODO add more order types

    std::expected<OrderId, std::string> submit_sell_request(OrderRequest& request);
    std::expected<OrderId, std::string> submit_market_sell_request(OrderRequest& request);
    // TODO add more order types
  };

}  // namespace obxx
