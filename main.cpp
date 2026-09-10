#include <iostream>
#include <vector>
#include <map>


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

void addOrder(std::vector<Order>& orders, const Order& order) {
    orders.push_back(order);
}

void learnMap() {
    std::map<double, int> orders;
    orders[100.5] = 10;
    orders[100] = 20;
    orders[101] = 5;
    orders[99.5] = 15;

    for(const auto& order : orders) {
        std::cout << "Price: " << order.first << ", Quantity: " << order.second << std::endl;
    }
}

int main() {
    std::vector<Order> orders;
    addOrder(orders, {1, Side::BUY, 100.5, 10});
    addOrder(orders, {2, Side::SELL, 101.0, 5});
    addOrder(orders, {3, Side::BUY, 99.5, 20});

    for (const auto& order : orders) {
        printOrder(order);
    }

    std::cout << "Learning map:" << std::endl;
    learnMap();
    

    return 0;

}