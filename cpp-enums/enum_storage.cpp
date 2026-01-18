#include <iostream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY='B', SELL='S' };

int main() {
  OrderType type1 = OrderType::SELL;
  OrderType2 type2 = OrderType2::SELL;
  
  std::cout << "sizeof(OrderType): " << sizeof(type1) << "\n";
  std::cout << "sizeof(OrderType2): " << sizeof(type2) << "\n";
  
  return 0;
}
