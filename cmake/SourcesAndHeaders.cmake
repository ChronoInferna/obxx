set(sources
    src/order_book.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/obxx/order_book.hpp
    include/obxx/order.hpp
    include/obxx/decimal.hpp
)

set(test_sources
  src/test_decimal.cpp
)
