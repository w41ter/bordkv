#pragma once 

#include <list>
#include <cstddef>
#include <stdexcept>
#include <unordered_map>

#include "uncopyable.h"

namespace bordkv {

template <typename Key, typename Value> 
class Cache {
	typedef typename std::pair<Key, Value> KeyValuePair;
	typedef typename std::list<KeyValuePair>::iterator kv_iterator;

	__UNCOPYABLE__(Cache);
public: 
	Cache(size_t max_size) 
		: max_size_{ max_size } { }

	void emplace(const Key& key, const Value& value) {
		auto it = cache_items_map_.find(key);
		cache_items_list_.push_front(KeyValuePair(key, value));
		if (it != cache_items_map_.end()) {
			cache_items_list_.erase(it->second);
			cache_items_map_.erase(it);
		}
		cache_items_map_[key] = cache_items_list_.begin();
		
		if (cache_items_map_.size() > max_size_) {
			auto last = cache_items_list_.end();
			last--;
			cache_items_map_.erase(last->first);
			cache_items_list_.pop_back();
		}
	}
	
	const Value& lookup(const Key& key) {
		auto it = cache_items_map_.find(key);
		if (it == cache_items_map_.end()) {
			throw std::range_error("There is no such key in cache");
		} else {
			cache_items_list_.splice(cache_items_list_.begin(), 
				cache_items_list_, it->second);
			return it->second->second;
		}
	}
	
	bool contains(const Key& key) const {
		return cache_items_map_.find(key) != cache_items_map_.end();
	}
	
	size_t size() const {
		return cache_items_map_.size();
	}
	
private:
	std::list<KeyValuePair> cache_items_list_;
	std::unordered_map<Key, kv_iterator> cache_items_map_;
	size_t max_size_;
};

} // namespace bordkv
