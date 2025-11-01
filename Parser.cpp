#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cctype>
using namespace std;

#include "Order.h"
#include "Parser.h"

// Very minimal parser for arrays like [["0.0024","10"],["0.0022","8"]]
std::vector<Order<double>> parsePriceArray(const std::string& s, const std::string& key) {
    std::vector<Order<double>> result;

    //Get position of the key - e.g. "bids" or "asks"
    size_t keyPos = s.find(key);
    if (keyPos == std::string::npos) return result;

    // Find the start of the array for this key
    size_t startArray = s.find('[', keyPos);
    if (startArray == std::string::npos) return result;

    // Find matching closing bracket for this array
    int bracketCount = 0;
    size_t endArray = startArray;
    for (; endArray < s.size(); ++endArray) {
        if (s[endArray] == '[') bracketCount++;
        else if (s[endArray] == ']') bracketCount--;
        if (bracketCount == 0) break;
    }

    // Parse all ["price","qty"] pairs inside
    size_t pos = startArray;
    while (pos < endArray) {
        size_t open = s.find('[', pos + 1);
        if (open == std::string::npos || open > endArray) break;
        size_t close = s.find(']', open);
        if (close == std::string::npos || close > endArray) break;

        std::string entry = s.substr(open + 1, close - open - 1);
        size_t comma = entry.find(',');
        if (comma != std::string::npos) {

            std::string priceStr = entry.substr(0, comma);
            std::string qtyStr   = entry.substr(comma + 1);

            priceStr.erase(remove(priceStr.begin(), priceStr.end(), '\"'), priceStr.end());
            priceStr.erase(remove_if(priceStr.begin(), priceStr.end(), ::isspace), priceStr.end());

            qtyStr.erase(remove(qtyStr.begin(), qtyStr.end(), '\"'), qtyStr.end());
            qtyStr.erase(remove_if(qtyStr.begin(), qtyStr.end(), ::isspace), qtyStr.end());


            double price = std::stod(priceStr);
            double qty   = std::stod(qtyStr);
            result.push_back({price, qty});

        }

        pos = close;
    }

    return result;
}

void convertVectorToUint64(const std::vector<Order<double>>& vecInp, std::vector<Order<uint64>>& vecOutput ){

    for (const Order<double>& order : vecInp){
        uint64 price = static_cast<uint64>(std::round(order.price * 1e8));
        uint64 quantity = static_cast<uint64>(std::round(order.quantity * 1e8));
        vecOutput.push_back({price, quantity});
    }

    return;
}
