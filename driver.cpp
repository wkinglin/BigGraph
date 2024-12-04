#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stack>
#include "PProcedure.h"
using namespace std;

void printResults(vector<vector<GPStore::Value>> &result) {
    if (result.empty()) {
        cout << "Empty result" << endl;
        return;
    }
    for (const auto &row : result) {
        size_t rowLen = row.size();
        for (size_t i = 0; i < rowLen; i++) {
            const auto &val = row[i];
            cout << val.toString();
            if (i != rowLen - 1)
                cout << "|";
        }
        cout << endl;
    }
}

std::string trim(const std::string& str, const std::string& whitespace = " \t\n\r") {
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return ""; // No content
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::vector<std::string> splitList(const std::string& str) {
	std::vector<std::string> tokens;
	std::string cur = "";
	stack<char> st;
	for (int i = 0; i < str.size(); ++i) {
		if (str[i] == '[') {
			st.push('[');
			cur += str[i];
		} else if (str[i] == ']') {
			st.pop();
			cur += str[i];
			if (st.empty()) {
				tokens.push_back(cur);
				cur = "";
			}
		} else if (str[i] == ',' || str[i] == ' ') {
			if (st.empty()) {
				cur = "";
			} else {
				cur += str[i];
			}
		} else {
			cur += str[i];
		}
	}
	return tokens;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

bool compareResults(std::vector<std::vector<GPStore::Value>> &result, std::vector<std::vector<std::string>> &trueResults) {
    if (result.size() != trueResults.size()) {
        cout << "Mismatch in number of rows" << endl;
        return false;
    }
    for (int i = 0; i < result.size(); ++i) {
        if (result[i].size() != trueResults[i].size()) {
            cout << "Mismatch in number of columns" << endl;
            return false;
        }
        for (int j = 0; j < result[i].size(); ++j) {
            if (result[i][j].getType() != GPStore::Value::LIST) {
                if (result[i][j].toString() != trueResults[i][j]) {
                    cout << "Mismatch in value" << endl;
                    return false;
                }
            } else {
                // Parse the trueResults into a vector<string>
                string curTrue = trueResults[i][j].substr(1, trueResults[i][j].size() - 2);	// get rid of outermost brackets
				vector<string> trueList;
				if (curTrue[0] == '[')
					trueList = splitList(curTrue);
				else
					trueList = split(curTrue, ',');
                size_t genListLen = 0;
                std::vector<GPStore::Value *> *genListVal = result[i][j].data_.List;
                for (int k = 0; k < genListVal->size(); ++k) {
                    string curVal = (*genListVal)[k]->toString();
                    if (find(trueList.begin(), trueList.end(), curVal) == trueList.end()) {
                        cout << "Mismatch in list value" << endl;
                        return false;
                    }
                    genListLen++;
                }
                if (genListLen != trueList.size()) {
                    cout << "Mismatch in list length" << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <sf in {0.1, 3}>" << endl;
        return 1;
    }
    string sf = argv[1];
    if (sf != "0.1" && sf != "3") {
        cout << "Usage: " << argv[0] << " <sf in {0.1, 3}>" << endl;
        return 1;
    }

    // [FILL HERE] Load the dataset according to the scale factor

    // 基本路径
    string separator;
    bool isWindows = false;
    #ifdef _WIN32
        isWindows = true;
    #else
        isWindows = false;
    #endif

    if(isWindows) separator = "\\";
    else separator = "/";

    string smallGraph = "social_network-csv_composite-longdateformatter-sf0.1" + separator + "social_network-csv_composite-longdateformatter-sf0.1";
    string bigGraph = "social_network-csv_composite-longdateformatter-sf3" + separator + "social_network-csv_composite-longdateformatter-sf3";
    if(isWindows){
        smallGraph = "..\\" + smallGraph;
        bigGraph = "..\\" + bigGraph;
    }

    string headersPath, dynamicPath, staticPath;
    string line1;
    long long nodeId = 0;
    if(sf=="0.1"){
        headersPath = smallGraph + separator + "headers";
        dynamicPath = smallGraph + separator + "dynamic";
        staticPath = smallGraph + separator + "static";
    }
    else{
        headersPath = bigGraph + separator + "headers";
        dynamicPath = bigGraph + separator + "dynamic";
        staticPath = bigGraph + separator + "static";
    }

    // 提取Person节点
    string personHeaderPath = headersPath + separator + "dynamic" + separator + "Person.csv";
    string personFilePath = dynamicPath + separator + "person_0_0.csv";
    ifstream finHeader(personHeaderPath);
    ifstream finFile(personFilePath);

    std::vector<string> props;
    std::vector<GPStore::Value> contents;

    if (!finHeader.is_open()) {
        cout << "Failed to open " << personHeaderPath << endl;
        return 1;
    }
    if (!finFile.is_open()) {
        cout << "Failed to open " << personHeaderPath << endl;
        return 1;
    }
    while (getline(finHeader, line1)) {
        props = split(line1, '|');
        //  提取的header内容
        for(auto& item:props) cout << item <<" ";
    }
    while (getline(finFile, line1)) {
        vector<string> stringContents = split(line1, '|');
        for(auto& item:stringContents) contents.emplace_back(item);
        if(props.size()!=contents.size()){
            cout << "Props and contents size not equal.";
            return 1;
        }
        Node node(nodeId++);
        for(int i = 0; i < props.size(); i++){
            node.setLabel("Person");
            node.setValues(props[i],&contents[i]);
        }

    }



    // Repeatedly read test cases from stdin
    string line;
    while (getline(cin, line)) {
        if (line == "exit")
            break;
        if (line == "builtin_test") {
            // Read test cases from file and compare the results with the ground truth
            string groundTruthDir = "ground_truth/";
            vector<string> procs = {"ic1", "ic2", "is1"};
            for (const string &proc : procs) {
                string groundTruthFile = groundTruthDir + proc + "-sf" + sf + ".txt";
                ifstream fin(groundTruthFile);
                if (!fin.is_open()) {
                    cout << "Failed to open " << groundTruthFile << endl;
                    return 1;
                }
                string line;
                while (getline(fin, line)) {
                    cout << proc << " " << line << endl;
                    vector<GPStore::Value> args;
                    vector<vector<GPStore::Value>> result;
                    if (proc == "ic1") {
                        size_t pos = line.find(" ");
                        string personId_str = line.substr(0, pos);
                        string firstName = line.substr(pos + 1);
                        long long personId = stoll(personId_str);
                        args.emplace_back(personId);
                        args.emplace_back(firstName);
                        // ic1(args, result);
                    } else if (proc == "ic2") {
                        size_t pos = line.find(" ");
                        string personId_str = line.substr(0, pos);
                        string datetime_str = line.substr(pos + 1);
                        long long personId = stoll(personId_str);
                        long long datetime = stoll(datetime_str);
                        args.emplace_back(personId);
                        args.emplace_back(datetime);
                        ic2(args, result);
                    } else if (proc == "is1") {
                        long long personId = stoll(line);
                        args.emplace_back(personId);
                        is1(args, result);
                    }
                    printResults(result);
                    getline(fin, line);
                    int numRows = stoi(line);
                    vector<vector<string>> trueResults;
                    for (int i = 0; i < numRows; ++i) {
                        getline(fin, line);
                        trueResults.emplace_back(split(line, '|'));
                    }
                    // Compare results
                    if (compareResults(result, trueResults))
                        cout << "Test passed" << endl;
                }
            }
        } else {
            size_t pos = line.find(" ");
            string proc = line.substr(0, pos);
            vector<GPStore::Value> args;
            vector<vector<GPStore::Value>> result;
            if (proc == "ic1") {
                size_t next_pos = line.find(" ", pos + 1);
                if (next_pos == string::npos) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                string personId_str = line.substr(pos + 1, next_pos - pos - 1);
                string firstName = line.substr(next_pos + 1);
                long long personId = -1;
                try {
                    personId = stoll(personId_str);
                } catch (invalid_argument &e) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                args.emplace_back(personId);
                args.emplace_back(firstName);
                ic1(args, result);
            } else if (proc == "ic2") {
                size_t next_pos = line.find(" ", pos + 1);
                if (next_pos == string::npos) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                string personId_str = line.substr(pos + 1, next_pos - pos - 1);
                string datetime_str = line.substr(next_pos + 1);
                long long personId = -1, datetime = -1;
                try {
                    personId = stoll(personId_str);
                } catch (invalid_argument &e) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                try {
                    datetime = stoll(datetime_str);
                } catch (invalid_argument &e) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                args.emplace_back(personId);
                args.emplace_back(datetime);
                ic2(args, result);
            } else if (proc == "is1") {
                string personId_str = line.substr(pos + 1);
                long long personId = -1;
                try {
                    personId = stoll(personId_str);
                } catch (invalid_argument &e) {
                    cout << "Invalid input" << endl;
                    continue;
                }
                args.emplace_back(personId);
                is1(args, result);
            }
            printResults(result);
        }
    }
    return 0;
}