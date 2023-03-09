/* 
 * CS:APP Data Lab 
 * 
 * <Nevin Liang; nevin>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  // taking advantage of demorgan's law
  // the not of an and is the or of the nots :)
  return ~(~x | ~y);
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  // first, since one byte is 8 bits, calculate the number of bits
  int m = (n << 3);
  // then shift x by that number of bits
  x = x >> m;
  // finally and it with the full one byte in order to get the correct value.
  return x & 0xFF;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  // shift it to the right with an arithmetic shift
  int ans = x >> n;
  // problem is there are still possible ones on the left
  // make a new var that is all left ones in the same place as ans's
  int leftOnes = ((1 << 31) >> n) << 1;
  // finally negative that to get all left 0's and rest 1's
  int leftZeros = ~leftOnes;
  // anding rids all left possible 1's
  return ans & leftZeros;
}
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n) {
  // max bit
  int m = 1 << 31;
  // the equivalent of 32 - n
  int n2 = 33 + ~n;

  // capture right n bits
  int z = x & ~(~0 << n);

  // rhs of result is right n bits shifted left 32 - n bits
  int rhs = z << n2;

  // lhs is original shifted right n bits
  int lhs = x >> n;

  // now we clear the left possible bits
  lhs = lhs & ~(((1 << 31) >> n) << 1);

  return lhs +  rhs;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
int conditional(int x, int y, int z) {
  int isy, isz;
  // we have to go from an integer x to a 0000 or a 1111
  // with that we can and it with either not y or z and end up with either value
  // 1111111 is the equivalent of -1 and 0 is just 0
  // not can transform x to either 1 or 0
  x = !x;
  // try to subtract 1 from x
  x = (x + ~0);
  // now we have true -> 11111 and false -> 00000

  // if its 11111 then we let isy be y and isz be 0
  // if its 00000 then we let isy be 0 and isz be z
  isy = y & x;
  isz = z & ~x;

  // finally or together
  return isy | isz;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  // 0 is the only number who's leftmost bit is never 1
  // whether you negate it or not
  // this should give 1------- if x is not 0 and 0------ otherwise
  int z = x | (~x + 1);
  // we want 0 if x is not 0 and 1 otherwise so we not it
  z = ~z;
  // get rid of the rest of the numbers to the right by right shifting
  z = z >> 31;
  // get rid of the numbers to the left of the last bit by ANDing with 1.
  return z & 1;
}
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) {
  // if we turn a 1 and a 0 to a 0, the number of 0's doesnt change
  // if we turn a 0 and a 0 to a 1, then the parity of # of 0's doesnt change
  // if we turn a 1 and a 1 to a 1, the number of 0's doesnt change
  // this is opposite of xor
  x = ~(x ^ (x >> 16));
  x = ~(x ^ (x >> 8));
  x = ~(x ^ (x >> 4));
  x = ~(x ^ (x >> 2));
  x = ~(x ^ (x >> 1));
  // the last digit of x, will either be 0 or 1.
  // if its 1, then number of 0's is even, if its 0, the # 0's is odd.
  // opposite of what we want
  return (~x & 1);
}
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  // 3 Cases: x = 1111..111, 01111..111, 10110101..1
  // aka -1, tmax, or random number.
  // first get rid of random number.
  int y = x ^ (x + 1);
  // y is 1111...111 in case 1 and 2 and something else otherwise
  y = ~y;
  // y is 0 in case 1 and 2 and somethign else otherwise
  y = !y;
  // y is 1 in case 1 and 2 and 0 otherwise
  // now we get rid of the -1 and make sure only tmax returns 1
  // ~x is 0 in case 1 and something else in case 2
  x = ~x;
  x = !!(x);
  // now x is 0 in case 1 and 1 in case 2

  // we want case 2 so thats x = 1 and y = 1.
  return x & y;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  // if a number like 0101 can fit in 3 bits
  // shifting it 29 bits to the left and 29 bits back
  // should give the same number
  // it wont though, because you'll end up with 111111....101
  // if the number was 101 as in -3 in 3 bits
  // 11111111....101 is still -3 so its right

  // calculate 32 - n
  int oppn = 32 + (~n + 1);
  // temp var equals x
  int y = x;
  // for later
  int same;
  // shift left then right
  y = x << oppn;
  y = y >> oppn;

  // check if the number is the same it was before
  same = x ^ y;
  // if same is all 0's then the number is the same and return true
  return !same;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  // credits for algorithm taken from lecture video
  // Professor Ghaforyfard


  int sign_x = x >> 31;

  // when x is positive or 0 its just x >> n
  // when x is negative though, we are technically calculating floor
  // by adding (1 << n) - 1 to x before right shifting n, we guarantee a ceiling
  sign_x = sign_x & ((1 << n) + ~(0));
  x += sign_x;
  return x >> n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  // just use the formula for negation in two's complement
  return ~x + 1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  // tests if x is zero or not
  int isZero = !x;
  // get the leftmost bit of x (sign bit)
  x = x >> 31;
  // make sure the number isn't 0 and is not negative
  return !(x & 1) & !isZero;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  // one nonnegative, one negative
  int opp = (!(x >> 31 & 1)) & (y >> 31 & 1);
  // both same sign
  // XOR can do this because XOR is difference
  int exor = x ^ y;
  int samesign = !(exor >> 31 & 1);

  // first calculate x - y
  int diff = x + (~y + 1);
  // test if diff is positive
  // implement same code as in isPositive haha
  int isZero = !diff;
  diff = diff >> 31;
  // either the numbers are nonnegative and negative, or their difference is positive
  return opp | (samesign & !(diff & 1) & !isZero); 
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
  // first calculate x - y
  int diff = x + (~y + 1);

  // wont overflow when y is 0 or x = y

  // the only time x - y overflows is when either

  // x = -big number and y = big number
  // the subtraction x - y will then be a big number
  // not possible for the difference to be negative because then big number will have to be 1 << 32
  
  // x = big number and y = -big number
  // the subtraction x - y will then be a negative number

  // make sure y's sign bit is same as result's which is also different from x's sign bit


  return !y | !diff | !((x ^ y) >> 31) | !((diff ^ x) >> 31);

}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */ 

int ilog2(int x) {
  // check if first 16 bits are 0 or not 0
  int result = x >> 16;
  // flag is 0 if result is 0000000 and 1 otherwise
  int flag = !!result;

  // if flag is 1 then we multiply it by 16 so next time we check the first 24 bits
  int total = flag << 4;

  // same thing with first 8 or 24 bits
  // 8 if there was a 1 in the first 16 and 24 if not
  result = x >> (8 + total);
  flag = !!result;
  total += flag << 3;

  // same thing here. 16 + 8 + 4 or 16 + 8 - 4 or 16 - 8 + 4 ... etc
  result = x >> (4 + total);
  flag = !!result;
  total += flag << 2;

  // same
  result = x >> (2 + total);
  flag = !!result;
  total += flag << 1;

  // same
  result = x >> (1 + total);
  flag = !!result;
  total += flag;

  // wasn't sure if +/- 1 error here so i just plugged in a bunch of numbers
  // as test cases and miraculously you don't need to +/- 1 haha
  return total;

}
