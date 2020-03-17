import re
import os

def main (latex_file, stdout):
    latex_file_old = latex_file+"-old"
    os.rename(latex_file, latex_file_old)
    os.remove(os.path.splitext(latex_file)[0]+".pdf")
    with open(latex_file, "w") as outfile:
        with open(latex_file_old) as infile:
            lines = infile.readlines()
            for line in lines:
                if re.match(r'(.*\\def)|(.*\\href)', line) == None:
                    line = re.sub(r'([a-zA-Z0-9]+)/', r'\1\slash{}', line)
                    line = re.sub(r'-{}-{}', r'\=/\=/', line)
                    line = re.sub(r'([^a-zA-Z0-9])-{}', r'\1\=/', line)
                print >>outfile, line.rstrip()
    os.remove(latex_file_old)
    return 0
