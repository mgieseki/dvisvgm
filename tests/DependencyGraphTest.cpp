/*************************************************************************
** DependencyGraphTest.cpp                                              **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include <gtest/gtest.h>
#include "optimizer/DependencyGraph.hpp"

using namespace std;

static void populate (DependencyGraph<int> &tree) {
	tree.insert(1);
	tree.insert(2);
	tree.insert(1, 3);
	tree.insert(1, 4);
	tree.insert(1, 5);
	tree.insert(4, 6);
	tree.insert(4, 7);
}


TEST(DependencyGraphTest, getKeys) {
	DependencyGraph<int> graph;
	populate(graph);
	auto keys = graph.getKeys();
	ASSERT_EQ(keys.size(), 7u);
	int count=0;
	for (int key : keys) {
		ASSERT_EQ(keys[count++], key);
	}
}


TEST(DependencyGraphTest, insert) {
	DependencyGraph<int> graph;
	populate(graph);
	for (int i=1; i <= 7; i++) {
		ASSERT_TRUE(graph.contains(i));
	}
	ASSERT_FALSE(graph.contains(0));
	ASSERT_FALSE(graph.contains(8));
}


TEST(DependencyGraphTest, removeDependencyPath) {
	DependencyGraph<int> graph;
	populate(graph);
	graph.removeDependencyPath(4);
	ASSERT_FALSE(graph.contains(1));
	ASSERT_TRUE(graph.contains(2));
	ASSERT_TRUE(graph.contains(3));
	ASSERT_FALSE(graph.contains(4));
	ASSERT_TRUE(graph.contains(5));
	ASSERT_TRUE(graph.contains(6));
	ASSERT_TRUE(graph.contains(7));
}
