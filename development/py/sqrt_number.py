from fractions import Fraction

class SqrtNumber(object):

    def __init__(self, a, b, r):
        self.a = Fraction(a)
        self.b = Fraction(b)
        self.r = Fraction(r)

    def __repr__(self):
        return "%s + %s*sqrt(%s)"%(str(self.a), str(self.b), str(self.r))

    def __cmp__(self, other):
        a, b, r = self.a, self.b, self.r
        c, d, s = other.a, other.b, other.r
        
    
a = SqrtNumber(1, 2, 3)
b = SqrtNumber(2, 3, 4)

print(a)
print(b)
