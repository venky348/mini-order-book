#include <iostream>
#include <string>


enum class OrderType {
    BUY,
    SELL
};

struct Order {
    int id;
    OrderType type;
    double price;
    int quantity;
};



void printOrder(const Order& order) {
    std::cout << "Order ID: " << order.id << ", Type: " 
              << (order.type == OrderType::BUY ? "BUY" : "SELL")
              << ", Price : " << order.price << ", Quantity : " << order.quantity
              << std::endl;
}

int main() {
    Order order1 = {1, OrderType::BUY, 100.5, 10};
    Order order2 = {2, OrderType::SELL, 101.0, 5};
    Order order3 = {3, OrderType::BUY, 99.5, 20};

    printOrder(order1);
    printOrder(order2);
    printOrder(order3);

    return 0;

}