#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
using namespace std;
using namespace std::chrono;
#include "Order.h"
#include "Parser.h"
#include "OrderBook.h"




// ---------- TEST 1: Basic tests of updating and testing boundaries ----------
void test_basicInsert() {
    Orders<Order<uint64>> bids;
    bids.updateOrder({100, 1});
    bids.updateOrder({200, 2});
    bids.updateOrder({50, 3});
    bids.updateOrder({50000001, 2});
    bids.updateOrder({(uint64)18446744073709551615u,2}); //largest uint64 number

    cout << "[TEST 1] AVL insertion structure:\n";
    bids.printOrders();

    assert((uint64)18446744073709551615u == bids.getMaxPrice());
    assert(50 == bids.getMinPrice());
    cout << "test_basicInsert passed\n";
}

// ---------- TEST 2: Quantity updates ----------
void test_updateQuantity() {
    Orders<Order<uint64>> bids;
    bids.updateOrder({100, 1});
    bids.updateOrder({100, 5}); // same price, should update quantity

    cout << "[TEST 2] Quantity update:\n";
    

    assert(bids.getMaxPrice() == 100);
    bids.updateOrder({100, 0}); // deletion of order

    assert(bids.getMaxPrice() == 0); //expecting to be empty now
    bids.printOrders();
    cout << "test_updateQuantity passed\n";
}


// ---------- TEST 3: OrderBook + Spread ----------
void test_orderBookSpread() {
    vector<Order<double>> bidOrders = { {100, 1}, {95, 2} };
    vector<Order<double>> askOrders = { {110, 1}, {120, 1} };

    OrderBook<double> book(bidOrders, askOrders);

    cout << "[TEST 3] OrderBook spread test:\n";
    book.printOrderBook();

    assert(book.getBestBid() == 100);
    assert(book.getBestAsk() == 110);
    assert(book.getSpread() == 10);
    cout << "test_orderBookSpread passed\n";
}

// ---------- TEST 4: Parser snapshot ----------
void test_parserSnapshot() {
    string snapshot = R"({
      "bids" : [ ["0.0024","14.70000000"], ["0.0022","6.40000000"], ["0.0020","9.70000000"] ],
      "asks" : [ ["0.0026","3.60000000"], ["0.0028","1.00000000"] ]
    })";
    vector<Order<uint64>> bids, asks;
    convertVectorToUint64(parsePriceArray(snapshot, "\"bids\""), bids);
    convertVectorToUint64(parsePriceArray(snapshot, "\"asks\""), asks);


    cout << "[TEST 4] Parser snapshot:\n";
    for (Order<uint64> &o : bids) cout << "Bid Price: " << o.price / 1e8<< ", Qty: " << o.quantity / 1e8 << endl;
    for (Order<uint64> &o : asks) cout << "Ask Price: " << o.price / 1e8<< ", Qty: " << o.quantity / 1e8<< endl;

    assert(bids.size() == 3);
    assert(asks.size() == 2);
    cout << "test_parserSnapshot passed\n";
}

// ---------- TEST 5: Initialization of snapshot + updates ----------
void test_orderBookUpdate() {
    string snapshot = R"({
      "bids" : [ ["0.0024","14.70000000"], ["0.0022","6.40000000"] ],
      "asks" : [ ["0.0026","3.60000000"], ["0.0028","1.00000000"] ]
    })";

    vector<Order<uint64>> bids, asks;
    convertVectorToUint64(parsePriceArray(snapshot, "\"bids\""), bids);
    convertVectorToUint64(parsePriceArray(snapshot, "\"asks\""), asks);

    /* Initializing with snapshot extracted info*/
    OrderBook<uint64> book(bids, asks);

    // Apply update
    string update = R"({
      "b": [ ["0.0024","0"], ["0.0020","9.7"] ],
      "a": [ ["0.0025","2.0"] ]
    })";

    vector<Order<uint64>> bidUpdates, askUpdates;
    convertVectorToUint64(parsePriceArray(update, "\"b\""), bidUpdates);
    convertVectorToUint64(parsePriceArray(update, "\"a\""), askUpdates);

    book.updateBook(bidUpdates, askUpdates);

    cout << "[TEST 5] After update:\n";
    book.printOrderBook();

    assert(book.getBestBid() == 0.0022 * 1e8); // 0.0024 removed, 0.0022 remains
    assert(book.getBestAsk() == 0.0025 * 1e8);
    cout << "test_orderBookUpdate passed\n";
}

void runUnitTests(){
    cout << "\n========== UNIT TEST SUITE ==========\n";
    test_basicInsert();
    test_updateQuantity();
    test_orderBookSpread();
    test_parserSnapshot();
    test_orderBookUpdate();

    cout << "\n All tests passed successfully!\n";
}

uint64 memoryEstimate(size_t numOrders) {
    // Rough estimate: For each order, we have: 2 pointers to the children, height bytes and Order data
    uint64 bytes = numOrders * (sizeof(Order<uint64>) + sizeof(Orders<Order<uint64>>) + sizeof(void*) + sizeof(unsigned short));
    return bytes;
}

void benchmark_insert(unsigned int N) {
    Orders<Order<uint64>> book;

    vector<Order<uint64>> orders;
    for (unsigned int i = 0; i < N; ++i)
        orders.push_back({1000 + i, (10001 % i) * (i % 20 )});

    auto start = high_resolution_clock::now();
    for (Order<uint64> &o : orders)
        book.updateOrder(o);
    auto end = high_resolution_clock::now();

    double ms = duration<double, milli>(end - start).count();
    cout << "[BENCHMARK] Insert " << N << " orders: "
         << fixed << setprecision(2) << ms << " ms \n";

    cout << "Memory estimate: ~" << memoryEstimate(N) / 1024.0 << " KB\n";
}

void benchmark_update(unsigned int N) {
    Orders<Order<uint64>> book;
    vector<Order<uint64>> orders;
    for (unsigned int i = 0; i < N; ++i)
        orders.push_back({1000 + i, (10001 % i) * (i % 20 )});

    auto start = high_resolution_clock::now();
    for (Order<uint64> &order : orders)
        book.updateOrder(order); // update with different pairies
    auto end = high_resolution_clock::now();

    double ms = duration<double, milli>(end - start).count();
    cout << "[BENCHMARK] Update " << N << " orders: "
         << fixed << setprecision(2) << ms << " ms \n";
}

void benchmark_delete(unsigned int N) {
    Orders<Order<uint64>> book;
    for (unsigned int i = 0; i < N; ++i)
        book.updateOrder({1000 + i, (10001 % i) * (i % 20 )});

    auto start = high_resolution_clock::now();
    for (unsigned int i = 0; i < N; ++i)
        book.updateOrder({1000 + i, 0});
    auto end = high_resolution_clock::now();

    double ms = duration<double, milli>(end - start).count();
    cout << "[BENCHMARK] Remove " << N << " orders: "
         << fixed << setprecision(2) << ms << " ms \n";
}


void runBenchmarks() {
    cout << "\n========== BENCHMARK SUITE ==========\n";
    for (size_t N : {100, 1000, 10000, 100000}) {
        cout << "\n--- N = " << N << " ---\n";
        benchmark_insert(N);
        benchmark_update(N);
        benchmark_delete(N);

    }
    cout << "\n✅ Benchmarks completed.\n";
}
// ---------- MAIN TEST RUNNER ----------
int main() {
    
    runUnitTests();



    runBenchmarks();
    return 0;
}
