import sys
import math
import numpy as np

"""
Command line: provide desired values for 
  1. alpha1,    tolerated attenuation at operation frequency, fo
  2. C2,
  3. r = R1/R2 ratio.

Example usage: 
(1) python3 compute 1.001 1e-9 5.0:
    corresponds to a desired alpha1 = 1.001, C2 = 1nF, R1 = 5*R2
    Output: R1 = 440781, R2 = 88156, C1 = 3.6n, C2 = 1n
"""

class Bioreactor:
    R1, R2, C1 = 0.0, 0.0, 0.0
    alpha2 = 100.0;
    fo, fc = 90, 36600
    fn = 1.0

    def __init__(self, C2: float=1e-9, alpha1: float=1.001, rho: float=1.0):
        self.alpha1 = alpha1
        self.C2 = C2
        self.Rratio = rho
        self.computeResistances()
        self.computeCapacitance()
        self.computeNaturalFreq()

    def tf(self, s):
        coeff2 = self.R1*self.R2*self.C1*self.C2
        coeff1 = (self.R1+self.R2)*self.C2
        coeff0 = 1.0
        return 1/(coeff2*s*s + coeff1*s + coeff0)

    def computeResistances(self):
        """
        ratio = R1/R2

        Constraints:
        (R1+R2)^2 * C2 = 2 * R1 * R2 * C1
        tolerated attenuation at fo
        desired attenuation at fc
        """
        _, right = self.computeInterval()
        sum_resistances = right/self.C2
        self.R2 = sum_resistances/(self.Rratio+1)
        self.R1 = self.Rratio*self.R2

    def computeCapacitance(self):
        self.C1 = self.C2*((self.R1+self.R2)**2)/2/self.R1/self.R2

    def gain(self, f: float):
        return np.abs(self.tf(2*np.pi*complex(0,f)))

    def gaindb(self, f: float):
        return 20*np.emath.logn(10, self.gain(f))

    def phaseShift(self, f: float):
        return np.angle(self.tf(2*np.pi*complex(0,f)))

    def phaseShiftDeg(self, f: float):
        return np.angle(self.tf(2*np.pi*complex(0,f)))*180/np.pi

    def computeNaturalFreq(self):
        _, r = self.computeInterval()
        self.fn = 1/np.sqrt(2)/np.pi/r

    def computeInterval(self):
        left = np.power(self.alpha2**2-1, 1/4)/np.sqrt(2)/np.pi/self.fc
        right = np.power(self.alpha1**2-1, 1/4)/np.sqrt(2)/np.pi/self.fo
        return left, right


# alpha1 = float(sys.argv[1])
# C2 = float(sys.argv[2])
# rho  = float(sys.argv[3])

# alpha1 = 1.00165518
# alpha1 = 1.00079968
alpha1 = 1.001
C2 = 1.0e-9
rho  = 5.0

b = Bioreactor(alpha1=alpha1, C2=C2, rho=rho)

b.computeResistances()
b.computeCapacitance()

print(b.R1, b.R2, b.C1, b.C2)
