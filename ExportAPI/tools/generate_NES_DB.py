import sys
import os

if '__main__' == __name__:

    infile = sys.argv[1]
    outfile = sys.argv[2]

    os.makedirs(os.path.dirname(outfile), exist_ok=True)

    with open(outfile, 'wt', encoding='utf-8') as out:
        with open(infile, 'rt', encoding='utf-8') as f:
            for line in f.readlines():
                line = line.strip()
                out.write(f'"{line}\\n"\n')
