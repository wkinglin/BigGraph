#include "PProcedure.h"
#include <iostream>
using namespace std;
const ull LIMIT_NUM = 20;
const char EDGE_IN = 'i';
const char EDGE_OUT = 'o';

void ProcessMessageLikes(Node &message, long long message_id, long long message_creation_date,
                         const std::string &message_content, Node &other_person, std::map<TYPE_ENTITY_LITERAL_ID, long long> &person_id_map,
                         std::map<long long, std::pair<long long, long long> > &candidates_index, std::map<std::pair<long long, long long>, std::tuple<long long, long long, std::string, int> > &candidates) {
    std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> person_friends = nullptr;
    ull friends_num = 0;
    std::shared_ptr<const long long[]> creation_date_list = nullptr;
    ull creation_data_width = 0;
    message.GetLinkedNodesWithEdgeProps("LIKES", person_friends, creation_date_list, creation_data_width, friends_num, EDGE_IN);
    for (ull j = 0; j < friends_num; ++j) {
        auto person_vid = person_friends[j];
        long long like_creation_date = creation_date_list[j];
        auto it = candidates_index.find(person_vid);
        if (it != candidates_index.end()) {
            auto &key = it->second;
            if (like_creation_date < 0 - key.first) {
                continue;
            }
            if (like_creation_date == 0 - key.first) {
                auto cit = candidates.find(key);
                long long old_message_id = std::get<1>(cit->second);
                if (message_id > old_message_id) {
                    continue;
                }
            }
            candidates.erase(key);
            key.first = 0 - like_creation_date;
            candidates.emplace(key, std::make_tuple(person_vid, message_id, message_content,
                                                    (like_creation_date - message_creation_date) / 1000 / 60));
        } else {
            long long person_id;
            auto pit = person_id_map.find(person_vid);
            if (pit != person_id_map.end()) {
                person_id = pit->second;
            } else {
                other_person.Goto(person_vid);
                person_id = other_person["id"]->toLLong();
                person_id_map[person_vid] = person_id;
            }
            auto key = std::make_pair(0 - like_creation_date, person_id);
            if (candidates.size() >= LIMIT_NUM && candidates.lower_bound(key) == candidates.end()) {
                continue;
            }
            candidates.emplace(key, std::make_tuple(person_vid, message_id, message_content,
                                                    (like_creation_date - message_creation_date) / 1000 / 60));
            candidates_index.emplace(person_vid, key);
            if (candidates.size() > LIMIT_NUM) {
                auto cit = --candidates.end();
                candidates_index.erase(candidates_index.find(std::get<0>(cit->second)));
                candidates.erase(cit);
            }
        }
    }
}

void ic1(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result) {
    string first_name = args[1].toString();
    const char* first_name_char = first_name.data();
    ull first_name_size = first_name.size();
    Node person_node("Person", "id", &args[0]);
    if (person_node.node_id_ == -1)
        return;
    std::set<std::tuple<int, std::string, long long, ull> > candidates;
    TYPE_ENTITY_LITERAL_ID start_vid = person_node.node_id_;
    std::vector<TYPE_ENTITY_LITERAL_ID> curr_frontier({start_vid});
    std::set<TYPE_ENTITY_LITERAL_ID> visited({start_vid});

    for (int distance = 0; distance <= 3; distance++) {
        std::vector<TYPE_ENTITY_LITERAL_ID > next_frontier;
        for (const auto& vid : curr_frontier) {
            Node froniter_person(vid);
            bool flag = vid == start_vid;
            flag = flag || (froniter_person["firstName"]->toString() != first_name);
            if (flag) continue;
            std::string last_name = froniter_person["lastName"]->toString();
            long long person_id = froniter_person["id"]->toLLong();
            auto tup = std::make_tuple(distance, last_name, person_id, vid);
            if (candidates.size() >= LIMIT_NUM) {
                auto& candidate = *candidates.rbegin();
                if (tup > candidate) continue;
            }
            candidates.emplace(std::move(tup));
            if (candidates.size() > LIMIT_NUM) {
                candidates.erase(--candidates.end());
            }
        }
        if (candidates.size() >= LIMIT_NUM || distance == 3) break;
        for (auto vid : curr_frontier) {
            Node froniter_person(vid);
            std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> friends_list = nullptr; ull list_len;
            froniter_person.GetLinkedNodes("KNOWS", friends_list, list_len, EDGE_OUT);
            for (unsigned friend_index = 0; friend_index < list_len; ++friend_index) {
                TYPE_ENTITY_LITERAL_ID friend_vid = friends_list[friend_index];
                if (visited.find(friend_vid) == visited.end()) {
                    visited.emplace(friend_vid);
                    next_frontier.emplace_back(friend_vid);
                }
            }
            friends_list = nullptr;
            froniter_person.GetLinkedNodes("KNOWS", friends_list, list_len, EDGE_IN);
            for (unsigned friend_index = 0; friend_index < list_len; ++friend_index) {
                TYPE_ENTITY_LITERAL_ID friend_vid = friends_list[friend_index];
                if (visited.find(friend_vid) == visited.end()) {
                    visited.emplace(friend_vid);
                    next_frontier.emplace_back(friend_vid);
                }
            }
            friends_list = nullptr;
        }
        std::sort(next_frontier.begin(), next_frontier.end());
        curr_frontier.swap(next_frontier);
    }

    for (const auto & tup : candidates) {
        ull vid = std::get<3>(tup);
        Node person(vid);

        result.emplace_back();
        result.back().reserve(13);
        result.back().emplace_back(std::get<2>(tup));
        result.back().emplace_back(std::get<1>(tup));
        result.back().emplace_back(std::get<0>(tup));
        result.back().emplace_back(*person["birthday"]);
        result.back().emplace_back(*person["creationDate"]);
        result.back().emplace_back(*person["gender"]);
        result.back().emplace_back(*person["browserUsed"]);
        result.back().emplace_back(*person["locationIP"]);
        result.back().emplace_back(*person["email"]);
        result.back().emplace_back(*person["language"]);
        result.back().emplace_back(*Node(person["PERSON_PLACE"]->toLLong())["name"]);   // TODO: PERSON_PLACE not in original data

        std::shared_ptr<const ull[]> list = nullptr; ull list_len = 0;
        std::shared_ptr<const long long[]> prop_list = nullptr; ull prop_len = 0;

        result.back().emplace_back(GPStore::Value::Type::LIST);
        person.GetLinkedNodesWithEdgeProps("STUDYAT", list, prop_list, prop_len, list_len, EDGE_OUT);
        for (unsigned i = 0; i < list_len; ++i) {
            Node university(list[i]);
            Node location_city(university["ORGANISATION_PLACE"]->toLLong());    // TODO: "ORGANISATION_PLACE"
            vector<GPStore::Value *> university_prop_vec{university["name"], new GPStore::Value(prop_list[i]), location_city["name"]};
            result.back().back().data_.List->emplace_back(new GPStore::Value(university_prop_vec, true));
        }
        list = nullptr; prop_list = nullptr;

        result.back().emplace_back(GPStore::Value::Type::LIST);
        person.GetLinkedNodesWithEdgeProps("WORKAT", list, prop_list, prop_len, list_len, EDGE_OUT);
        for (unsigned i = 0; i < list_len; ++i) {
            Node company(list[i]);
            Node location_country(company["ORGANISATION_PLACE"]->toLLong());
            vector<GPStore::Value *> university_prop_vec{company["name"], new GPStore::Value(prop_list[i]), location_country["name"]};
            result.back().back().data_.List->emplace_back(new GPStore::Value(university_prop_vec, true));
        }
    }
}

void ic2(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result) {
    // 解析参数：personId和maxDate
    TYPE_ENTITY_LITERAL_ID personId = args[0].toLLong();
    GPStore::Value maxDate = args[1];

    // 通过personId找到其对应的nodeId
    TYPE_ENTITY_LITERAL_ID nodeId = 0;
    if(PersonIDMap.find(to_string(personId))==PersonIDMap.end())
    {
        printf("What the fuck? This person (id: %lld) does not exist!!!\n", personId);
    }
    else
    {
        nodeId = stoll(PersonIDMap[to_string(personId)]);
    }
    if(totalMap.find(to_string(nodeId))==totalMap.end())
    {
        printf("What the fuck? This Node (id: %lld) does not exist!!!\n", nodeId);
    }
    Node person(nodeId);

    // 先找出这个Person的所有Friend Node
    std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> friends_list = nullptr; // 这个list里面存着所有Friend Node的ID
    std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> friends_list_in = nullptr; // 这个list里面存着所有Friend Node的ID
    ull list_len, list_len1;
    person.GetLinkedNodes("KNOWS", friends_list, list_len, EDGE_OUT);
    person.GetLinkedNodes("KNOWS", friends_list_in, list_len1, EDGE_IN);

    // 自定义比较函数
    struct CustomCompare {
        bool operator()(const std::tuple<ull, ull, ull, ull>& a, const std::tuple<ull, ull, ull, ull>& b) const {
            // 先比较第一个元素（降序）
            if (std::get<0>(a) != std::get<0>(b)) {
                return std::get<0>(a) > std::get<0>(b);  // 降序
            }
            // 如果第一个元素相同，再比较第二个元素（升序）
            if (std::get<1>(a) != std::get<1>(b)) {
                return std::get<1>(a) < std::get<1>(b);  // 升序
            }
            // 如果前两个元素相同，第三个元素保持默认顺序（无需排序）
            return false;  // 第三个元素不参与排序
        }
    };

    // 对每一个Friend Node，找出它的所有Message
    std::set<std::tuple<ull, ull, ull, ull>, CustomCompare> candidates;
    for(int i = 0;i<list_len;i++)
    {
        Node friend_node(friends_list[i]);
        std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> msg_list = nullptr; // 这个list里面存着这个人所有Message Node的ID
        ull msg_list_len;
        friend_node.GetLinkedNodes("HASCREATOR", msg_list, msg_list_len, EDGE_IN);
        // 将这些Messages放入列表中
        for(int j = 0;j<msg_list_len;j++)
        {
            Node msg(msg_list[j]);
            if(msg["creationDate"]->comp(maxDate)==1) continue;
            auto tup = std::make_tuple(msg["creationDate"]->toLLong(), msg["id"]->toLLong(), friend_node.node_id_, msg.node_id_);
            if (candidates.size() >= LIMIT_NUM) {
                auto& candidate = *candidates.rbegin();
                if (!CustomCompare()(tup, candidate)) {
                    continue;  // 如果当前元素比候选集中的最小元素还小，则跳过插入
                }
            }
            candidates.emplace(std::move(tup));
            if (candidates.size() > LIMIT_NUM) {
                candidates.erase(--candidates.end());
            }
        }
    }
    for(int i = 0;i<list_len1;i++)
    {
        Node friend_node(friends_list_in[i]);
        std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> msg_list = nullptr; // 这个list里面存着这个人所有Message Node的ID
        ull msg_list_len;
        friend_node.GetLinkedNodes("HASCREATOR", msg_list, msg_list_len, EDGE_IN);
        // 将这些Messages放入列表中
        for(int j = 0;j<msg_list_len;j++)
        {
            Node msg(msg_list[j]);
            if(msg["creationDate"]->comp(maxDate)==1) continue;
            auto tup = std::make_tuple(msg["creationDate"]->toLLong(), msg["id"]->toLLong(), friend_node.node_id_, msg.node_id_);
            if (candidates.size() >= LIMIT_NUM) {
                auto& candidate = *candidates.rbegin();
                if (!CustomCompare()(tup, candidate)) {
                    continue;  // 如果当前元素比候选集中的最小元素还小，则跳过插入
                }
            }
            candidates.emplace(std::move(tup));
            if (candidates.size() > LIMIT_NUM) {
                candidates.erase(--candidates.end());
            }
        }
    }

    for(const auto &tup: candidates)
    {
        ull fid = std::get<2>(tup);
        ull mid = std::get<3>(tup);
        Node friend_node(fid);
        Node msg_node(mid);

        // 处理imageFile
        GPStore::Value* content = msg_node["content"];
        if(!content->comp(new GPStore::Value(""))) content = msg_node["imageFile"];

        result.emplace_back();
        result.back().reserve(13);
        result.back().emplace_back(*friend_node["id"]);
        result.back().emplace_back(*friend_node["firstName"]);
        result.back().emplace_back(*friend_node["lastName"]);
        result.back().emplace_back(*msg_node["id"]);
        result.back().emplace_back(*content);
        result.back().emplace_back(*msg_node["creationDate"]);
    }
}

void is1(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result) {
    Node person_node("Person", "id", &args[0]);
    if (person_node.node_id_ == -1)
        return;
    result.emplace_back();
    result.back().reserve(8);
    result.back().emplace_back(*person_node["firstName"]);
    result.back().emplace_back(*person_node["lastName"]);
    result.back().emplace_back(*person_node["birthday"]);
    result.back().emplace_back(*person_node["locationIP"]);
    result.back().emplace_back(*person_node["browserUsed"]);
    Node city_node(person_node["PERSON_PLACE"]->toLLong());
    result.back().emplace_back(*city_node["id"]);
    result.back().emplace_back(*person_node["gender"]);
    result.back().emplace_back(*person_node["creationDate"]);
}