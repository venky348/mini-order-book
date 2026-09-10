#include <iostream>
#include <vector>


enum class Side {
    BUY,
    SELL
};

struct Order {
    int id;
    Side side;
    double price;
    int quantity;
};



void printOrder(const Order& order) {
    std::cout << "Order ID: " << order.id << ", Side: " 
              << (order.side == Side::BUY ? "BUY" : "SELL")
              << ", Price : " << order.price << ", Quantity : " << order.quantity
              << std::endl;
}

int main() {
    std::vector<Order> orders;
    orders.push_back({1, Side::BUY, 100.5, 10});
    orders.push_back({2, Side::SELL, 101.0, 5});
    orders.push_back({3, Side::BUY, 99.5, 20});

    for (const auto& order : orders) {
        printOrder(order);
    }

    return 0;

}