#include <string>
#include <memory>
#include "Value.h"
#include "unordered_map"
using namespace std;

class Node {
 public:
  unsigned node_id_; //每个node要维护一个新的id
  std::string label_string; //node的Type
  std::unordered_map<std::string,GPStore::Value> columns;  //node的属性

  /**
   * node和其他node的关系
   * 第一个参数为关系的类型，eg： PERSON_PLACE
   * 第二个参数为存放的node的总编号
   */
  unordered_map<string, vector<pair<string, string>>> relations;
  unordered_map<string, string> relationsProp;

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

  void addRelation(std::string relationName, std::string index, std::string prop, std::string propValue);

  void print();
};


// 不同节点的列表，格式为id2Node
extern std::unordered_map<std::string, Node> PersonMap;

extern std::unordered_map<std::string, Node> OrganisationMap;
extern std::unordered_map<std::string, Node> CityMap;
extern std::unordered_map<std::string, Node> CompanyMap;

extern std::unordered_map<std::string, Node> PlaceMap;
extern std::unordered_map<std::string, Node> UniversityMap;
extern std::unordered_map<std::string, Node> CountryMap;

extern std::unordered_map<std::string, Node> MessageMap;
extern std::unordered_map<std::string, Node> PostMap;
extern std::unordered_map<std::string, Node> CommentMap;

// 节点的实体id转全局id
extern std::unordered_map<std::string, std::string> PersonIDMap;

extern std::unordered_map<std::string, std::string> OrganisationIDMap;
extern std::unordered_map<std::string, std::string> CityIDMap;
extern std::unordered_map<std::string, std::string> CompanyIDMap;

extern std::unordered_map<std::string, std::string> PlaceIDMap;
extern std::unordered_map<std::string, std::string> UniversityIDMap;
extern std::unordered_map<std::string, std::string> CountryIDMap;

extern std::unordered_map<std::string, std::string> MessageIDMap;
extern std::unordered_map<std::string, std::string> PostIDMap;
extern std::unordered_map<std::string, std::string> CommentIDMap;

