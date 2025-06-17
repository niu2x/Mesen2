import sys
import os

if '__main__' == __name__:

    infile = sys.argv[1]
    outfile = sys.argv[2]

    os.makedirs(os.path.dirname(outfile))

    with open(outfile, 'wt') as out:
        out.write('R"RAW(\n')
        with open(infile) as f:
            out.write(f.read())
        out.write(')RAW"\n')
