#!/usr/bin/python
# This file is part of the dvisvgm package and published under the
# terms of the GNU General Public License version 3 or later.
# See file COPYING for further details.
# Copyright (C) 2016-2020 Martin Gieseking <martin.gieseking@uos.de>

import re
import sys

def extract_hashes (fname):
    with open(fname) as f:
        lines = f.readlines()
        found = False
        for line in lines:
            if ' hash2unicode {{\n' in line:
                found = True
            elif found:
                match = re.match(r'\s*{(0x[0-9a-f]{8}),\s*0x[0-9a-f]{4}}, //\s*(.+)\s*$', line)
                if match:
                    hashval = match.group(1)
                    name = match.group(2)
                    print('\t{}{}, "{}"{},'.format('{', hashval, name, '}'))
                else:
                    found = False

if (len(sys.argv) < 2):
    sys.exit(1)

print("""\
#include <xxhash.h>
#include <cstdint>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

struct NameHash {
    uint32_t hash;
    string name;
} nameHashes[] = {\
""")
extract_hashes(sys.argv[1])
print(r"""};

int main () {
    if (distance(begin(nameHashes), end(nameHashes)) == 0) {
        cout << "hash table is empty\n";
        return 1;
    }
    uint32_t prev_hash=0;
    for (NameHash &nameHash : nameHashes) {
        const string &name = nameHash.name;
        const uint32_t hash = nameHash.hash;
        if (XXH32(&name[0], name.length(), 0) != hash) {
            cout << "hash of '" << name << "' doesn't match\n";
            return 1;
        }
        if (hash < prev_hash) {
            cout << "misplaced hash value " << hex << setw(8) << setfill('0') << hash << "\n";
            return 1;
        }
        if (hash == prev_hash) {
            cout << "colliding hash values " << hex << setw(8) << setfill('0') << hash << "\n";
            return 1;
        }
        prev_hash = hash;
    }
    cout << "hash check passed\n";
    return 0;
}
""")
