#include <iostream>
//#include <string>
#include <string.h>

using namespace std;

void Error(int Code) {
    printf("Error: %d\n", Code);
    exit(1);
}

class BigInt
{
    string value;
    int lenght;
    bool isNeg;
public:

    const string &getValue() const {return value;}
    bool getIsNeg() {return isNeg;}
    void setIsNeg(bool isNeg) {this->isNeg = isNeg;}

    // Конструкторы
    BigInt();
    BigInt(long x);
    BigInt(const string &value);
    BigInt(const BigInt& bigInt);

    BigInt &operator=(const BigInt &bigInt);
    BigInt operator+() const &&;
    BigInt operator-() const &&;

    // Арифметика
    BigInt operator+(const BigInt &bigInt) const;
    BigInt operator-(const BigInt &bigInt) const;
    BigInt operator*(const BigInt &bigInt) const;
    BigInt operator/(const BigInt &bigInt) const;
    BigInt operator%(const BigInt &bigInt) const;

    BigInt operator&();

    // Сложные функции
    BigInt pow(long n) const;

    // Сравнения //TODO Error if base1!= base2
    const bool operator==(const BigInt &bigInt) const;
    const bool operator!=(const BigInt &bigInt) const;

    const bool operator<(const BigInt &bigInt) const;
    const bool operator>(const BigInt &bigInt) const;
    const bool operator<=(const BigInt &bigInt) const;
    const bool operator>=(const BigInt &bigInt) const;

    // Визуализация и взаимодействие
    BigInt operator<<(size_t n) const;
    BigInt operator>>(size_t n) const;

    friend ostream &operator<<(ostream &stream, BigInt &bigInt); // вывод числа в выходной поток
    friend istream &operator>>(istream &stream, BigInt &bigInt); // ввод числа из входного потока
};

//TODO переделать под 2-16 системы счисления

// Конструкторы
BigInt::BigInt() {
    this->isNeg = false;
    this->value = "0";
}
BigInt::BigInt(long x) {
    this->isNeg = x < 0;
    this->value = to_string(isNeg ? -x : x);
}
BigInt::BigInt(const string &value) {
    if (!value.length()) {
        this->value = "0";
        isNeg = false;

        return;
    }

    isNeg = value[0] == '-';
    this->value = value.substr(isNeg);

    // определяем число ведущих нулей в строке
    int count = 0;
    while (this->value[count] == '0' && this->value.length() - count > 1)
        count++;

    this->value = this->value.substr(count); // удаляем ведущие нули

    // проверяем "на цифру" каждый символ строки, кидаем исключение, если есть другие символы
    for(size_t i = 0; i < this->value.length(); i++)
        if (this->value[i] < '0' || this->value[i] > '9')
            throw string("BigInt(const string &value) - string contain incorrect characters: ") + this->value;
}
BigInt::BigInt(const BigInt& bigInt) {
    this->value = bigInt.getValue();
    this->isNeg = bigInt.getIsNeg();
}

const string &BigInt::getValue() const {
    return value;
}

// получение флага отрицательности числа
const bool BigInt::getIsNeg() const {
    return isNeg;
}

void BigInt::setIsNeg(bool isNeg) {
    this->isNeg = isNeg;
}


// получение числа в степени n
BigInt BigInt::pow(long n) const {
    if (!n)
        return 1;

    if (n & 1)
        return pow(n - 1) * *this;
    else {
        BigInt tmp = pow(n / 2);
        return tmp * tmp;
    }
}

// вычисление корня n-ой степени из числа
BigInt BigInt::sqrt(long n) const {
    if (n < 2)
        throw string("BigInt::sqrt() - n must be 2 or more");

    if (isNeg && ((n & 1)== 0))
        throw string("BigInt::sqrt() - value must be positive");

    size_t length = (value.length() + 1) / 2;
    size_t index = 0;

    string v(length, '0');

    while (index < length) {
        v[index] = '9';

        while (BigInt(v).pow(n) > this->abs() && v[index] > '0')
            v[index]--;

        index++;
    }

    return isNeg ? string("-") + v : v;
}

const bool BigInt::operator==(const BigInt &bigInt)  {
    return (value == bigInt.getValue()) && (isNeg == bigInt.getIsNeg());
}
const bool BigInt::operator!=(const BigInt &bigInt)  {
    return !(*this == bigInt);
}
const bool BigInt::operator<(const BigInt &bigInt) const {
    string value2 = bigInt.getValue(); // получаем значение второго числа
    size_t len1 = value.length(); // запоминаем длину первого числа
    size_t len2 = value2.length(); // запоминаем длину второго числа

    // если знаки одинаковые, то проверяем значения
    if (isNeg == bigInt.getIsNeg()) {
        // если длины не равны
        if (len1 != len2)
            return (len1 < len2) ^ isNeg; // меньше число с меньшей длинной для положительных и с большей длиной для отрицательных

        size_t i = 0;

        // ищем разряд, в котором значения отличаются
        while (i < len1 && value[i] == value2[i])
            i++;

        // если разряд найден, то меньше число с меньшей цифрой для положительных и с большей цифрой для отрицательных, иначе числа равны
        return (i < len1) && ((value[i] < value2[i]) ^ isNeg);
    }

    return isNeg; // знаки разные, если число отрицательное, то оно меньше, если положительное, то больше
}
const bool BigInt::operator>(const BigInt &bigInt) const {
    return !(*this < bigInt || *this == bigInt);
}
const bool BigInt::operator<=(const BigInt &bigInt) const {
    return *this < bigInt || *this == bigInt;
}
const bool BigInt::operator>=(const BigInt &bigInt) const {
    return *this > bigInt || *this == bigInt;
}

// операция присваивания
BigInt &BigInt::operator=(const BigInt &bigInt) {
    value = bigInt.getValue();
    isNeg = bigInt.getIsNeg();

    return *this;
}

// унарный минус, если было отрицательным, возвращаем положительное, иначе отрицательное
BigInt BigInt::operator-() const && {
    return BigInt(isNeg ? value : string("-") + value);
}

// унарный плюс (просто копируем значение числа)
BigInt BigInt::operator+() const && {
    return BigInt(*this);
}

// умножение методом Карацубы
BigInt BigInt::karatsuba_mul(const BigInt &a, const BigInt &b) {
    string v1 = a.getValue();
    string v2 = b.getValue();

    size_t len1 = v1.length();
    size_t len2 = v2.length();
    size_t len = max(len1, len2);

    if (len1 + len2 < 9)
        return stol(a.getValue()) * stol(b.getValue());

    len += len % 2;
    size_t n = len / 2;

    BigInt Xr(len1 > n ? v1.substr(len1 - n, n) : v1);
    BigInt Xl(a >> n);
    BigInt Yr(len2 > n ? v2.substr(len2 - n, n) : v2);
    BigInt Yl(b >> n);

    BigInt P1 = karatsuba_mul(Xl, Yl);
    BigInt P2 = karatsuba_mul(Xr, Yr);
    BigInt P3 = karatsuba_mul(Xl + Xr, Yl + Yr);

    return (P1 << len) + ((P3 - P2 - P1) << n) + P2;
}


// бинарный плюс (сложение двух чисел)
BigInt BigInt::operator+(const BigInt &bigInt) const {
    bool isAddOp = !(bigInt.getIsNeg() ^ isNeg); // если знаки одинаковые, то выполняем сложение

    if (isAddOp) {
        string num2 = bigInt.getValue(); // запоминаем значение второго числа

        size_t len1 = value.length(); // запоминаем длину первого числа
        size_t len2 = num2.length(); // запоминаем длину второго числа
        size_t length = 1 + max(len1, len2);  // длина суммы равна максимуму из двух длин + 1 из-за возможного переноса разряда

        char res[length + 1]; // строковый массив для выполнения операции сложения

        res[length - 1] = res[length] = '\0';

        for (size_t i = 0; i < length - 1; i++) {
            int j = length - 1 - i;
            res[j] += ((i < len2) ? (num2[len2 - 1 - i] - '0') : 0) + ((i < len1) ? (value[len1 - 1 - i] - '0') : 0); // выполняем сложение разрядов
            res[j - 1] = res[j] / 10; // выполняем перенос в следущий разряд, если он был
            res[j] = res[j] % 10 + '0'; // оставляем только единицы от возможного переноса и превращаем символ в цифру
        }

        res[0] += '0';

        return BigInt(isNeg ? string("-") + string(res) : string(res)); // возвращаем результат, в зависимости от знака`
    }
    else
        return isNeg ? (bigInt - (-BigInt(*this))) : (*this - (-BigInt(bigInt))); // одно из чисел отрицательное, а другое положительное, отправляем на вычитание, меняя знак
}

// бинарный минус (вычитание двух чисел)
BigInt BigInt::operator-(const BigInt &bigInt) const {
    if (*this == bigInt)
        return 0; // если числа равны, то какой смысл вычитать?

    // если оба числа положительные, то выполняем вычитание
    if (!isNeg && !bigInt.getIsNeg()) {
        string value2 = bigInt.getValue(); // запоминаем значение второго числа

        size_t len1 = value.length(); // запоминаем длину первого числа
        size_t len2 = value2.length(); // запоминаем длину второго числа
        size_t length = max(len1, len2); // длина результата не превысит максимума длин чисел

        bool isNegRes = bigInt > *this; // определяем знак результата

        int a[length], b[length]; // массивы аргументов
        a[0] = b[0] = 0; // обнуляем нулевые элементы массивов

        char res[length + 1]; // строковый массив для результата
        res[length - 1] = res[length] = '\0'; // устанавливаем символ окончания строки

        int sign = (2 * isNegRes - 1); // получаем числовое значение знака результата

        for (size_t i = 0; i < length - 1; i++) {
            a[i] += (i < len1) ? (value[len1 - 1 - i] - '0') : 0; // формируем разряды
            b[i] += (i < len2) ? (value2[len2 - 1 - i] - '0') : 0; // из строк аргументов

            b[i + 1] = -isNegRes; // в зависимости от знака занимаем или не занимаем
            a[i + 1] = isNegRes- 1; // 10 у следующего разряда

            res[length - 1 - i] += 10 + sign * (b[i] - a[i]);
            res[length - 1 - i - 1]  = res[length - 1 - i] / 10;
            res[length - 1 - i] = res[length - 1 - i] % 10 + '0';
        }

        // выполняем операцию с последним разрядом
        a[length - 1] += (length - 1 < len1) * (value[0] - '0');
        b[length - 1] += (length - 1 < len2) * (value2[0] - '0');

        // записываем в строку последний разряд
        res[0] += sign * (b[length - 1] - a[length - 1]) + '0';

        return BigInt(isNegRes ? string("-") + string(res) : string(res)); // возвращаем результат, учитывая знак
    } else // если оба числа отрицательные, то меняем местами, меняем знаки и повторяем вычитание, а если знаки разные, то отправляем на сумму
        return isNeg && bigInt.getIsNeg() ? (-BigInt(bigInt) - (-BigInt(*this))) : (*this + -BigInt(bigInt));
}

// бинарная звёздочка (умножение двух чисел)
BigInt BigInt::operator*(const BigInt &bigInt) const {
    if (value == "0" || bigInt.getValue() == "0")
        return 0; // если один из множителей равен нулю, то результат равен нулю

    string value2 = bigInt.getValue(); // запоминаем значение второго числа

    size_t len1 = value.length(); // запоминаем длину первого числа
    size_t len2 = value2.length(); // запоминаем длину второго числа
    size_t length = len1 + len2 + 1; // резульат влезет в сумму длин + 1 из-за возможного переноса
    bool isNegRes = isNeg ^ bigInt.getIsNeg(); // флаг отрицательности результата - отрицательный, если числа разных знаков

    if (length < 10) { // умножаем как обычный long
        long res = stol(value) * stol(value2);
        return BigInt(isNegRes ? -res : res);
    }
    else if (length < 2000) { // умножаем в столбик
        int a[length], b[length]; // массивы аргументов и результата

        char res[length + 1]; // строковый массив для результата
        res[length] = '\0'; // устанавливаем символ окончания строки

        // заполняем массивы инверсной записью чисел (с ведущими нулями)
        for (size_t i = 0; i < length; i++) {
            a[i] = (i < len1) ? (value[len1 - 1 - i] - '0') : 0;
            b[i] = (i < len2) ? (value2[len2 - 1 - i] - '0') : 0;
            res[i] = 0;
        }

        // выполняем умножение "в столбик""
        for (size_t i = 0; i < len1; i++) {
            for (size_t j = 0; j < len2; j++) {
                res[length - 1 - (i + j)] += a[i] * b[j];
                res[length - 1 - (i + j + 1)] += res[length - 1 - (i + j)] / 10;
                res[length - 1 - (i + j)] %= 10;
            }
        }

        // переписываем результат в строку
        for (size_t i = 0; i < length; i++)
            res[length - 1 - i] += '0';

        return BigInt(isNegRes ? string("-") + string(res) : string(res)); // возвращаем результат, учитывая его знак
    } else { // умножаем по методу Карацубы
        BigInt res = karatsuba_mul(*this, bigInt);
        return isNegRes ? -BigInt(res) : res;
    }
}

// бинарный слеш (деление двух чисел)
BigInt BigInt::operator/(const BigInt &bigInt) const {
    string value1 = value;
    string value2 = bigInt.getValue(); // запоминаем значение второго числа

    if (value2[0] == '0')
        throw string("Division by zero!"); // нельзя делить на ноль, этому ещё в школе учат

    if (value[0] == '0')
        return 0; // а вот ноль делить можно на всё, кроме нуля, но смысл?

    if (value2 == "1")
        return BigInt(bigInt.getIsNeg() ? -BigInt(*this) : *this); // делить на 1 можно, но смысл?

    size_t zeroes = 0;
    while (value2[value2.length() - 1 - zeroes] == '0')
        zeroes++;

    if (zeroes >= value.length())
        return 0;

    // избавляемся от круглых чисел
    if (zeroes) {
        value1 = value1.substr(0, value1.length() - zeroes);
        value2 = value2.substr(0, value2.length() - zeroes);
    }

    bool isNegRes = isNeg ^ bigInt.getIsNeg(); // считаем знак числа

    BigInt tmp(value2);

    size_t divider_length = value2.length(); // запоминаем длину делителя
    long divider_v = divider_length > 8 ? 0 : atol(value2.c_str()); // если длина больше 8, то обнуляем long'овый делитель, иначе переводим строку в long

    size_t length = value1.length(); // получаем длину делимого
    size_t index = 0; // стартуем с нулевого индекса

    string div; // строка результата деления
    string v; // строка подчисла (которое делится на делитель в столбик)

    // формируем начальное число для деления
    while (BigInt(v) < tmp && index < length)
        v += value1[index++];

    do {
        int count = 0; // результат деления подчисла на делитель

        // если можем разделить, то делим
        if (BigInt(v) >= tmp) {
            if (divider_length > 8) { // если не входит в long, то делим с помощью вычитания
                BigInt mod = v;

                while (mod >= tmp) {
                    mod -= tmp;
                    count++;
                }

                v = mod.getValue();
            } else {
                long mod = stol(v);
                count = mod / divider_v;
                v = to_string(mod % divider_v);
            }
        }

        div += count + '0'; // если не делили, то добавили ноль к результату, иначе добавили результат дедения

        if (index <= length)
            v += value1[index++]; // формируем новое значение для подчисла
    } while (index <= length);

    return BigInt(isNegRes && div != "0" ? string("-") + div : div); // возвращаем результат учитывая знак и возможное равенство нулю
}

// бинарный процент (операция взятия остатка от деления) (полностью аналогична делению)
BigInt BigInt::operator%(const BigInt &bigInt) const {
    string value2 = bigInt.getValue();

    if (value2[0] == '0')
        throw string("Division by zero!");

    if (value[0] == '0' || value2 == "1")
        return 0;

    if (value.length() < 9 && value2.length() < 9) {
        long res = stol(value) % stol(value2);
        return isNeg ? -res : res;
    }

    BigInt tmp(value2);

    size_t divider_length = value2.length(); // запоминаем длину делителя
    long divider_v = divider_length >= 9 ? 0 : atol(value2.c_str()); // если длина больше 8, то обнуляем long'овый делитель, иначе переводим строку в long

    size_t length = value.length();
    size_t index = 0;
    BigInt mod2 = value;
    string v;

    while (BigInt(v) < tmp && index < length)
        v += value[index++];

    do {
        if (BigInt(v) >= tmp) {
            if (divider_v)
                v = to_string(stol(v) % divider_v);
            else {
                BigInt mod = v;

                while (mod >= tmp)
                    mod = mod - tmp;

                v = mod.getValue();
            }
        }

        if (index <= length) {
            mod2 = v;
            v += value[index++];
        }
    } while (index <= length);

    if (mod2.getValue() == "0")
        return 0;

    return isNeg ? -BigInt(mod2) : mod2;
}

BigInt BigInt::operator<<(size_t n) const {
    return BigInt(string(isNeg ? "-" : "") + value + string(n, '0'));
}

BigInt BigInt::operator>>(size_t n) const {
    if (n >= value.length())
        return 0;

    return BigInt(string(isNeg ? "-" : "") + value.substr(0, value.length() - n));
}


// вывод числа в выходной поток
ostream &operator<<(ostream &stream, const BigInt &bigInt) {
    if (bigInt.getIsNeg())
        stream << "-";

    return stream << bigInt.getValue();
}

// ввод числа из входного потока
istream &operator>>(istream &stream, BigInt &bigInt) {
    string value;
    stream >> value;
    bigInt = BigInt(value);

    return stream;
}



void PrintF(unsigned char I) {
    if (I < 10) printf("%d", I);
    else if (I == 10) printf("A");
    else if (I == 11) printf("B");
    else if (I == 12) printf("C");
    else if (I == 13) printf("D");
    else if (I == 14) printf("E");
    else if (I == 15) printf("F");
    else Error(100);
}

void PrintFF(unsigned char I) {
    unsigned char J = I / 16;
    PrintF(J);
    I = I % 16;
    PrintF(I);
}

void PrintArrFF(unsigned char * AAA) {

    for (int i = 31; i >= 0; i--) {
        PrintFF(AAA[i]);
        printf(" ");
    }
    printf("\n");
}


int main() {
    printf("Hello world!\n\n");
    return 0;
}
