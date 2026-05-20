set(sources
    src/order_book.cpp
    src/order.cpp
)

set(exe_sources
    examples/main.cpp
    ${sources}
)

set(headers
    include/obxx/order_book.hpp
    include/obxx/order.hpp
    include/obxx/decimal.hpp
)

set(test_sources
  src/test_decimal.cpp
  src/test_order.cpp
  src/test_order_book.cpp
)
