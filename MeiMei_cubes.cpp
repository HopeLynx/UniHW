#include <iostream>
#include <vector>
#include <iomanip> // setprecision

using namespace std;

//бросают n кубиков с m сторон, найти сколько случаев = f

// Возведение в степень
int pow_int(int x, int y){
        int  tmp = 1;
        for (int i=0;i<y;i++)	tmp *= x;
        return tmp;
    }

// Подсчёт кол-ва действий на каждом уровне
int count_steps(int options, int up_to, int level){
    return options/pow_int(up_to,level);
}

// Сдвиг границ области значений
void offset_borders(int& min, int& max, int offset){
    min += offset;
    max += offset;
}

// Проверка входа числа в границы области значений
int check_ans(int min, int max, int searching_for){
    if (searching_for >= min && searching_for <= max)  return 1;
    return 0;
}

// Функция вычисления ответа на поставленный вопрос
int solvation(int number_of_cubes, int up_to, int searching_for){

    // ideas....

    // кол-во размещений = факториал(н) / факториал(н-м)
    // м - кол-во кубов , н - кол-во вариантов
    //long int cnt_of_options = factorial(up_to)/factorial(up_to);
    //кол-во строк в таблице = n в степени кол-ва переменных


    // если смена на уровень выше - то прибавляем 1 или
    // если смена на н уровней выше - то прибавляем 1-((кол-во уровней-1) * (up_to-1))
    // кол-во уровней  = 2 если 133 становится 211
    // кол-во уровней = 1 если  113 становится 121


    int ans = 0;
    if (searching_for < number_of_cubes || searching_for > up_to * number_of_cubes) {return 0;}

    int min = number_of_cubes; int max = min - 1 + up_to;

    vector <int> cnt_steps;
    int options = pow_int(up_to,number_of_cubes);
    // шаг = up_to
    //переходов на уровень выше всегда count steps на прошлом уровне / up_to
    for (int i = 1; i < number_of_cubes;i++){cnt_steps.push_back(count_steps(options,up_to,i));}

    int size_steps = cnt_steps.size();

    //init_check
    ans += check_ans(min,max,searching_for);
    for (int j = 1; j < cnt_steps[0]; j++){
        offset_borders(min, max, 1);
        //Уровни считаются с конца , первый в конце ,последний в начале
        for (int i = size_steps-1; i > 0  ; i--){
            if (j % cnt_steps[i] == 0) offset_borders(min, max, -(up_to-1));
        }
        ans += check_ans(min,max,searching_for);
    }
    return ans;
}

// Печать полученного значения
void print_ans(int answer){cout<< "Number of matching comparisons:" << answer << endl;}

// Печать относительного числа полученного кол-ва к числу всевозможных значений
void print_approximate(int ans,int number_of_cubes,int up_to){
    int options = pow_int(up_to,number_of_cubes);
    float appr = ((1.0*ans)/(1.0*options))*100.0;
    cout << "It is approximately " << setprecision(6) << appr << "% of all the options (" << ans<<" / " << options<<")\n";
}

int main()
{
    int number_of_cubes = 4;
    cout << "Enter quantity of cubes: "; cin >> number_of_cubes; cout << endl;

    int up_to = 6;
    cout << "Enter number of cube sides: "; cin >> up_to; cout << endl;

    int searching_for_min = 11;
    cout << "Enter desired value of cubes sum: "; cin >> searching_for_min; cout << endl;

    int range = 0;
    cout << "Enter view range to check: (0 for no range) "; cin >> range; cout << endl;

    for (int i = searching_for_min; i <= searching_for_min + range; i++){
        int answer = solvation(number_of_cubes,up_to,i);
        cout << "sum = " << i << endl;
        print_ans(answer);
        print_approximate(answer,number_of_cubes,up_to);
        cout << endl;
    }


    return 0;
}
