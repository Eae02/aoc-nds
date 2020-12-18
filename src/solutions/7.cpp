#include "sol.hpp"
#include "../common.hpp"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string.h>
#include <stdio.h>

struct Node {
	std::string_view name;
	std::string_view containsStr;
	uint32_t numParents = 0;
	uint32_t numChildren = 0;
	uint32_t childCountMemo = 0;
	bool countParVisited = false;
	Node** parents = nullptr;
	Node** children = nullptr;
	int* childCounts = nullptr;
};

uint32_t countChildren(Node* node) {
	if (node->childCountMemo != 0)
		return node->childCountMemo;
	uint32_t ans = 1;
	for (uint32_t c = 0; c < node->numChildren; c++) {
		ans += node->childCounts[c] * countChildren(node->children[c]);
	}
	node->childCountMemo = ans;
	return ans;
}

uint32_t countParents(Node* node) {
	if (node->countParVisited)
		return 0;
	node->countParVisited = true;
	uint32_t ans = 1;
	for (uint32_t c = 0; c < node->numParents; c++) {
		ans += countParents(node->parents[c]);
	}
	return ans;
}

bool solveDay7(std::string_view input, AnswerBuffer& ans) {
	uint32_t numLines = std::count(input.begin(), input.end(), '\n');
	
	std::vector<Node> nodes;
	nodes.reserve(numLines);
	
	int totalConnections = 0;
	while (!input.empty()) {
		std::string_view line = takeCharsUntil(input, "\n");
		if (line.empty()) continue;
		
		size_t containPos = line.find("contain");
		size_t bagsPos = line.find(" bags");
		if (containPos == std::string_view::npos) continue;
		
		Node& node = nodes.emplace_back();
		node.name = line.substr(0, bagsPos);
		node.containsStr = trim(line.substr(containPos + strlen("contain")));
		
		if (!node.containsStr.starts_with("no other bags")) {
			node.numChildren = std::count(node.containsStr.begin(), node.containsStr.end(), ',') + 1;
			totalConnections += node.numChildren;
		}
	}
	
	std::sort(nodes.begin(), nodes.end(), [&] (const Node& a, const Node& b) {
		return a.name < b.name;
	});
	
	std::unique_ptr<Node*[]> connections = std::make_unique<Node*[]>(totalConnections * 2);
	std::unique_ptr<int[]> childCounts = std::make_unique<int[]>(totalConnections);
	int nextConnectionsIdx = 0;
	
	auto findNodeByName = [&] (std::string_view name) -> Node* {
		auto it = std::lower_bound(nodes.begin(), nodes.end(), name, [&] (const Node& a, const std::string_view& b) { return a.name < b; });
		assert(it != nodes.end() && it->name == name);
		return &*it;
	};
	
	for (Node& node : nodes) {
		if (node.numChildren == 0) {
			continue;
		}
		
		node.children = connections.get() + nextConnectionsIdx;
		node.childCounts = childCounts.get() + nextConnectionsIdx;
		
		std::string_view containsStr = node.containsStr;
		while (!containsStr.empty()) {
			assert(nextConnectionsIdx < totalConnections);
			
			std::string_view part = trim(takeCharsUntil(containsStr, ","));
			childCounts[nextConnectionsIdx] = parseInt(part);
			
			size_t spaceIdx = part.find(" ");
			size_t bagIdx = part.find("bag");
			assert(spaceIdx != std::string_view::npos && bagIdx != std::string_view::npos);
			
			std::string_view otherNodeName = trim(part.substr(spaceIdx, bagIdx - spaceIdx));
			Node* otherNode = findNodeByName(otherNodeName);
			otherNode->numParents++;
			connections[nextConnectionsIdx] = otherNode;
			
			nextConnectionsIdx++;
		}
	}
	assert(nextConnectionsIdx == totalConnections);
	
	nextConnectionsIdx = 0;
	for (Node& node : nodes) {
		node.parents = connections.get() + totalConnections + nextConnectionsIdx;
		nextConnectionsIdx += node.numParents;
		node.numParents = 0;
	}
	assert(nextConnectionsIdx == totalConnections);
	
	for (Node& node : nodes) {
		for (uint32_t c = 0; c < node.numChildren; c++) {
			node.children[c]->parents[node.children[c]->numParents++] = &node;
		}
	}
	
	Node* shinyGoldNode = findNodeByName("shiny gold");
	int part1Ans = countParents(shinyGoldNode) - 1;
	int part2Ans = countChildren(shinyGoldNode) - 1;
	
	ans.answerWithInts(part1Ans, part2Ans);
	return true;
}
