#include <iostream>

#include "obxx/order_book.hpp"

int main()
{
  obxx::OrderBook order_book;

  // Example of building and submitting an order request, then querying the order status
  auto request1 = obxx::OrderRequestBuilder(obxx::OrderType::Market, obxx::OrderSide::Buy, 100).build();
  if (!request1.has_value())
  {
    std::cerr << "Failed to build order request: " << request1.error() << std::endl;
    return 1;
  }
  auto order_id = order_book.submit_order_request(*request1);
  if (!order_id.has_value())
  {
    std::cerr << "Failed to submit order request: " << order_id.error() << std::endl;
    return 1;
  }
  auto order_query = order_book.query_order_id(*order_id);

  std::cout << "Request 1: " << (order_query.has_value() ? obxx::to_string(order_query.value().status) : order_query.error())
            << std::endl;

  // Example using chaining with and_then to build, submit, and query an order request in one expression
  auto submit = [&order_book](obxx::OrderRequest r) { return order_book.submit_order_request(r); };
  auto query = [&order_book](obxx::OrderId id) { return order_book.query_order_id(id); };

  auto request2 =
      obxx::OrderRequestBuilder(obxx::OrderType::Market, obxx::OrderSide::Buy, 100).build().and_then(submit).and_then(query);

  std::cout << "Request 2: " << (request2.has_value() ? obxx::to_string(request2.value().status) : request2.error())
            << std::endl;

  return 0;
}
