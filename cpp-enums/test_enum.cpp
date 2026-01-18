#include <iostream>

enum class OrderType { BUY, SELL };
enum class OrderType2 : char { BUY = 'B', SELL = 'S' };

int main() {
  OrderType type = OrderType::SELL;
  OrderType2 type2 = OrderType2::SELL;

  std::cout << static_cast<int>(type);
  std::cout << static_cast<char>(type2);

  return 0;
}
