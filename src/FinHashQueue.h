//
// Created by YubinKim on 25/03/16 일.
//

#ifndef FINHASHQUEUE_H
#define FINHASHQUEUE_H
#include <unordered_map>


/**
 * 원소가 추가된 순서를 유지하는 해시 맵입니다. 
 * value의 선입선출을 유지하고, key를 통해 삭제할 수 있습니다.
 * 탐색 및 삽입, 삭제 모두 상수시간 복잡도를 가집니다.
 */
template <typename _Kty, typename _Valty, typename _Hasher= std::hash<_Kty>, typename _Keyeq = std::equal_to<_Kty>>
class hash_queue
{
    using pair_list = std::list<std::pair<_Kty, _Valty>>;

    pair_list keyValQueue; // key, value queue
	
    std::unordered_map<_Kty, typename pair_list::iterator, _Hasher, _Keyeq> keyListHash; // key, list_iterator 매핑 테이블

public:
    void push(const _Kty& key, const _Valty& value)
    {
        // 순서 유지를 위해, 이미 존재할 경우 삭제 후 다시 삽입
        if (keyListHash.contains(key))
        {
            erase(key);
        }

        // 리스트에 삽입 및 해시 테이블에 위치 저장
        keyListHash[key] = keyValQueue.emplace(keyValQueue.end(), key, value);
    }

    _Valty top()
    {
        if (keyValQueue.empty())
        {
            return {};
        }

        return keyValQueue.front().second;
    }

    void pop()
    {
        if (keyValQueue.empty())
        {
            return;
        }

        keyListHash.erase(keyValQueue.front().first);
        keyValQueue.pop_front();
    }

    bool erase(const _Kty& key)
    {
        const auto findIt = keyListHash.find(key);
        if (findIt == keyListHash.end())
        {
            return false;
        }

        keyValQueue.erase(findIt->second);
        keyListHash.erase(findIt);
        return true;
    }

    _Valty* find(const _Kty& key)
    {
        const auto findIt = keyListHash.find(key);
        if (findIt == keyListHash.end())
        {
            return nullptr;
        }

        return &(findIt->second->second);
    }

    bool empty()
    {
        return keyValQueue.empty();
    }

    size_t size()
    {
        return keyValQueue.size();
    }
};


#endif //FINHASHQUEUE_H
