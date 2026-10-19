"""Generate exact rational/integer goldens for the referenced MSL floating-point entries."""
from fractions import Fraction
from math import isqrt
from pathlib import Path
import random

ROOT=Path(__file__).resolve().parents[2]

def power(exponent): return Fraction(1<<exponent) if exponent>=0 else Fraction(1,1<<-exponent)
def exponent(value):
    result=value.numerator.bit_length()-value.denominator.bit_length()
    return result-1 if value<power(result) else result
def finite(bits,width):
    mantissa,ebits,bias=(23,8,127) if width==32 else (52,11,1023)
    exp=(bits>>mantissa)&((1<<ebits)-1); fraction=bits&((1<<mantissa)-1)
    return (-1 if bits>>(width-1) else 1)*Fraction(fraction+((1<<mantissa) if exp else 0))*power((exp or 1)-bias-mantissa)
def round32(value,zero_sign=0):
    if not value: return zero_sign<<31
    sign=int(value<0); value=abs(value); exp=exponent(value)
    quantum=max(exp-23,-149); scaled=value/power(quantum)
    quotient,remainder=divmod(scaled.numerator,scaled.denominator)
    if 2*remainder>scaled.denominator or (2*remainder==scaled.denominator and quotient&1): quotient+=1
    if not quotient: return sign<<31
    if quotient<(1<<23): return (sign<<31)|quotient
    if quotient==(1<<24): quotient>>=1; exp+=1
    if exp>127: return (sign<<31)|0x7F800000
    return (sign<<31)|((max(exp,-126)+127)<<23)|(quotient-0x800000)
def add32(a,b):
    ma,mb=a&0x7FFFFFFF,b&0x7FFFFFFF
    if ma>0x7F800000 or mb>0x7F800000: return 0x7FFFFFFF
    if ma==mb==0x7F800000 and (a^b)>>31: return 0x7FFFFFFF
    if ma==0x7F800000: return a
    if mb==0x7F800000: return b
    return round32(finite(a,32)+finite(b,32),int(a==b==0x80000000))
def div32(a,b):
    ma,mb=a&0x7FFFFFFF,b&0x7FFFFFFF; sign=(a^b)&0x80000000
    if ma>0x7F800000 or mb>0x7F800000 or ma==mb and ma in (0,0x7F800000): return 0x7FFFFFFF
    if ma==0x7F800000 or mb==0: return sign|0x7F800000
    if ma==0 or mb==0x7F800000: return sign
    return round32(finite(a,32)/finite(b,32),sign>>31)
def fix32(bits):
    if bits&0x7FFFFFFF>=0x4F000000: return 0x80000000 if bits>>31 else 0x7FFFFFFF
    value=finite(bits,32); result=abs(value.numerator)//value.denominator
    return (-result if value<0 else result)&0xFFFFFFFF
def sqrt64(bits):
    magnitude=bits&0x7FFFFFFFFFFFFFFF
    if bits>>63 and magnitude or magnitude>0x7FF0000000000000: return bits|0x7FF8000000000000,33
    if magnitude in (0,0x7FF0000000000000): return bits,123
    value=finite(bits,64); exp=exponent(value)//2; scaled=value/power(2*(exp-52))
    n,d=scaled.numerator,scaled.denominator; root=isqrt(n//d)
    difference=4*n-(2*root+1)**2*d
    if difference>0 or difference==0 and root&1: root+=1
    if root==1<<53: root>>=1; exp+=1
    return ((exp+1023)<<52)|(root-(1<<52)),123

def main():
    rng=random.Random(0x48474650)
    special=[0,0x80000000,1,0x80000001,0x7FFFFF,0x800000,0x3F000000,0x3F800000,0xBF800000,
             0x40000000,0x4EFFFFFF,0x4F000000,0x7F7FFFFF,0x7F800000,0xFF800000,0x7F800001,0xFFC12345]
    pairs=[(a,b) for a in special for b in special]+[(rng.getrandbits(32),rng.getrandbits(32)) for _ in range(300)]
    integers=[0,1,(1<<24)-1,(1<<24)+1,(1<<31)-1,1<<31,(1<<63)-1,1<<63,(1<<64)-1]+[rng.getrandbits(64) for _ in range(100)]
    square=[0,1,0x8000000000000000,0x3FF0000000000000,0x4000000000000000,0xC006000000000000,
            0x7FEFFFFFFFFFFFFF,0x7FF0000000000000,0xFFF0000000000000,0x7FF0000000000001,0xFFF8123456789ABC]
    square += [rng.getrandbits(64) for _ in range(200)]
    out=['// Generated exact rational/integer goldens; do not edit.']
    out+=['static const struct { u32 a,b,add,divide,fix; } floatCases[]={']
    out += ['{'+','.join(f'0x{x:08X}u' for x in (a,b,add32(a,b),div32(a,b),fix32(a)))+'},' for a,b in pairs]
    out+=['};','static const struct { u64 value; u32 signed64,unsigned64,signed32; } integerFloatCases[]={']
    for value in integers:
        signed=value-(1<<64) if value>>63 else value; small=value&0xFFFFFFFF; small=small-(1<<32) if small>>31 else small
        out.append(f'{{0x{value:016X}ULL,0x{round32(Fraction(signed)):08X}u,0x{round32(Fraction(value)):08X}u,0x{round32(Fraction(small)):08X}u}},')
    out+=['};','static const struct { u64 value,result; int error; } squareCases[]={']
    for value in square:
        result,error=sqrt64(value);out.append(f'{{0x{value:016X}ULL,0x{result:016X}ULL,{error}}},')
    out+=['};']
    path=ROOT/'ios/build-original-host/include/hg_float_checks.h';path.parent.mkdir(parents=True,exist_ok=True);path.write_text('\n'.join(out)+'\n')
    print(f'Prepared {len(pairs)} float pairs, {len(integers)} integer conversions and {len(square)} square-root goldens.')

if __name__=='__main__': main()
