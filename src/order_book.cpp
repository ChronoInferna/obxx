#include "obxx/order_book.hpp"

namespace obxx
{

  void PriceLevel::add_order(OrderId id)
  {
    orders.push_back(id);
    volume += (*orders_ptr)[id]->quantity;
    trade_quantity++;
  }

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
          price_level.volume -= orders_[id]->quantity;
          orders_[id]->status = OrderStatus::Filled;
          break;
        }
        else
        {
          // Partially fill order and remove price level
          orders_[id]->quantity -= price_level.volume;
          price_level.volume = 0;
          price_level.orders.clear();
        }
      }
    }

    return id;
  }

  std::expected<OrderId, std::string> OrderBook::submit_sell_request(OrderRequest& request)
  {
    Order order{ request };
    return std::unexpected("Unimplemented, cannot submit sell request");
  }

  std::expected<void, std::string> OrderBook::cancel_order(OrderId id)
  {
    return std::unexpected("Unimplemented, cannot cancel order " + std::to_string(id));
  }

}  // namespace obxx
