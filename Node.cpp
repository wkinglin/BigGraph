
#include "Node.h"

// 不同节点的列表，格式为id2Node
std::unordered_map<std::string, Node> PersonMap;
std::unordered_map<std::string, Node> CityMap;
std::unordered_map<std::string, Node> CompanyMap;
std::unordered_map<std::string, Node> UniversityMap;
std::unordered_map<std::string, Node> CountryMap;
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