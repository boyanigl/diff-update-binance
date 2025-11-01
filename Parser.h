#ifndef PARSER_H
#define PARSER_H

std::vector<Order<double>> parsePriceArray(const std::string& s, const std::string& key);
void convertVectorToUint64(const std::vector<Order<double>>& vecInp, std::vector<Order<uint64>>& vecOutput );

#endif