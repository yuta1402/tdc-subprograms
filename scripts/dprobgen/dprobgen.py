import argparse
import numpy as np
from scipy import integrate
from scipy.stats import norm

def parse_args():
    parser = argparse.ArgumentParser(
            prog='dprobgen.py',
            description='the generator of the transition probability of drift',
            add_help=True,
            )

    parser.add_argument('pass_ratio', help='pass ratio', type=float)
    parser.add_argument('drift_stddev', help='drift standard devlation', type=float)
    parser.add_argument('max_drift', help='max drift', type=int)
    parser.add_argument('segments', help='num of segments', type=int)

    parser.add_argument('-o', '--output', help='output filepath', type=argparse.FileType('w'))

    args = parser.parse_args()
    return args

def drift_pdf(x, pass_ratio, drift_stddev, c=1.0):
    condlist = [ np.abs(x) < 1.0-pass_ratio, np.abs(x) >= 1.0-pass_ratio ]

    f = lambda x: c*norm.pdf(x, loc=0.0, scale=drift_stddev)
    funclist = [ f , 0.0 ]

    return np.piecewise(x, condlist, funclist)

def main():
    args = parse_args()

    # calculate normalizing constant c
    pdf = lambda x: drift_pdf(x, args.pass_ratio, args.drift_stddev)
    iy, err = integrate.quad(pdf, -np.inf, np.inf)
    c = 1.0 / iy

    pdf = lambda x: drift_pdf(x, args.pass_ratio, args.drift_stddev, c=c)

    drift_size = 2*args.max_drift*args.segments + 1
    prob_table = np.ndarray((drift_size, drift_size))

    max_d = args.max_drift * args.segments
    for da in range(-max_d, max_d+1):
        for db in range(-max_d, max_d+1):
            w = 1.0 / (2.0 * args.segments)
            left = (db - da)/args.segments - w
            right = (db - da)/args.segments + w
            ip, err = integrate.quad(pdf, left, right)

            i = da + max_d
            j = db + max_d
            prob_table[i][j] = ip

        i = da + max_d
        # normalized
        prob_table[i] /= prob_table[i].sum()

    filename = 'prob_table_r{:.4e}_v{:.4e}_d{}_s{}.dat'.format(args.pass_ratio, args.drift_stddev, args.max_drift, args.segments)
    np.savetxt(filename, prob_table, fmt='%.10e', delimiter=' ')

if __name__ == '__main__':
    main()
