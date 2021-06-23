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
    vector<int> slices;
    int sign;

    //Настроиваемые параметры хранения большого числа
    //Количество символов в строке, которые станут одной "чанкой"
    static const int BASE = 9;
    //Зависит от BASE (BASE_N = 10^BASE), используется при нормализации
    static const int BASE_N = 1000 * 1000 * 1000;

    // пример для того чтобы работало для двоичной системы счисления
    //    static const int BASE = 1;
    //    static const int BASE_N = 2;

    //Служебные функции, делают вычисление “за кадром, для упрощения обработки промежуточных результатов”
    Big_Integer _plus(Big_Integer &a);
    Big_Integer _minus(Big_Integer &a);
    Big_Integer _multi(Big_Integer &);
    Big_Integer _simplemulti(Big_Integer &);
    void _normalizationSigns();
    void _normalizationChunks();
    void _resize(int newsize);

public:
    Big_Integer operator + (Big_Integer &num);
    Big_Integer operator - (Big_Integer &num);
    Big_Integer operator * (Big_Integer);
    bool operator == (Big_Integer);
    bool operator != (Big_Integer);
    bool operator <= (Big_Integer);
    bool operator >= (Big_Integer);
    bool operator < (Big_Integer);
    bool operator > (Big_Integer);

    friend ostream & operator << (ostream &os, Big_Integer &num);

    int getBASE() {
        return this->BASE;
    }

    //Конструктор, строку конвертирует в большое число
    Big_Integer(string str) {
        int i;
        if (BASE != 1) {
            //Записываем с конца по BASE символов строки в чанки
            for (i = str.size() - BASE; i >= BASE - 1; i -= BASE) {
                slices.push_back(stoi(str.substr(i, BASE)));
            }
        }
        else {
            for (i = str.size() - BASE; i >= BASE; i -= BASE) {
                slices.push_back(stoi(str.substr(i, BASE)));
            }
        }

        //Дошли до начала строки, тут ищем знак и записываем последнюю чанку
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
    Big_Integer() {
        sign = 1;
    }
};

//Изменение размера массива с чанками
void Big_Integer::_resize(int newSize) {
    slices.resize(newSize);
}

/*
 * Функция нормализует большое число так, чтобы
 * во всех чанках лежали BASE-разрядные числа
*/
void Big_Integer::_normalizationChunks() {
    int over = 0; //"Лишнее", которое будет кочевать в следующие чанки
    for (int i = 0; i < slices.size() - 1; i++) {
        slices[i] += over; //Прибавляем привесок от прошлых чанок
        over = my_div(slices[i], BASE_N); //Считаем перевес в текущей чанке
        slices[i] = my_mod(slices[i], BASE_N); //Выравниваем чанку по базе
    }

    //Прибавляем перевес к последней чанке
    slices[slices.size() - 1] += over;
    //Обрабатываем перевес в последней чанке
    if (slices[slices.size() - 1] / BASE_N) {
        over = my_div(slices[slices.size() - 1], BASE_N);
        slices[slices.size() - 1] = my_mod(slices[slices.size() - 1], BASE_N);
        slices.push_back(over); //Создаем нову чанку с остатками
    }

    return;
}

//Функция нормализует большое число для печати так,
//чтобы все чанки были положительными, но sign = -1(если число отрицательное)
void Big_Integer::_normalizationSigns() {
    //Если в последней чанке отрицательное число
    if (slices[slices.size() - 1] < 0) {
        sign = -sign; //Меняем знак числа
        slices[0] = BASE_N - slices[0]; //Нормализуем первую чанку
        for (unsigned int i = 1; i < slices.size(); i++) {
            slices[i] = (BASE_N - slices[i] - 1) % BASE_N; //Нормализуем ост. чанки
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

    //Приводим размер чанок обоих чисел
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
    res._normalizationChunks();
    return res;
}

//Оператор -, выполняет корректное вычитание
Big_Integer Big_Integer::operator - (Big_Integer &num) {
    Big_Integer res;

    //Приводим размер чанок
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
    res._normalizationChunks();
    return res;
}

bool Big_Integer::operator == (Big_Integer num) {
    num._normalizationChunks();
    num._normalizationSigns();
    (*this)._normalizationChunks();
    (*this)._normalizationSigns();

    if (this->sign == num.sign) {
        //Если числа разного размера, то они не равны
        if (this->slices.size() != num.slices.size()) {
            return false;
        }

        //Сравнить почанково
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
    num._normalizationChunks();
    num._normalizationSigns();
    (*this)._normalizationChunks();
    (*this)._normalizationSigns();

    if (this->sign == num.sign) {
        //Если числа разного размера, то они не равны
        if (this->slices.size() != num.slices.size()) {
            return true;
        }

        //Сравнить почанково
        for (int i = this->slices.size() - 1; i >= 0; i--) {
            if (this->slices[i] != num.slices[i]) {
                return true;
            }
        }
        return false;
    }
    //Если знаки не равны, то и числа не равны
    else {
        return true;
    }
}

//Перегрузка оператора << для вывода в поток
ostream & operator << (ostream &os, Big_Integer &num) {
    num._normalizationSigns();

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
//    BigNumber n1("-200000000000000000000000000000000000000000000000000000000000010");
//    BigNumber n2("-20");
    Big_Integer n1("1010");
    Big_Integer n2("1");
    Big_Integer n3 = n1 + n2;
    cout << n3 << endl;

    return 0;
}
