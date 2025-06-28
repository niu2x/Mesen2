import sys
import os

if '__main__' == __name__:

    infile = sys.argv[1]
    outfile = sys.argv[2]

    os.makedirs(os.path.dirname(outfile), exist_ok=True)

    with open(outfile, 'wt', encoding='utf-8') as out:
        out.write('R"RAW(\n')
        with open(infile, 'rt', encoding='utf-8') as f:
            out.write(f.read())
        out.write(')RAW"\n')
