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
		table[name] = std::move(obj);
	}

	T& operator[](const std::string& name) {
		// Throws error if name doesn't exist
		return table.at(name);
	}

	bool contains(const std::string& name) {
		return table.find(name) != table.end();
	}

private:
	std::unordered_map<std::string, T> table;
};

}