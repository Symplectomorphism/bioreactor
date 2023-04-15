import sys
import math

"""
Command line: provide value for C2 and the desired R1/R2 ratio.

Example usage: 
(1) python3 compute 1.0e-9 4.0: corresponds to a desired C2 = 1nF, R1 = 4*R2
    Output: R1 = 400140, R2 = 100035, C1 = 3.125n, C2 = 1n
(2) python3 compute 4.7e-9 10.0: corresponds to a desired C2 = 4.7nF, R1 = 10*R2
    Output: R1 = 96746, R2 = 9674.6, C1 = 28.44n, C2 = 4.7n
"""

def computeResistances(C2: float, ratio: float):
    """
    ratio = R1/R2

    Constraints:
    (R1+R2)^2 * C2 = 2 * R1 * R2 * C1
    -40 >= 20 log(10, T(jw_c)), w_c: carrier freq.
    sqrt(R1*R2*C1*C2) <= 1/2/pi/fn, fn = 5 * 90Hz, 90Hz: desired osc. freq.
    """
    fn = 450
    sum = 1/math.sqrt(2)/math.pi/fn/C2
    R2 = sum/(ratio+1)
    R1 = ratio*R2
    return R1, R2

def computeCapacitance(C2: float, R1: float, R2:float):
    return C2*((R1+R2)**2)/2/R1/R2

C2 = float(sys.argv[1])
r = float(sys.argv[2])
R1, R2 = computeResistances(C2, r)
C1 = computeCapacitance(C2, R1, R2)

print(R1, R2, C1, C2)
