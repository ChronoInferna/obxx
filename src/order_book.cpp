#include "obxx/order_book.hpp"

namespace obxx
{

  // ===== PriceLevel methods ======

  void PriceLevel::add_order(OrderId id)
  {
    orders.push_back(id);
    volume += (*orders_map_ptr)[id]->quantity;
    trade_quantity++;
  }

  OrderQuantity PriceLevel::fill_quantity(OrderQuantity quantity)
  {
    if (quantity < volume)
    {
      volume -= quantity;
      for (auto id : orders)
      {
        quantity = fill_quantity_single(id, quantity);
        if (quantity == 0)
        {
          break;
        }
      }
      return 0;
    }
    else
    {
      volume = 0;
      trade_quantity = 0;
      orders.clear();
      return quantity;
    }
  }

  OrderQuantity PriceLevel::fill_quantity_single(OrderId id, OrderQuantity quantity)
  {
    if (quantity < (*orders_map_ptr)[id]->quantity)
    {
      volume -= quantity;

      (*orders_map_ptr)[id]->quantity -= quantity;
      (*orders_map_ptr)[id]->status = OrderStatus::PartiallyFilled;

      return 0;
    }
    else
    {
      quantity -= (*orders_map_ptr)[id]->quantity;
      volume -= (*orders_map_ptr)[id]->quantity;

      (*orders_map_ptr)[id]->quantity = 0;
      (*orders_map_ptr)[id]->status = OrderStatus::Filled;

      // Remove order from price level
      orders.erase(std::remove(orders.begin(), orders.end(), id), orders.end());
      trade_quantity--;

      return quantity;
    }
  }

  // ===== Order handling methods =====

  OrderId OrderBook::create_order(OrderRequest& request)
  {
    auto order = std::make_unique<Order>(request);
    OrderId order_id = OrderIdGenerator::next();
    orders_[order_id] = std::move(order);
    return order_id;
  }

  void OrderBook::add_order_to_bids(OrderId id)
  {
    Decimal<2> price = orders_[id]->price.value();
    if (bids_.find(price) == bids_.end())
    {
      bids_[price] = PriceLevel{ {}, 0, 0, &orders_ };
    }
    bids_[price].add_order(id);
  }

  void OrderBook::add_order_to_asks(OrderId id)
  {
    Decimal<2> price = orders_[id]->price.value();
    if (asks_.find(price) == asks_.end())
    {
      asks_[price] = PriceLevel{ {}, 0, 0, &orders_ };
    }
    bids_[price].add_order(id);
  }

  std::expected<OrderId, std::string> OrderBook::submit_order_request(OrderRequest& request)
  {
    if (request.side == OrderSide::Buy)
    {
      return submit_buy_request(request);
    }
    else
    {
      return submit_sell_request(request);
    }
  }

  std::expected<void, std::string> OrderBook::cancel_order(OrderId id)
  {
    return std::unexpected("Unimplemented, cannot cancel order " + std::to_string(id));
  }

  // ====== Buy order handling =====

  std::expected<OrderId, std::string> OrderBook::submit_buy_request(OrderRequest& request)
  {
    switch (request.type)
    {
      case OrderType::Market: return submit_market_buy_request(request);
      case OrderType::Limit:
        // TODO
        return std::unexpected("Unimplemented, cannot submit limit buy request");
      case OrderType::Stop:
        // TODO
        return std::unexpected("Unimplemented, cannot submit stop buy request");
      case OrderType::StopLimit:
        // TODO
        return std::unexpected("Unimplemented, cannot submit stop limit buy request");
      default: return std::unexpected("Invalid order type");
    }
  }

  std::expected<OrderId, std::string> OrderBook::submit_limit_buy_request(OrderRequest& request)
  {
    return std::unexpected("Unimplemented, cannot submit limit buy request");
  }

  std::expected<OrderId, std::string> OrderBook::submit_market_buy_request(OrderRequest& request)
  {
    OrderId id = create_order(request);

    // Determine if it fills
    if (bids_.empty())
    {
      orders_[id]->status = OrderStatus::Rejected;
    }
    else  // Fill
    {
      for (auto& [price, price_level] : asks_)
      {
        if (orders_[id]->quantity <= price_level.volume)
        {
          // Fill order and update price level
          price_level.fill_quantity(request.quantity);

          orders_[id]->status = OrderStatus::Filled;
          break;
        }
        else
        {
          price_level.fill_quantity(request.quantity);

          // Partially fill order and remove price level
          orders_[id]->quantity -= price_level.volume;
          orders_[id]->status = OrderStatus::PartiallyFilled;
        }
      }
    }

    return id;
  }

  // ===== Sell order handling =====

  std::expected<OrderId, std::string> OrderBook::submit_sell_request(OrderRequest& request)
  {
    return std::unexpected("Unimplemented, cannot submit sell request");
  }

  std::expected<OrderId, std::string> OrderBook::submit_limit_sell_request(OrderRequest& request)
  {
    return std::unexpected("Unimplemented, cannot submit limit sell request");
  }

  std::expected<OrderId, std::string> OrderBook::submit_market_sell_request(OrderRequest& request)
  {
    return std::unexpected("Unimplemented, cannot submit market sell request");
  }

  // ===== Output and query methods =====

  std::vector<Event> OrderBook::poll_events(int num_requests) const
  {
    // TODO
    return {};
  }

  std::optional<Decimal<2>> OrderBook::best_bid() const
  {
    if (bids_.empty())
      return std::nullopt;
    return bids_.begin()->first;
  }

  std::optional<Decimal<2>> OrderBook::best_ask() const
  {
    if (asks_.empty())
      return std::nullopt;
    return asks_.begin()->first;
  }

  std::optional<Order> OrderBook::query_order_id(OrderId id) const
  {
    // TODO
    return std::nullopt;
  }

  std::optional<OrderQuantity> OrderBook::volume_at_price(Decimal<2> price) const
  {
    // TODO
    return std::nullopt;
  }

}  // namespace obxx
