#!/usr/bin/python
# This file is part of the dvisvgm package and published under the
# terms of the GNU General Public License version 3 or later.
# See file COPYING for further details.
# Copyright (C) 2016-2017 Martin Gieseking <martin.gieseking@uos.de>

import re
import sys

def extract_hashes (fname):
	with open(fname) as f:
		lines = f.readlines()
		found = False
		for line in lines:
			if ' hash2unicode = {{\n' in line:
				found = True
			elif found:
				match = re.search(r'^\s*{(0x[0-9a-f]{8}),\s*0x[0-9a-f]{4}}, //\s*(.+)\s*$', line)
				if match:
					print '\t{%s, "%s"},' % (match.group(1), match.group(2))
				else:
					found = False

if (len(sys.argv) < 2):
	sys.exit(1)

print """\
#include <xxhash.h>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

struct NameHash {
	unsigned hash;
	string name;
} nameHashes[] = {
"""
extract_hashes(sys.argv[1])
print """\
};

int main () {
	unsigned prev_hash=0;
	size_t size = sizeof(nameHashes)/sizeof(NameHash);
	if (size == 0) {
		cout << "hash table is empty\\n";
		return 1;
	}
	for (unsigned i=0; i < size; i++) {
		const string &name = nameHashes[i].name;
		const unsigned hash = nameHashes[i].hash;
		if (XXH32(&name[0], name.length(), 0) != hash) {
			cout << "hash of '" << name << "' doesn't match\\n";
			return 1;
		}
		if (hash < prev_hash) {
			cout << "misplaced hash value " << hex << setw(8) << setfill('0') << hash << "\\n";
			return 1;
		}
		if (hash == prev_hash) {
			cout << "colliding hash values " << hex << setw(8) << setfill('0') << hash << "\\n";
			return 1;
		}
		prev_hash = hash;
	}
	cout << "hash check passed\\n";
	return 0;
}
"""
