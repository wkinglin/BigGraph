#include <string>
#include <memory>
#include "Value.h"
#include "unordered_map"

class Node {
 public:
  unsigned node_id_; //每个node要维护一个新的id
  std::string label_string; //node的Type
  std::unordered_map<std::string,GPStore::Value> columns;  //node的属性
  std::unordered_map<std::string,GPStore::Value> relation;  //node与其他node的关系

  Node()=default;
  Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value);
  Node(unsigned node_id);
  void Goto(unsigned new_node_id) {
    node_id_ = new_node_id;
  }
  GPStore::Value* operator[](const std::string& property_string);
  void GetLinkedNodes(const std::string&, std::shared_ptr<const unsigned[]>& nodes_list, unsigned& list_len, char edge_dir);
  void GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                   unsigned& prop_len, unsigned& list_len, char edge_dir);

  void setLabel(const std::string& label_string);
  void setValues(const std::string& prop_string, const GPStore::Value* value);
};

