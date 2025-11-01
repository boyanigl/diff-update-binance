#ifndef ORDER_BOOOK_H
#define ORDER_BOOOK_H

template <typename T>
class OrderBook {
	private:
		Orders<Order<T>> bids;
		Orders<Order<T>> asks;
	public:
		OrderBook() : bids(), asks() {}
		OrderBook(const std::vector<Order<T>>& bidOrders, const std::vector<Order<T>>& askOrders) : bids(bidOrders), asks(askOrders) {}
		~OrderBook() {}
		void printOrderBook() const {
			std::cout << "Bids:\n";
			bids.printOrders();
			std::cout << "Asks:\n";
			asks.printOrders();
		}
		T getBestBid() const {
			return bids.getMaxPrice();
		}

		T getBestAsk() const {
			return asks.getMinPrice();
		}

		T getSpread() const {
			return asks.getMinPrice() - bids.getMaxPrice();
		}

		void updateBook(const std::vector<Order<T>>& bidUpdates, const std::vector<Order<T>>& askUpdates) {
			for (const Order<T>& bid : bidUpdates) {
				bids.updateOrder(bid);
			}
			for (const Order<T>& ask : askUpdates) {
				asks.updateOrder(ask);
			}
		}
};

#endif