#include "calculator.hpp"
#include <cctype>       // for isdigit()
#include <string>       // for std::string
#include <algorithm>    // for reverse()
#include <sstream>      // optional sign

// Checks if string s is a valid double
bool isValidDouble(const std::string &s) {
    int i = 0, len = s.length();
    if (len == 0) return false;

    // handle optional sign
    if (s[i] == '+' || s[i] == '-') i++;

    bool hasDigits = false, hasDot = false;

    // check each char
    for (; i < len; ++i) {
        if (isdigit(s[i])) hasDigits = true;
        else if (s[i] == '.') {
            if (hasDot) return false;  // only 1 dot allowed
            hasDot = true;
        } else return false;  // invalid char
    }

    if (!hasDigits) return false;
    if (hasDot && s.back() == '.') return false; // can't end in dot

    return true;
}

// splits a string into integer and fractional parts
std::pair<std::string, std::string> splitParts(const std::string &s) {
    size_t dot = s.find('.');
    if (dot == std::string::npos)
        return {s, ""}; // no decimal
    return {s.substr(0, dot), s.substr(dot + 1)};
}

// removes sign and tells if it's negative
std::string stripSign(const std::string &s, bool &isNegative) {
    isNegative = false;
    if (s[0] == '-') { // if it has the sign then it's negative
        isNegative = true;
        return s.substr(1);
    } else if (s[0] == '+') { // if it has + then its positive
        return s.substr(1);
    }
    return s;
}

// adds two unsigned integer strings
std::string addStrings(const std::string &a, const std::string &b) {
    int i = a.size() - 1, j = b.size() - 1, carry = 0;
    std::string result;

    while (i >= 0 || j >= 0 || carry) {
        int digitA = (i >= 0 ? a[i--] - '0' : 0);
        int digitB = (j >= 0 ? b[j--] - '0' : 0);
        int sum = digitA + digitB + carry; // sum the digits
        result += (sum % 10) + '0';
        carry = sum / 10; // compute the carry
    }

    std::reverse(result.begin(), result.end());
    return result;
}

// subtracts b from a (assumes a >= b)
std::string subtractStrings(const std::string &a, const std::string &b) {
    int i = a.size() - 1, j = b.size() - 1, borrow = 0;
    std::string result;

    while (i >= 0) {
        int digitA = a[i] - '0' - borrow;
        int digitB = (j >= 0 ? b[j--] - '0' : 0);

        if (digitA < digitB) {
            digitA += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }

        result += (digitA - digitB) + '0';
        i--;
    }

    // remove leading zeros
    while (result.length() > 1 && result.back() == '0')
        result.pop_back();

    std::reverse(result.begin(), result.end());
    return result;
}

// compares two numeric strings, returns <0 if a<b, 0 if equal, >0 if a>b
int compareStrings(const std::string &a, const std::string &b) {
    std::string A = a, B = b;
    while (A.length() < B.length()) A = "0" + A;
    while (B.length() < A.length()) B = "0" + B;
    return A.compare(B);
}

// adds fractional parts, sets carry if needed
std::string addFractionParts(std::string a, std::string b, int &carry) {
    while (a.size() < b.size()) a += '0';
    while (b.size() < a.size()) b += '0';

    int i = a.size() - 1;
    std::string result;

    while (i >= 0) {
        int sum = (a[i] - '0') + (b[i] - '0') + carry;
        result += (sum % 10) + '0';
        carry = sum / 10;
        i--;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

// subtracts fractional parts, sets borrow if needed
std::string subtractFractionParts(std::string a, std::string b, int &borrow) {
    while (a.size() < b.size()) a += '0';
    while (b.size() < a.size()) b += '0';

    int i = a.size() - 1;
    std::string result;

    while (i >= 0) {
        int digitA = a[i] - '0' - borrow;
        int digitB = b[i] - '0';
        if (digitA < digitB) {
            digitA += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result += (digitA - digitB) + '0';
        i--;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

// adds two double strings (handles sign, decimal)
std::string addDoubles(const std::string &rawA, const std::string &rawB) {
    bool negA = false, negB = false;
    std::string a = stripSign(rawA, negA); // clean signs
    std::string b = stripSign(rawB, negB);

    auto [intA, fracA] = splitParts(a); // split into parts
    auto [intB, fracB] = splitParts(b);

    while (fracA.size() < fracB.size()) fracA += '0'; // normalize decimals
    while (fracB.size() < fracA.size()) fracB += '0';

    std::string fracResult;
    std::string intResult;
    int carryOrBorrow = 0;

    // case 1: same sign -> normal add
    if (negA == negB) {
        fracResult = addFractionParts(fracA, fracB, carryOrBorrow);
        intResult = addStrings(intA, intB);
        if (carryOrBorrow) intResult = addStrings(intResult, "1");
        if (negA) intResult = "-" + intResult;
    } 

    // case 2: different signs -> subtract
    else {
        bool resultNegative;
        int cmpInt = compareStrings(intA, intB);
        int cmpFrac = compareStrings(fracA, fracB);

        std::string bigInt, smallInt, bigFrac, smallFrac;

        if ((cmpInt > 0) || (cmpInt == 0 && cmpFrac >= 0)) {
            bigInt = intA; bigFrac = fracA;
            smallInt = intB; smallFrac = fracB;
            resultNegative = negA;
        } else {
            bigInt = intB; bigFrac = fracB;
            smallInt = intA; smallFrac = fracA;
            resultNegative = negB;
        }

        fracResult = subtractFractionParts(bigFrac, smallFrac, carryOrBorrow);
        if (carryOrBorrow) bigInt = subtractStrings(bigInt, "1");
        intResult = subtractStrings(bigInt, smallInt);
        if (resultNegative) intResult = "-" + intResult;
    }


    // final output
    if (!fracResult.empty() && fracResult.find_first_not_of('0') != std::string::npos)
        return intResult + "." + fracResult;
    return intResult;
}
