#include <string>
#include <memory>
#include "Value.h"
#include "unordered_map"
using namespace std;

typedef unsigned long long ull;

class Node {
public:
    ull node_id_; //每个node要维护一个新的id
    std::string label_string; //node的Type
    std::unordered_map<std::string, GPStore::Value> columns;  //node的属性

    unordered_map<string, vector<string>> inRelations;
    unordered_map<string, vector<string>> outRelations;
    unordered_map<string, string> relationsProp;
    unordered_map<string, string> typeToRelation;

    Node()=default;
    Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value);
    Node(ull node_id);
    void Goto(ull new_node_id) {
        node_id_ = new_node_id;
    }

    GPStore::Value* operator[](const std::string& property_string);
    void GetLinkedNodes(const std::string&, std::shared_ptr<const ull[]>& nodes_list, ull& list_len, char edge_dir);
    void GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const ull[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                     ull& prop_len, ull& list_len, char edge_dir);



    void setLabel(const std::string& basic_string);
    void setValues(const std::string& prop_string, const GPStore::Value* value);

    void addRelation(int direct, std::string relationName, std::string index, std::string prop, std::string propValue);
    void changeTypeToRelation(string name, string relname);
    void print();


};


// 不同节点的列表，格式为id2Node
extern std::unordered_map<std::string, std::shared_ptr<Node>> totalMap;

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
