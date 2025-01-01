#include <iostream>
#include "Node.h"



// 不同节点的列表，格式为id2Node
std::unordered_map<std::string, Node> totalMap;

std::unordered_map<std::string, Node> PersonMap;

std::unordered_map<std::string, Node> OrganisationMap;
std::unordered_map<std::string, Node> CityMap;
std::unordered_map<std::string, Node> CompanyMap;

std::unordered_map<std::string, Node> PlaceMap;
std::unordered_map<std::string, Node> UniversityMap;
std::unordered_map<std::string, Node> CountryMap;

std::unordered_map<std::string, Node> MessageMap;
std::unordered_map<std::string, Node> PostMap;
std::unordered_map<std::string, Node> CommentMap;

// 节点的实体id转全局id
std::unordered_map<std::string, std::string> PersonIDMap;

std::unordered_map<std::string, std::string> OrganisationIDMap;
std::unordered_map<std::string, std::string> CityIDMap;
std::unordered_map<std::string, std::string> CompanyIDMap;

std::unordered_map<std::string, std::string> PlaceIDMap;
std::unordered_map<std::string, std::string> UniversityIDMap;
std::unordered_map<std::string, std::string> CountryIDMap;

std::unordered_map<std::string, std::string> MessageIDMap;
std::unordered_map<std::string, std::string> PostIDMap;
std::unordered_map<std::string, std::string> CommentIDMap;

std::unordered_map<string, std::unordered_map<string, Node>*> type2Map={
        {"Person", &PersonMap},
        {"Comment", &CommentMap},
        {"Post", &PostMap},
        {"University", &UniversityMap},
        {"Company", &CompanyMap},
        {"City", &CityMap},
        {"Country", &CountryMap},
        {"Message", &MessageMap},
        {"Organisation", &OrganisationMap},
        {"Place", &PlaceMap},
};

std::unordered_map<string, std::unordered_map<string, string>*> type2IDMap={
        {"Person", &PersonIDMap},
        {"Comment", &CommentIDMap},
        {"Post", &PostIDMap},
        {"University", &UniversityIDMap},
        {"Company", &CompanyIDMap},
        {"City", &CityIDMap},
        {"Country", &CountryIDMap},
        {"Message", &MessageIDMap},
        {"Organisation", &OrganisationIDMap},
        {"Place", &PlaceIDMap},
};

Node::Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value) {
    if(prop_string != "id") {
        throw runtime_error("无法查找相应属性: " + prop_string);
    }

    auto& innerMap = *type2Map[label_string];
    auto& innerIDMap = *type2IDMap[label_string];

    // 获取全局索引
    string index = innerIDMap[value->toString()];
    
    // 从Map中获取对应节点
    const Node& source_node = innerMap[index];
    
    // 复制节点的所有属性到当前对象
    this->node_id_ = source_node.node_id_;
    this->label_string = source_node.label_string;
    this->columns = source_node.columns;
    this->inRelations = source_node.inRelations;
    this->outRelations = source_node.outRelations;
    this->relationsProp = source_node.relationsProp;
    this->typeToRelation = source_node.typeToRelation;
}

Node::Node(unsigned node_id) {
    // 从Map中获取对应节点
    const Node& source_node = totalMap[to_string(node_id)];

    // 复制节点的所有属性到当前对象
    this->node_id_ = source_node.node_id_;
    this->label_string = source_node.label_string;
    this->columns = source_node.columns;
    this->inRelations = source_node.inRelations;
    this->outRelations = source_node.outRelations;
    this->relationsProp = source_node.relationsProp;
    this->typeToRelation = source_node.typeToRelation;
}

GPStore::Value* Node::operator[](const std::string& property_string) {
    // 首先在columns中查找属性
    // 遍历所有columns,检查key的前缀是否匹配
    for(auto& it : columns) {
        // 找到冒号的位置
        size_t pos = it.first.find(':');
        if(pos != string::npos) {
            // 只比较冒号前的部分
            if(it.first.substr(0, pos) == property_string) {
                return &(it.second);
            }
        }
    }
    
    // 如果在columns中找不到,检查是否在typeToRelation中
    auto relIt = this->typeToRelation.find(property_string);
    if(relIt != this->typeToRelation.end()) {
        // 找到对应的relation名称
        string relationName = relIt->second;
        
        // 在outRelations中查找该relation
        auto outIt = outRelations.find(relationName);
        if(outIt != outRelations.end() && !outIt->second.empty()) {

            // 返回第一个关联节点的ID
            string target = outIt->second[0];
            size_t pos = target.find('|');
            string targetIndex = target.substr(0, pos);

            return new GPStore::Value(stoll(targetIndex));
        }
    }
    
    // 如果都找不到,返回一个空值
    return new GPStore::Value(-1);
}

void Node::GetLinkedNodes(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, unsigned& list_len, char edge_dir) {
    vector<unsigned> result;
    
    // 根据边的方向选择要遍历的关系集合
    const auto& relations = (edge_dir == 'i') ? inRelations : outRelations;
    
    // 如果存在该关系,则遍历所有目标节点
    if(relations.find(pre_str) != relations.end()) {
        for(const auto& target : relations.at(pre_str)) {
            string targetIndex = target.substr(0, target.find('|'));
            result.push_back(stoul(targetIndex));
        }
    }

    // 设置返回结果
    list_len = result.size();
    if (list_len > 0) {
        // 创建新数组并复制结果
        unsigned* temp = new unsigned[list_len];
        for (size_t i = 0; i < list_len; i++) {
            temp[i] = result[i];
        }
        nodes_list.reset(temp);
    } else {
        nodes_list = nullptr;
    }

}

void Node::GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                       unsigned& prop_len, unsigned& list_len, char edge_dir) {
    vector<unsigned> node_result;
    vector<long long> prop_result;
    
    // 根据边的方向选择要遍历的关系集合
    const auto& relations = (edge_dir == 'i') ? inRelations : outRelations;
    
    // 如果存在该关系,则遍历所有目标节点
    if(relations.find(pre_str) != relations.end()) {
        for(const auto& target : relations.at(pre_str)) {
            size_t pos = target.find('|');
            string targetIndex = target.substr(0, pos);
            string propValue = target.substr(pos + 1);
            
            node_result.push_back(stoul(targetIndex));
            prop_result.push_back(stoll(propValue));
        }
    }

    // 设置返回结果
    list_len = node_result.size();
    prop_len = prop_result.size();
    
    if (list_len > 0) {
        // 创建新数组并复制结果
        unsigned* temp_nodes = new unsigned[list_len];
        long long* temp_props = new long long[prop_len];
        
        for (size_t i = 0; i < list_len; i++) {
            temp_nodes[i] = node_result[i];
            temp_props[i] = prop_result[i];
        }
        
        nodes_list.reset(temp_nodes);
        prop_list.reset(temp_props);
    } else {
        nodes_list = nullptr;
        prop_list = nullptr;
    }
}

void Node::setLabel(const std::string &basic_string) {
    this->label_string = basic_string;
}

void Node::setValues(const std::string &prop_string, const GPStore::Value *value) {
    if (value != nullptr) {
        this->columns[prop_string] = *value;
        delete value; // 删除传入的指针
    }
}

void Node::print() {
    std::cout << "Node_id: " << this->node_id_ << "\n";
    std::cout << "label_string: " << this->label_string << "\n";
    std::cout << "columns_size: " << this->columns.size() << "\n";
    for(auto& item:this->columns)
        std::cout << item.first << ": " << item.second.toString() << "\n";
    std::cout << "inRelations_size: " << this->inRelations.size() << "\n";
    for(auto& item:this->inRelations)
        std::cout << item.first << "\n";
    std::cout << "outRelations_size: " << this->outRelations.size() << "\n";
    for(auto& item:this->outRelations)
        std::cout << item.first << "\n";
}

/**
   * direct 0 为入边； 1 为出边
   */

void Node::addRelation(int direct, std::string relationName, std::string index, std::string prop, std::string propValue) {
    // 构建关系字符串,格式为"节点索引|属性值" 
    string relationStr = index + "|" + propValue;
    
    // 根据方向添加关系
    if (direct == 0) {
        // 添加入边关系
        this->inRelations[relationName].push_back(relationStr);
    } else {
        // 添加出边关系 
        this->outRelations[relationName].push_back(relationStr);
    }

    // 记录关系的属性名
    this->relationsProp[relationName] = prop;
}

void Node::changeTypeToRelation(string name, string relName){
    this->typeToRelation[name] = relName;
}