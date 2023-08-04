#pragma once

#include <unordered_map>

#include "core.h"

namespace GRender {

template <typename T>
class Table {
public:
	Table(void) = default;
	~Table(void) = default;

	// To avoid problems with possible GPU data
	Table(const Table&) = delete;
	Table& operator=(const Table&) = delete;

	void insert(const std::string& name, T&& obj) {
		ASSERT(!contains(name), "Table already contains -> " + name);
		table.emplace(name, std::move(obj));
	}

	template<typename ...ARGS>
	void emplace(const std::string& name, ARGS&& ...args) {
		ASSERT(!contains(name), "Table already contains -> " + name);
		table.emplace(std::piecewise_construct,
					  std::forward_as_tuple(name),
					  std::forward_as_tuple(std::forward<ARGS>(args)...));
	}

	T& operator[](const std::string& name) {
		ASSERT(contains(name), "Table doesn't contains -> " + name);
		return table.at(name);
	}

	bool contains(const std::string& name) {
		return table.find(name) != table.end();
	}

private:
	std::unordered_map<std::string, T> table;
};

}