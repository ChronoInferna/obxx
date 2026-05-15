#include <iostream>

#include "obxx/order_book.hpp"

// TODO move to docs/examples/ and add more examples of usage

int main()
{
  obxx::OrderBook order_book;

  // Example usage: submit a market buy order for 100 units and query its status
  auto build_result = obxx::OrderRequestBuilder(obxx::OrderType::Market, obxx::OrderSide::Buy, 100).build();
  if (!build_result.has_value())
  {
    std::cerr << "Failed to build order: " << build_result.error() << std::endl;
    return 1;
  }

  auto request = build_result.value();
  auto order_id = order_book.submit_order_request(request);

  auto order_query = order_book.query_order_id(order_id);

  std::cout << "Request 1: " << (order_query.has_value() ? obxx::to_string(order_query.value().status) : order_query.error())
            << std::endl;
}
