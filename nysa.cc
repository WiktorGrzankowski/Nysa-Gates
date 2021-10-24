#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <regex>

using std::cout;
using std::cin;
using std::cerr;

#define UPPER_VALUE_BOUND 999999999

void printSignalsValue(std::map<u_int32_t, std::pair<int, bool>>* signalMap) {
    for (auto& elem : *signalMap) {
        cout << elem.second.first;
        elem.second.first = -1;
    }
    cout << "\n";
}

void AND_NAND_operator(std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                       std::map<u_int32_t, std::pair<int, bool>>::iterator*
                       elem, std::vector<u_int32_t> &signals, std::string &op) {
    int value = 1;

    for (u_int32_t& signal : signals)
        value = value && signalMap->at(signal).first;

    if (op == "AND")
        (*elem)->second.first = value;
    else
        (*elem)->second.first = !value;
}

void NOT_operator(std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                  std::map<u_int32_t, std::pair<int, bool>>::iterator* elem,
                  u_int32_t key) {
    (*elem)->second.first = !signalMap->at(key).first;
}

void XOR_operator(std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                  std::map<u_int32_t, std::pair<int, bool>>::iterator* elem,
                  u_int32_t key1, u_int32_t key2) {
    if (signalMap->at(key1).first != signalMap->at(key2).first)
        (*elem)->second.first = 1;
    else
        (*elem)->second.first = 0;
}

void OR_NOR_operator(std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                     std::map<u_int32_t, std::pair<int, bool>>::iterator* elem,
                     std::vector<u_int32_t> &signals, std::string& op) {
    int value = 0;

    for (u_int32_t& signal : signals)
        value = value || signalMap->at(signal).first;

    if (op == "OR")
        (*elem)->second.first = value;
    else
        (*elem)->second.first = !value;
}

void calcSignalValueHelp(const std::unordered_map<u_int32_t,
                         std::pair<std::string, std::vector<u_int32_t>>>&
                         outSignalsMap, std::map<u_int32_t,
                         std::pair<int, bool>>* signalMap,
                         std::map<u_int32_t,
                         std::pair<int, bool>>::iterator* elem) {
    std::string op = outSignalsMap.at((*elem)->first).first;
    std::vector<u_int32_t> signals = outSignalsMap.at((*elem)->first).second;

    if (op == "AND" || op == "NAND")
        AND_NAND_operator(signalMap, elem, signals, op);
    else if (op == "NOT")
        NOT_operator(signalMap, elem, signals[0]);
    else if (op == "XOR")
        XOR_operator(signalMap, elem, signals[0], signals[1]);
    else
        OR_NOR_operator(signalMap, elem, signals, op);
}

/*
 * Calculates values of signals that are not IN signals.
 * In signals are calculated by the function calcInSignals.
 */
void calcRestSignalValue(const std::unordered_map<u_int32_t,
                         std::pair<std::string, std::vector<u_int32_t>>>&
                         outSignalsMap, std::map<u_int32_t,
                         std::pair<int, bool>>* signalMap,
                         std::vector<u_int32_t>& order) {
    for (auto elem : order) {
        auto elemSignalMap = signalMap->find(elem);
        if (elemSignalMap->second.first == -1)
            calcSignalValueHelp(outSignalsMap, signalMap, &elemSignalMap);
    }
}

/*
 * Calculates values of IN signals.
 * IN signals are signals that are dependent on any gate.
 */
void calcInSignals(std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                   size_t nextValues,
                   const std::unordered_map<u_int32_t, std::pair<std::string,
                   std::vector<u_int32_t>>>& outSignalsMap) {
    size_t value;
    // Information whether the value has already been written into signalMap.
    bool countValue = true;

    for (auto elem = signalMap->rbegin(); elem != signalMap->rend(); ++elem) {
        if (countValue) {
            if (nextValues > 0) {
                value = nextValues % 2;
                nextValues = nextValues / 2;
            } else value = 0;
        }
        if (outSignalsMap.count(elem->first) == 0) {
            elem->second.first = value;
            countValue = true;
        } else countValue = false;
    }
}

void topologicalSortHelper(const std::unordered_map<u_int32_t,
                           std::pair<std::string, std::vector<u_int32_t>>>&
                           outSignalsMap, int node, std::map<u_int32_t,
                           std::pair<int, bool>>* signalMap,
                           std::vector<u_int32_t>* order) {
    signalMap->at(node).second = false;

    for (auto elem: outSignalsMap.at(node).second) {
        if (signalMap->at(elem).second)
            topologicalSortHelper(outSignalsMap, elem, signalMap, order);
    }

    order->push_back(node);
}

void topologicalSort(const std::unordered_map<u_int32_t, std::pair<std::string,
                     std::vector<u_int32_t>>>& outSignalsMap,
                     std::map<u_int32_t, std::pair<int, bool>>* signalMap,
                     std::vector<u_int32_t>* order) {
    for (auto& elem: outSignalsMap) {
        uint32_t node = elem.first;
        if (signalMap->at(node).second)
            topologicalSortHelper(outSignalsMap, node, signalMap, order);
    }
}

/*
 * Calculates, using helper functions, values of signals for all the
 * possible combinations of IN signals.
 * Writes onto the standard output a two-dimensional array of results
 * for the scheme stored in outSignalMap and singalMap.
 */
void calcSignals(const std::unordered_map<u_int32_t, std::pair<std::string,
                 std::vector<u_int32_t>>>& outSignalsMap,
                 std::map<u_int32_t, std::pair<int, bool>>* signalMap) {
    size_t inSignalsCount = signalMap->size() - outSignalsMap.size();
    size_t numberOfCombinations = 1 << inSignalsCount;

    std::vector<u_int32_t> order;
    topologicalSort(outSignalsMap, signalMap, &order);

    for (size_t i = 0; i < numberOfCombinations; i++) {

        calcInSignals(signalMap, i, outSignalsMap);

        calcRestSignalValue(outSignalsMap, signalMap, order);

        printSignalsValue(signalMap);
    }
}

bool checkForCyclesHelper(u_int32_t key, std::map<u_int32_t,
                          std::pair<int, bool>> *signalMap,
                          const std::unordered_map<u_int32_t,
                          std::pair<std::string,
                          std::vector<u_int32_t>>>& outSignalsMap) {
    if (outSignalsMap.count(key) == 0)
        return false;

    if (!signalMap->at(key).second) { // The node has not yet been visited.
        signalMap->at(key).second = true;
        signalMap->at(key).first = 0;
        std::vector<u_int32_t> signals = outSignalsMap.at(key).second;
        for (auto signal : signals) {
            if ((!signalMap->at(signal).second
                && checkForCyclesHelper(signal, signalMap, outSignalsMap))
                || signalMap->at(signal).first == 0)
                return true;
        }
    }

    signalMap->at(key).first = -1;
    return false;
}

bool checkForCycles(std::map<u_int32_t, std::pair<int, bool>> *signalMap,
                    const std::unordered_map<u_int32_t, std::pair<std::string,
                    std::vector<u_int32_t>>>& outSignalsMap) {
    for (auto elem = outSignalsMap.begin();
         elem != outSignalsMap.end(); ++elem) {
        if (checkForCyclesHelper(elem->first, signalMap, outSignalsMap)) {
            cerr << "Error: sequential logic analysis "
                    "has not yet been implemented.\n";
            return true;
        }
    }
    return false;
}

bool checkForRepeatedOut(bool outIsOk, int out, int lineNum) {
    if (!outIsOk && out > 0 && out < UPPER_VALUE_BOUND) {
        cerr << "Error in line " << lineNum << ": " << "signal "
             << out << " is assigned to multiple outputs.\n";
        return false;
    }
    return true;
}

bool printErrorInLine(size_t lineNum, const std::string &line) {
    cerr << "Error in line " << lineNum << ": " << line << "\n";
    return false;
}

u_int32_t convertToInt(const std::string& stringNumber) {
    for (char const &c : stringNumber) {
        if (std::isdigit(c) == 0)
            return 0;
    }
    std::stringstream intValue(stringNumber);
    u_int32_t number = 0;
    intValue >> number;
    return number;
}

/*
 * Reads IN signals and saves them in signalMap.
 * Checks whether the signals are correct.
 */
bool readIn(const std::string& token, std::vector<u_int32_t>* ins,
            std::map<u_int32_t, std::pair<int, bool>>* signalMap) {
    u_int32_t convertedString = convertToInt(token);

    if (convertedString <= 0 || convertedString > UPPER_VALUE_BOUND) {
        ins->push_back(convertedString);
        return false;
    }

    if ((*signalMap).count(convertedString) == 0)
        (*signalMap)[convertedString].first = -1;

    ins->push_back(convertedString);
    return true;
}

/*
 * Checks whether the out signal is correct.
 */
bool checkOut(u_int32_t out, size_t lineNum, const std::string &line,
              const std::unordered_map<u_int32_t, std::pair<std::string,
              std::vector<u_int32_t>>>& outSignalsMap, std::map<u_int32_t,
              std::pair<int, bool>>* signalMap) {
    if (out <= 0 || out > UPPER_VALUE_BOUND) {
        return printErrorInLine(lineNum, line);
    } else if (outSignalsMap.count(out) > 0) {
        return false;
    } else {
        (*signalMap)[out].first = -1;
        return true;
    }
}

bool isLineCorrectRegexwise(const std::string &line) {
    const static std::string correctNumber = "(?:\\s+[1-9]\\d{0,8})";
    const static std::regex correctLine =
            std::regex("\\s*(?:(?:XOR" + correctNumber + "{3})|(?:NOT" +
                       correctNumber + "{2})|(?:(OR|NOR|AND|NAND)" +
                       correctNumber + "{3,}\\s*))\\s*");
    return regex_match(line, correctLine);
}

/*
 * Reads input and saves the signals in outSignalsMap.
 */
bool readLine(const std::string& line,
              std::unordered_map<u_int32_t, std::pair<std::string,
              std::vector<u_int32_t>>>* outSignalsMap, size_t lineNum,
              std::map<u_int32_t, std::pair<int, bool>>* signalMap) {
    if (!isLineCorrectRegexwise(line))
        return printErrorInLine(lineNum, line);

    std::stringstream s(line);
    std::string token, op, outS;
    s >> op; // The operator is read first.

    s >> outS; // Następnie wczytywany jest sygnał wyjściowy.
    u_int32_t out = convertToInt(outS);
    bool outIsOk = checkOut(out, lineNum, line, *outSignalsMap, signalMap);
    std::vector<u_int32_t> ins;

    bool areInsOk = true;
    while (s >> token)
        areInsOk = readIn(token, &ins, signalMap);

    if (!areInsOk)
        return printErrorInLine(lineNum, line);

    if (!checkForRepeatedOut(outIsOk, out, lineNum))
        return false;

    std::pair<std::string, std::vector<u_int32_t>> p = {op, ins};
    outSignalsMap->insert({out, p});
    return true;
}

void nysa() {
    // Key: signal number.
    // Value: pair<gate type, in signals for the gate>
    std::unordered_map<u_int32_t, std::pair<std::string,
        std::vector<u_int32_t>>> outSignalsMap;
    // Key: signal number.
    // Value: pair<gate value, has the signal been visited in toposort>
    std::map<u_int32_t, std::pair<int, bool>> signalMap;

    std::string line;
    size_t lineNum = 1;
    bool linesAreOk = true;

    while (getline(cin, line)) {
        linesAreOk = readLine(line, &outSignalsMap, lineNum, &signalMap)
                     && linesAreOk;
        lineNum++;
    }

    bool noCycles = !checkForCycles(&signalMap, outSignalsMap);

    if (linesAreOk && noCycles)
        calcSignals(outSignalsMap, &signalMap);

    outSignalsMap.clear();
    signalMap.clear();
}

int main() {
    nysa();
    return 0;
}
