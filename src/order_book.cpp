#include "obxx/order_book.hpp"

#include <algorithm>
#include <stdexcept>

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

  void PriceLevel::remove_order(OrderId id)
  {
    if (std::find(orders.begin(), orders.end(), id) == orders.end())
      return;

    volume -= (*orders_map_ptr)[id]->quantity;
    trade_quantity--;
    orders.erase(std::remove(orders.begin(), orders.end(), id), orders.end());
    return;
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

  OrderId OrderBook::submit_order_request(OrderRequest& request)
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

  bool OrderBook::cancel_order(OrderId id)
  {
    if (orders_.find(id) == orders_.end())
    {
      return false;
    }

    orders_[id]->status = OrderStatus::Canceled;
    Decimal<2> price = orders_[id]->price.value();
    if (orders_[id]->side == OrderSide::Buy)
    {
      bids_[price].remove_order(id);
    }
    else
    {
      asks_[price].remove_order(id);
    }

    return true;
  }

  // ====== Buy order handling =====

  OrderId OrderBook::submit_buy_request(OrderRequest& request)
  {
    switch (request.type)
    {
      case OrderType::Market: return submit_market_buy_request(request);
      case OrderType::Limit: return submit_limit_buy_request(request);
      case OrderType::Stop:
        // TODO
        throw std::runtime_error("Unimplemented, cannot submit stop buy request");
      case OrderType::StopLimit:
        // TODO
        throw std::runtime_error("Unimplemented, cannot submit stop limit buy request");
      default: throw std::runtime_error("Invalid order type");
    }
  }

  OrderId OrderBook::submit_limit_buy_request(OrderRequest& request)
  {
    OrderId id = create_order(request);

    // Fill (if crosses book) until can't
    if (!asks_.empty() && request.price.value() >= asks_.begin()->first)
    {
      for (auto& [price, price_level] : asks_)
      {
        if (request.price.value() >= price)
        {
          if (orders_[id]->quantity <= price_level.volume)
          {
            // Fill
            price_level.fill_quantity(request.quantity);

            orders_[id]->status = OrderStatus::Filled;
            break;
          }
          else
          {
            // Partially fill
            orders_[id]->quantity -= price_level.volume;

            price_level.fill_quantity(request.quantity);

            orders_[id]->status = OrderStatus::PartiallyFilled;
          }
        }
        else
        {
          break;
        }
      }
    }

    // Add to book if not filled
    if (orders_[id]->status != OrderStatus::Filled)
    {
      add_order_to_bids(id);
    }

    return id;
  }

  OrderId OrderBook::submit_market_buy_request(OrderRequest& request)
  {
    OrderId id = create_order(request);

    // Determine if it fills
    if (bids_.empty())
    {
      orders_[id]->status = OrderStatus::Canceled;
    }
    else  // Fill
    {
      for (auto& [price, price_level] : asks_)
      {
        if (orders_[id]->quantity <= price_level.volume)
        {
          // Fill
          price_level.fill_quantity(request.quantity);
          orders_[id]->status = OrderStatus::Filled;
          break;
        }
        else
        {
          // Partially fill
          orders_[id]->quantity -= price_level.volume;
          price_level.fill_quantity(request.quantity);
          orders_[id]->status = OrderStatus::PartiallyFilled;
        }
      }
    }

    // Cancel rest if not filled
    if (orders_[id]->status != OrderStatus::Filled)
    {
      orders_[id]->status = OrderStatus::Canceled;
    }
    // TODO fok?

    return id;
  }

  // ===== Sell order handling =====

  OrderId OrderBook::submit_sell_request(OrderRequest& request)
  {
    switch (request.type)
    {
      case OrderType::Market: return submit_market_sell_request(request);
      case OrderType::Limit: return submit_limit_sell_request(request);
      case OrderType::Stop:
        // TODO
        throw std::runtime_error("Unimplemented, cannot submit stop sell request");
      case OrderType::StopLimit:
        // TODO
        throw std::runtime_error("Unimplemented, cannot submit stop limit sell request");
      default: throw std::runtime_error("Invalid order type");
    }
  }

  OrderId OrderBook::submit_limit_sell_request(OrderRequest& request)
  {
    OrderId id = create_order(request);

    // Fill (if crosses book) until can't
    if (!bids_.empty() && request.price.value() <= bids_.begin()->first)
    {
      for (auto& [price, price_level] : bids_)
      {
        if (request.price.value() <= price)
        {
          if (orders_[id]->quantity <= price_level.volume)
          {
            // Fill
            price_level.fill_quantity(request.quantity);
            orders_[id]->status = OrderStatus::Filled;
            break;
          }
          else
          {
            // Partially fill
            orders_[id]->quantity -= price_level.volume;
            price_level.fill_quantity(request.quantity);
            orders_[id]->status = OrderStatus::PartiallyFilled;
          }
        }
        else
        {
          break;
        }
      }
    }

    // Add to book if not filled
    if (orders_[id]->status != OrderStatus::Filled)
    {
      add_order_to_bids(id);
    }

    return id;
  }

  OrderId OrderBook::submit_market_sell_request(OrderRequest& request)
  {
    OrderId id = create_order(request);

    // Determine if it fills
    if (asks_.empty())
    {
      orders_[id]->status = OrderStatus::Canceled;
    }
    else  // Fill
    {
      for (auto& [price, price_level] : bids_)
      {
        if (orders_[id]->quantity <= price_level.volume)
        {
          // Fill
          price_level.fill_quantity(request.quantity);
          orders_[id]->status = OrderStatus::Filled;
          break;
        }
        else
        {
          // Partially fill
          orders_[id]->quantity -= price_level.volume;
          price_level.fill_quantity(request.quantity);
          orders_[id]->status = OrderStatus::PartiallyFilled;
        }
      }
    }

    // Cancel rest if not filled
    if (orders_[id]->status != OrderStatus::Filled)
    {
      orders_[id]->status = OrderStatus::Canceled;  // TODO partially filled/canceled status?
    }
    // TODO fok?

    return id;
  }

  // ===== Output and query methods =====

  std::vector<Event> OrderBook::poll_events(int num_requests) const
  {
    // TODO
    return { Update{ Decimal<2>(0), num_requests, OrderSide::Buy } };
  }

  std::expected<Decimal<2>, std::string> OrderBook::query_best_bid() const
  {
    if (bids_.empty())
    {
      return std::unexpected("No bids in order book");
    }
    return bids_.begin()->first;
  }

  std::expected<Decimal<2>, std::string> OrderBook::query_best_ask() const
  {
    if (asks_.empty())
    {
      return std::unexpected("No asks in order book");
    }
    return asks_.begin()->first;
  }

  std::expected<Order, std::string> OrderBook::query_order_id(OrderId id) const
  {
    if (orders_.find(id) == orders_.end())
    {
      return std::unexpected("Order ID " + std::to_string(id) + " not found");
    }
    return *orders_.at(id);
  }

  std::expected<OrderQuantity, std::string> OrderBook::query_volume_at_price(Decimal<2> price) const
  {
    if (bids_.find(price) != bids_.end())
    {
      return bids_.at(price).volume;
    }
    else if (asks_.find(price) != asks_.end())
    {
      return asks_.at(price).volume;
    }
    else
    {
      return std::unexpected("Price " + price.to_string() + " not found in order book");
    }
  }

}  // namespace obxx
