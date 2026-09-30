#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <optional>

long long nextTimeStamp = 1;

enum OrderStatus {
    OPEN,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

enum class Side {
    BUY,
    SELL
};

struct Order {
    int id;
    Side side;
    double price;
    int quantity;
    OrderStatus status;
    long long timeStamp;
};

struct Trade {
    int buyOrderId;
    int sellOrderId;
    double price;
    int quantity;
};

std::string getOrderStatusName(OrderStatus status) {
    switch(status) {
        case OrderStatus::OPEN:
            return "OPEN";
        case OrderStatus::PARTIALLY_FILLED:
            return "PARTIALLY_FILLED";
        case OrderStatus::FILLED:
            return "FILLED";
        case OrderStatus::CANCELLED:
            return "CANCELED";
        default:
            return "UNKNOWN";
    }
}

void printOrder(const Order& order) {
    std::cout << "Order ID: " << order.id << ", Side: " 
              << (order.side == Side::BUY ? "BUY" : "SELL")
              << ", Price : " << order.price << ", Quantity : " << order.quantity
              << ", Status : " << getOrderStatusName(order.status)
              << std::endl;
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

bool addOrder(std::map<int, Order>& orderRegistry, std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, std::map<double, std::vector<Order>>& sellOrders, const Order& order) {
    if (orderRegistry.find(order.id) != orderRegistry.end()){
        return false;
    }

    orderRegistry[order.id] = order;
    if (order.side == Side::BUY) {
        addBuyOrder(buyOrders, order);
    } else {
        addSellOrder(sellOrders, order);
    }

    return true;
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

void matchOrders(std::map<int, Order>& orderRegistry, std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, std::map<double, std::vector<Order>>& sellOrders, std::vector<Trade>& trades) {
    if (buyOrders.empty() || sellOrders.empty()) {
        std::cout << "Orders do not match" << std::endl;
        return;
    }

    while (!buyOrders.empty() && !sellOrders.empty()){
        auto buyLevel = buyOrders.begin();
        auto sellLevel = sellOrders.begin();

        double bestBuyPrice = buyLevel->first;
        double bestSellPrice = sellLevel->first;

        if (bestBuyPrice < bestSellPrice){
            break;
        }
        
        std::cout << "Orders Match" << std::endl;
        std::cout << "Best Buy Price  : " << bestBuyPrice << std::endl;
        std::cout << "Best Sell Price : " << bestSellPrice << std::endl;

        auto& bestBuyOrders = buyLevel->second;
        auto& bestSellOrders = sellLevel->second;

        auto& bestBuyOrder = bestBuyOrders.front();
        auto& bestSellOrder = bestSellOrders.front();

        std::cout << "Best Buy Order" << std::endl;
        printOrder(bestBuyOrder);

        std::cout << "Best Sell Order" << std::endl;
        printOrder(bestSellOrder);

        int tradeQuantity = std::min(bestBuyOrder.quantity, bestSellOrder.quantity);

        std::cout << "Trade executed" << std::endl;
        std::cout << "Trade Price    : " << bestSellPrice << std::endl;
        std::cout << "Trade Quantity : " << tradeQuantity << std::endl;

        bestBuyOrder.quantity -= tradeQuantity;
        bestSellOrder.quantity -= tradeQuantity;
        orderRegistry[bestBuyOrder.id].quantity -= tradeQuantity;
        orderRegistry[bestSellOrder.id].quantity -= tradeQuantity;

        trades.push_back({bestBuyOrder.id, bestSellOrder.id, bestSellPrice, tradeQuantity});


        if (bestBuyOrder.quantity == 0){
            bestBuyOrder.status = OrderStatus::FILLED;
            orderRegistry[bestBuyOrder.id].status = OrderStatus::FILLED;
            bestBuyOrders.erase(bestBuyOrders.begin());
        } else {
            orderRegistry[bestBuyOrder.id].status = OrderStatus::PARTIALLY_FILLED;
            bestBuyOrder.status = OrderStatus::PARTIALLY_FILLED;
        }
        if (bestBuyOrders.empty()){
            buyOrders.erase(buyLevel);
        }
        if (bestSellOrder.quantity == 0){
            bestSellOrder.status = OrderStatus::FILLED;
            orderRegistry[bestSellOrder.id].status = OrderStatus::FILLED;
            bestSellOrders.erase(bestSellOrders.begin());
        } else {
            orderRegistry[bestSellOrder.id].status = OrderStatus::PARTIALLY_FILLED;
            bestSellOrder.status = OrderStatus::PARTIALLY_FILLED;
        }
        if (bestSellOrders.empty()){
            sellOrders.erase(sellLevel);
        }  
    }
    
    if (buyOrders.empty() || sellOrders.empty()){
        std::cout << "One side of the book is empty" << std::endl;
    } else {
        std::cout << "Best BUY price is lower than best SELL price" << std::endl;
    }
}

bool cancelBuyOrder(std::map<int, Order>& orderRegistry, std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, int orderId) {
    bool returnValue = false;
    for(auto buyLevelIt = buyOrders.begin(); buyLevelIt != buyOrders.end(); ){
        for(auto it = buyLevelIt->second.begin(); it != buyLevelIt->second.end();) {
            if (it->id == orderId){
                orderRegistry[orderId].status = OrderStatus::CANCELLED;
                it->status = OrderStatus::CANCELLED;
                returnValue = true;
                it = buyLevelIt->second.erase(it);
            } else {
                ++it;
            }
        }
        if (buyLevelIt->second.empty()){
            buyLevelIt = buyOrders.erase(buyLevelIt);
        }
        else{
            ++buyLevelIt;
        }

        if (returnValue)
            return true;
    }
    return false;
}

bool cancelSellOrder(std::map<int, Order>& orderRegistry, std::map<double, std::vector<Order>>& sellOrders, int orderId){
    bool returnValue = false;
    for(auto sellLevelIt = sellOrders.begin(); sellLevelIt != sellOrders.end(); ){
        for(auto it = sellLevelIt->second.begin(); it != sellLevelIt->second.end(); ){
            if (it->id == orderId){
                orderRegistry[orderId].status = OrderStatus::CANCELLED;
                it->status = OrderStatus::CANCELLED;
                returnValue = true;
                it = sellLevelIt->second.erase(it);
            } else {
                ++it;
            }
        }
        if (sellLevelIt->second.empty()){
            sellLevelIt = sellOrders.erase(sellLevelIt);
        } else {
            ++sellLevelIt;
        }

        if (returnValue)
            return true;
    }

    return false;
}

bool cancelOrder(std::map<int, Order>& orderRegistry, std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, std::map<double, std::vector<Order>>& sellOrders, int orderId) {
    auto it = orderRegistry.find(orderId);
    if (it == orderRegistry.end()){
        return false;
    }
    if ((it->second.status == OrderStatus::CANCELLED) || (it->second.status == OrderStatus::FILLED)){
        return false;
    }
    return (cancelBuyOrder(orderRegistry, buyOrders, orderId) || cancelSellOrder(orderRegistry,sellOrders, orderId));
}

std::optional<double> getBestBid(const std::map<double, std::vector<Order>, std::greater<double>>& buyOrders){
    if (buyOrders.empty()){
        return std::nullopt;
    }
    return buyOrders.begin()->first;
}

std::optional<double> getBestAsk(const std::map<double, std::vector<Order>>& sellOrders) {
    if (sellOrders.empty()){
        return std::nullopt;
    }
    return sellOrders.begin()->first;
}

std::optional<double> getSpread(const std::map<double, std::vector<Order>, std::greater<double>>& buyOrders, const std::map<double, std::vector<Order>>& sellOrders){
    std::optional<double> bestBid = getBestBid(buyOrders);
    std::optional<double> bestAsk = getBestAsk(sellOrders);

    if (bestBid.has_value() && bestAsk.has_value())
        return bestAsk.value() - bestBid.value();
    else
        return std::nullopt;
}

void printTrades(const std::vector<Trade>& trades) {
    std::cout << "Trade History" << std::endl;

    for(const auto& trade : trades) {
        std::cout <<"BUY Order : " << trade.buyOrderId
                  <<", SELL Order : " << trade.sellOrderId
                  <<", Price : " << trade.price
                  <<", Quantity : " << trade.quantity << std::endl;
    }
}

void printOrderRegistry(const std::map<int, Order>& OrderRegistry){
    for(const auto& entry : OrderRegistry) {
        printOrder(entry.second);
    }
}

std::optional<OrderStatus> getOrderStatus(const std::map<int, Order>& orderRegsitry, int orderId) {
    auto it = orderRegsitry.find(orderId);

    if (it == orderRegsitry.end()) {
        return std::nullopt;
    }

    return it->second.status;
}

std::optional<Order> getOrder(const std::map<int, Order>& orderRegistry, int orderId) {
    auto it = orderRegistry.find(orderId);

    if (it == orderRegistry.end()){
        return std::nullopt;
    }

    return it->second;
}

int main() {
    // std::vector<Order> orders;
    // addOrder(orders, {1, Side::BUY, 100.5, 10});
    // addOrder(orders, {2, Side::SELL, 101.0, 5});
    // addOrder(orders, {3, Side::BUY, 99.5, 20});

    // for (const auto& order : orders) {
    //     printOrder(order);
    // }

    std::map<int, Order> orderRegistry;
    std::map<double, std::vector<Order>, std::greater<double>> buyOrders;
    std::map<double, std::vector<Order>> sellOrders;

    addOrder(orderRegistry, buyOrders, sellOrders, {1, Side::BUY, 100.5, 10, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {3, Side::BUY, 99.5, 20, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {4, Side::BUY, 100.5, 15, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {5, Side::BUY, 101.0, 10, OrderStatus::OPEN});

    addOrder(orderRegistry, buyOrders, sellOrders, {2, Side::SELL, 101.5, 10, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {6, Side::SELL, 99, 10, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {7, Side::SELL, 100, 30, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {8, Side::SELL, 99, 3, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {9, Side::SELL, 100, 4, OrderStatus::OPEN});
    addOrder(orderRegistry, buyOrders, sellOrders, {10, Side::SELL, 101, 10, OrderStatus::OPEN});

    

    // addBuyOrder(buyOrders, {1, Side::BUY, 100.5, 10, OrderStatus::OPEN});
    // addBuyOrder(buyOrders, {3, Side::BUY, 99.5, 20, OrderStatus::OPEN});
    // addBuyOrder(buyOrders, {4, Side::BUY, 100.5, 15, OrderStatus::OPEN});
    // addBuyOrder(buyOrders, {5, Side::BUY, 101.0, 10, OrderStatus::OPEN});

    std::cout << "***************** BUY ORDERS *****************" << std::endl;
    printBuyBook(buyOrders);

    
    
    // addSellOrder(sellOrders, {2, Side::SELL, 101.5, 10, OrderStatus::OPEN});
    // addSellOrder(sellOrders, {6, Side::SELL, 99, 10, OrderStatus::OPEN});
    // addSellOrder(sellOrders, {7, Side::SELL, 100, 30, OrderStatus::OPEN});
    // addSellOrder(sellOrders, {8, Side::SELL, 99, 3, OrderStatus::OPEN});
    // addSellOrder(sellOrders, {9, Side::SELL, 100, 4, OrderStatus::OPEN});
    // addSellOrder(sellOrders, {10, Side::SELL, 101, 10, OrderStatus::OPEN});


    std::cout << "***************** SELL ORDERS *****************" << std::endl;
    printSellBook(sellOrders);

    std::cout << "***************** CANCEL ORDERS *****************" << std::endl;
    if (cancelOrder(orderRegistry, buyOrders, sellOrders, 11)){
        std::cout << "Order cancelled successfully" << std::endl;
    } else {
        std::cout << "Order is filled, already cancelled or not found" << std::endl;
    }

    if (cancelOrder(orderRegistry, buyOrders, sellOrders, 3)){
        std::cout << "Order cancelled successfully" << std::endl;
    } else {
        std::cout << "Order is filled, already, cancelled or not found" << std::endl;
    }

    auto tempOrder = getOrder(orderRegistry, 3);
    if (tempOrder){
        printOrder(*tempOrder);
    }


    std::cout << "***************** BEST BID/ASK SPREAD *****************" << std::endl;
    std::optional<double> bestBid = getBestBid(buyOrders);
    std::optional<double> bestAsk = getBestAsk(sellOrders);
    std::optional<double> spread = getSpread(buyOrders, sellOrders);
    if (bestBid.has_value())
        std::cout << "Best Bid : " << bestBid.value() << std::endl;
    else
        std::cout << "There is no Best Bid" << std::endl;

    if (bestAsk.has_value())
        std::cout << "Best Ask : " << bestAsk.value() << std::endl;
    else
        std::cout << "There is no Best Ask" << std::endl;

    if (spread.has_value())
        std::cout << "Spread : " << spread.value() << std::endl;
    else
        std::cout << "There is no spread" << std::endl;


    std::vector<Trade> trades;

    std::cout << "***************** MATCH ORDERS *****************" << std::endl;
    matchOrders(orderRegistry, buyOrders, sellOrders, trades);

    std::cout << "***************** TRADE HISTORY *****************" << std::endl;
    printTrades(trades);

    std::cout << "***************** ORDER REGISTRY *****************" << std::endl;
    printOrderRegistry(orderRegistry);

    std::cout << "***************** ORDER STATUS *****************" << std::endl;
    auto status = getOrderStatus(orderRegistry, 7);

    if (status.has_value()) {
        std::cout << "Order 7 Status : " << getOrderStatusName(status.value()) << std::endl;
    } else {
        std::cout << "Order not found" << std::endl;
    }

    std::cout << "***************** GET ORDER *****************" << std::endl;
    auto order1 = getOrder(orderRegistry, 7);

    if (order1.has_value()){
        printOrder(order1.value());
    } else {
        std::cout << "Order not found" << std::endl;
    }

    auto order2 = getOrder(orderRegistry, 100);

    if (order2.has_value()){
        printOrder(order2.value());
    } else {
        std::cout << "Order not found" << std::endl;
    }

    std::cout << "***************** DUPLICATE ORDER *****************" << std::endl;
    bool added = addOrder(orderRegistry, buyOrders, sellOrders, {1, Side::BUY, 102.0, 5, OrderStatus::OPEN});
    
    if (added) {
        std::cout << "Order addded successfully" << std::endl;
    } else {
        std::cout << "Order ID already exists" << std::endl;
    }


    return 0;

}