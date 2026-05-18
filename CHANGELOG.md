## 0.1.0 (2026-05-18)

### Feat

- implemented limit orders and sell side orders
- create example main.cpp
- string version of order status and reordered orders to be type, side, quantity
- reorganize and add filling functionality for price level
- introduce interaction with order
- restructure orders, add order status, and flesh out builder
- basics of some smaller functions
- add framework for order book
- **decimal.cpp**: start implementation of decimal class

### Fix

- **decimal.hpp**: comparison with floats
- decimal fixes
- remaining fixes on decimal class

### Refactor

- improve api boundary for order creation
- small changes to example
- decided to make things more imperative and use fewer std::expected functions since all the error checking should be in the order request builder stage
- move template again
- move template
- change an auto
- change to correct paths
- fill in template
