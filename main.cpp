#include <iostream>
#include <vector>
#include <map>
#include <functional>


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

void addBuyOrder(std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, const Order& order) {
    if (order.side == Side::BUY) {
        buyOrders[order.price].push_back(order);
    }
}

void printBuyBook(const std::map<double, std::vector<Order>, std::greater<double>>& buyOrders) {
    std::cout << "Buy Orders:" << std::endl;
    for (const auto& entry : buyOrders) {
        double price = entry.first;
        const auto& ordersAtPrice = entry.second;
        std::cout << "Price: " << price << ", Orders: " << ordersAtPrice.size() << std::endl;
        for (const auto& order : ordersAtPrice) {
            printOrder(order);
        }
    }
}

void addSellOrder(std::map<double, std::vector<Order>>& sellOrders, const Order& order){
    if (order.side == Side::SELL) {
        sellOrders[order.price].push_back(order);
    }
}

void printSellBook(const std::map<double, std::vector<Order>>& sellOrders) {
    std::cout << "Sell Orders:" << std::endl;
    for(const auto& entry : sellOrders){
        double price = entry.first;
        const auto& orderAtPrice = entry.second;
        std::cout << "Price: " << price << ", Orders: " << orderAtPrice.size() << std::endl;
        for (const auto& order: orderAtPrice){
            printOrder(order);
        }
    }
}


int main() {
    std::vector<Order> orders;
    addOrder(orders, {1, Side::BUY, 100.5, 10});
    addOrder(orders, {2, Side::SELL, 101.0, 5});
    addOrder(orders, {3, Side::BUY, 99.5, 20});

    // for (const auto& order : orders) {
    //     printOrder(order);
    // }

    std::map<double, std::vector<Order>, std::greater<double>> buyOrders;

    addBuyOrder(buyOrders, {1, Side::BUY, 100.5, 10});
    addBuyOrder(buyOrders, {3, Side::BUY, 99.5, 20});
    addBuyOrder(buyOrders, {4, Side::BUY, 100.5, 15});
    addBuyOrder(buyOrders, {5, Side::BUY, 101.0, 5});

    printBuyBook(buyOrders);

    std::map<double, std::vector<Order>> sellOrders;
    
    addSellOrder(sellOrders, {2, Side::SELL, 101.5, 10});
    addSellOrder(sellOrders, {6, Side::SELL, 99, 10});
    addSellOrder(sellOrders, {7, Side::SELL, 100, 30});
    addSellOrder(sellOrders, {8, Side::SELL, 99, 2});

    printSellBook(sellOrders);

    return 0;

}