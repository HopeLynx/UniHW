#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

//Деление для отрицательных чисел, вспомогательная функция, используется для вычислений связаных с системой счисления
int my_div(int num, int diver)  {
    if ((num < 0) && (num % diver))
        return num / diver - 1;
    else
        return num / diver;
}

//Взятие по модулю для отрицательных чисел , вспомогательная функция, используется для вычислений связаных с системой счисления
int my_mod(int num, int diver)  {
    if ((num < 0) && (num % diver))
            return num % diver + diver;
    else
        return num % diver;
}

// РАБОТАЕТ с системами счисления <= 10

class Big_Integer {
private:
    vector <int> slices;
    int sign;

    //Настроиваемые параметры хранения большого числа
    //Количество символов в строке, которые станут одним "куском"
    static const int BASE = 9;
    //Зависит от BASE (BASE_N = N^BASE), используется при нормализации
    static const int BASE_N = 1000 * 1000 * 1000;

    // пример для того чтобы работало для двоичной системы счисления
    //    static const int BASE = 1;
    //    static const int BASE_N = 2;

    //Служебные функции, делают вычисление “за кадром, для упрощения обработки промежуточных результатов”
    Big_Integer _plus(Big_Integer &num);
    Big_Integer _minus(Big_Integer &num);
    Big_Integer _multi(Big_Integer &num);
    Big_Integer _simple_multi(Big_Integer &num);
    void _normalization_with_signs();
    void _normalization_slices();
    void _resize(int newsize);

public:
    Big_Integer operator + (Big_Integer &num);
    Big_Integer operator - (Big_Integer &num);
    Big_Integer operator * (Big_Integer);
    bool operator == (Big_Integer);
    bool operator != (Big_Integer);

    friend ostream & operator << (ostream &os, Big_Integer &num);

    int getBASE() {return this->BASE;}

    //Конструктор, строку конвертирует в большое число
    Big_Integer(string str) {
        int i;
        if (BASE != 1) {
            for (i = str.size() - BASE; i >= BASE - 1; i -= BASE) {
                slices.push_back(stoi(str.substr(i, BASE)));
            }
        }
        else {
            for (i = str.size() - BASE; i >= BASE; i -= BASE) {
                slices.push_back(stoi(str.substr(i, BASE)));
            }
        }
        if (str[0] == '-') {
            sign = -1;
            if (i + BASE - 1 != 0) {
                slices.push_back(stoi(str.substr(1, i + BASE - 1)));
            }
        }
        else {
            sign = 1;
            slices.push_back(stoi(str.substr(0, i+BASE)));
        }
    }

    //Конструктор без аргументов - "пустое" положительное число
    Big_Integer() {sign = 1;}
};

//Изменение размера массива
void Big_Integer::_resize(int newSize) {slices.resize(newSize);}

/*
 * Функция нормализует большое число так, чтобы
 * во всех кусках лежали BASE-разрядные числа
*/
void Big_Integer::_normalization_slices() {
    int over = 0;
    for (unsigned int i = 0; i < slices.size() - 1; i++) {
        slices[i] += over;
        over = my_div(slices[i], BASE_N);
        slices[i] = my_mod(slices[i], BASE_N);
    }
    slices[slices.size() - 1] += over;
    if (slices[slices.size() - 1] / BASE_N) {
        over = my_div(slices[slices.size() - 1], BASE_N);
        slices[slices.size() - 1] = my_mod(slices[slices.size() - 1], BASE_N);
        slices.push_back(over);
    }
    return;
}

//Функция нормализует большое число для печати так,
//чтобы все куски были положительными, но sign = -1(если число отрицательное)
void Big_Integer::_normalization_with_signs() {
    if (slices[slices.size() - 1] < 0) {
        sign = -sign;
        slices[0] = BASE_N - slices[0];
        for (unsigned int i = 1; i < slices.size(); i++) {
            slices[i] = (BASE_N - slices[i] - 1) % BASE_N;
        }
    }
    //Убираем из числа нулевые кусочки
    int i = slices.size() - 1;
    while (slices[i] == 0) {
        if (i == 0) {
            sign = 1;
            return;
        }
        slices.pop_back();
        i--;
    }
    return;
}

//Функция сложения
Big_Integer Big_Integer::_plus(Big_Integer &num) {
    Big_Integer res;
    res.sign = this->sign;
    for (unsigned int i = 0; i < this->slices.size(); i++) {
        res.slices.push_back(this->slices[i] + num.slices[i]);
    }
    return res;
}

//Функция вычитания
Big_Integer Big_Integer::_minus(Big_Integer &num) {
    Big_Integer res;
    res.sign = this->sign;
    for (unsigned int i = 0; i < this->slices.size(); i++) {
        res.slices.push_back(this->slices[i] - num.slices[i]);
    }
    return res;
}

//Оператор +, выполняет корректное сложение больших чисел
Big_Integer Big_Integer::operator + (Big_Integer &num) {
    Big_Integer res;
    if (this->slices.size() > num.slices.size()) {
        num._resize(slices.size());
    }
    else {
        (*this)._resize(num.slices.size());
    }
    //Выполняем операцию в зависимости от знаков чисел
    if (sign == num.sign) {
        res = (*this)._plus(num);
    }
    else {
        res = (*this)._minus(num);
    }
    //Нормализуем результат
    res._normalization_slices();
    return res;
}

//Оператор -, выполняет корректное вычитание
Big_Integer Big_Integer::operator - (Big_Integer &num) {
    Big_Integer res;
    if (this->slices.size() > num.slices.size()) {
        num._resize(slices.size());
    }
    else {
        (*this)._resize(num.slices.size());
    }
    //В зависимости от знаков, выполняем нужное действие
    if (sign != num.sign) {
        res = (*this)._plus(num);
    }
    else {
        res = (*this)._minus(num);
    }
    //Нормализуем результат
    res._normalization_slices();
    return res;
}

//Функция умножения больших чисел
Big_Integer Big_Integer::_simple_multi(Big_Integer &num) {
    Big_Integer result;

    result._resize(2 * this->slices.size());
    result.sign = this->sign;

    for (unsigned int i = 0; i < this->slices.size(); i++) {
        for (unsigned int j = 0; j < num.slices.size(); j++) {
            result.slices[i + j] += this->slices[i] * num.slices[j];
        }
    }

    result._normalization_slices();
    return result;
}

//Функция приводит большие числа к нужному размеру
Big_Integer Big_Integer::_multi(Big_Integer &num) {
    auto maxSize = max(this->slices.size(), num.slices.size());
    unsigned int newSize = 1;
    while (newSize < maxSize) {
        newSize *= 2;
    }
    (*this)._resize(newSize);
    num._resize(newSize);
    return (*this)._simple_multi(num);
}

Big_Integer Big_Integer::operator * (Big_Integer num) {
    Big_Integer result;
    result = (*this)._multi(num);
    result._normalization_slices();
    if (this->sign == num.sign) {
        result.sign = 1;
    }
    else {
        result.sign = -1;
    }
    return result;
}

bool Big_Integer::operator == (Big_Integer num) {
    num._normalization_slices();
    num._normalization_with_signs();
    (*this)._normalization_slices();
    (*this)._normalization_with_signs();
    if (this->sign == num.sign) {
        //Если числа разного размера, то они не равны
        if (this->slices.size() != num.slices.size()) {
            return false;
        }
        //Сравнить покусочно
        for (int i = this->slices.size() - 1; i >= 0; i--) {
            if (this->slices[i] != num.slices[i]) {
                return false;
            }
        }
        return true;
    }
    //Если знаки не равны, то и числа не равны
    else {
        return false;
    }
}

bool Big_Integer::operator != (Big_Integer num) {
   return !(*this == num);
}

//Перегрузка оператора << для вывода в поток
ostream & operator << (ostream &os, Big_Integer &num) {
    num._normalization_with_signs();
    if (num.sign == -1) {
        os << '-';
    }
    os << num.slices[num.slices.size() - 1];
    for (int i = num.slices.size() - 2; i >= 0; i--) {
        os << setw(num.getBASE()) << setfill('0') << num.slices[i];
    }
    return os;
}

int main() {
    Big_Integer n1("-200000000000000000000000000000000000000000000000000000000000010");
    Big_Integer n2("-20");
    Big_Integer n3 = n1 + n2;
    cout << n3 << endl;

    return 0;
}
