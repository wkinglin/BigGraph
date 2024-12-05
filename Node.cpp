#include <iostream>
#include "Node.h"

// 不同节点的列表，格式为id2Node
std::unordered_map<std::string, Node> PersonMap;
std::unordered_map<std::string, Node> CityMap;
std::unordered_map<std::string, Node> CompanyMap;
std::unordered_map<std::string, Node> UniversityMap;
std::unordered_map<std::string, Node> CountryMap;
std::unordered_map<std::string, Node> PostMap;
std::unordered_map<std::string, Node> CommentMap;
std::unordered_map<std::string, Node> MessageMap;

Node::Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value) {
    this->label_string = label_string;

}

Node::Node(unsigned node_id) {
    this->node_id_ = node_id;
}

GPStore::Value* Node::operator[](const std::string& property_string) {

}

void Node::GetLinkedNodes(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, unsigned& list_len, char edge_dir) {

}

void Node::GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                       unsigned& prop_len, unsigned& list_len, char edge_dir) {

}

void Node::setLabel(const std::string &label_string) {
    this->label_string = label_string;
}

void Node::setValues(const std::string &prop_string, const GPStore::Value *value) {
    this->columns[prop_string] = *value;
}

void Node::print() {
    std::cout << "Node_id: " << this->node_id_ << "\n";
    std::cout << "label_string: " << this->label_string << "\n";
    std::cout << "columns_size: " << this->columns.size() << "\n";
    for(auto& item:this->columns)
        std::cout << item.first << ": " << item.second.toString() << "\n";
    std::cout << "relations_size: " << this->relations.size() << "\n";
    for(auto& item:this->relations)
        std::cout << item.first << ": " << item.second.toString() << "\n";
}
