#ifndef ORDER_BOOOK_H
#define ORDER_BOOOK_H

template <typename PriceQtyType>
class OrderBook {
	private:
		Orders<Order<PriceQtyType>> bids;
		Orders<Order<PriceQtyType>> asks;
	public:
		OrderBook() : bids(), asks() {}
		OrderBook(const std::vector<Order<PriceQtyType>>& bidOrders, const std::vector<Order<PriceQtyType>>& askOrders) : bids(bidOrders), asks(askOrders) {}
		OrderBook(const OrderBook&) = delete;
    	OrderBook& operator=(const OrderBook&) = delete;
		~OrderBook() {}
		void printOrderBook() const {
			std::cout << "Bids:\n";
			bids.printOrders();
			std::cout << "Asks:\n";
			asks.printOrders();
		}
		PriceQtyType getBestBid() const {
			return bids.getMaxPrice();
		}

		PriceQtyType getBestAsk() const {
			return asks.getMinPrice();
		}

		PriceQtyType getSpread() const {
			return asks.getMinPrice() - bids.getMaxPrice();
		}

		void updateBook(const std::vector<Order<PriceQtyType>>& bidUpdates, const std::vector<Order<PriceQtyType>>& askUpdates) {
			for (const Order<PriceQtyType>& bid : bidUpdates) {
				bids.updateOrder(bid);
			}
			for (const Order<PriceQtyType>& ask : askUpdates) {
				asks.updateOrder(ask);
			}
		}
};

#endif