#include <iostream>
#include <vector>

using namespace std;

//бросают n кубиков с m сторон, найти сколько случаев = f 

// Возведение в степень
int pow_int(int x, int y){
        int  tmp = 1;
        for (int i=0;i<y;i++)	tmp *= x;
        return tmp;
    }

// Печать массива
void print_array(int a[], int N){
    for (int i = 0; i < N; i++) printf("%d ", a[i]);cout<<endl;
}

// Вычисление факториала
long int factorial (long int n)
{
  return (n < 2) ? 1 : n * factorial (n - 1);
}

// тупой метод
void print_options(int num_of_cubes, int up_to){
    int* arr = new int [num_of_cubes];
    // инициавлизация массива кубиков
    for (int i =0; i<num_of_cubes; i++){
        arr[i] = 1;
    }
    // печать и итерация
    for (int i =num_of_cubes-1 ; i > -1; i--){
        for (int j = arr[i]; j< up_to; j++){
            arr[i] = j;
        }
    }



    delete [] arr;
}

int solvation(int num_of_cubes, int up_to, int eq){
    long int min = num_of_cubes * 1;
    long int max = num_of_cubes * up_to;
    //сравнение с мин и макс
    if (eq < min || eq > max) return 0;

    // кол-во размещений = факториал(н) / факториал(н-м)
    // м - кол-во кубов , н - кол-во вариантов
    //long int cnt_of_options = factorial(up_to)/factorial(up_to);

    //кол-во строк в таблице = n в степени кол-ва переменных
    long int cnt_of_options = pow_int(up_to,num_of_cubes);
    // инициализация массива кол-ва вариантов на уровнях
    long int* arr = new long int [num_of_cubes];
    arr[0] = cnt_of_options;
    for (int i = 1; i < num_of_cubes; i++){
        arr[i] = arr[i-1] / up_to;
    }
    // min,min+up_to-(up_to-1),....min+up_to-1,min+up_to  = базис


    // если смена на уровень выше - то прибавляем 1 или
    // если смена на н уровней выше - то прибавляем 1-((кол-во уровней-1) * (up_to-1))
    // кол-во уровней  = 2 если 133 становится 211
    // кол-во уровней = 1 если  113 становится 121







    return 0;
}

//уровни считаются с конца , первый в конце ,последний в начале

int count_steps(int options, int up_to, int level){
    return options/pow_int(up_to,level);
}

void offset_borders(int& min, int& max, int offset){
    min += offset;
    max += offset;
}

int check_ans(int min, int max, int searching_for){
    if (searching_for >= min && searching_for <= max)  return 1;
    return 0;
}

void print_ans(int ans){cout<< "Number of matching comparisons:" << ans;}

int main()
{
    int n = 4;
    cout << "Enter quantity of cubes: "; cin >> n; cout << endl;
    int up_to = 6;
    cout << "Enter number of cube sides: "; cin >> up_to; cout << endl;
    int searching_for = 23;
    cout << "Enter quantity of cubes: "; cin >> n; cout << endl;
    int ans = 0;
    if (searching_for < n || searching_for > up_to * n) {print_ans(ans);return 0;}

    int min = n; int max = min - 1 + up_to;

    vector <int> cnt_steps;
    int options = pow_int(up_to,n);
//    cout << options << endl;

    // шаг = up_to
    for (int i = 1; i < n;i++){
        cnt_steps.push_back(count_steps(options,up_to,i));
        cout << cnt_steps[i-1] << endl;
    }

    int size_steps = cnt_steps.size();
//    cout << size_steps<<endl;
    cout << min <<"   "<< max << endl;
    //cout << cnt_steps[2]<<endl;

    //init_check
    ans += check_ans(min,max,searching_for);
    for (int j = 1; j < cnt_steps[0]; j++){
//    cout<<"lul";

        offset_borders(min, max, 1);
        for (int i = size_steps-1; i > 0  ; i--){
            if (j % cnt_steps[i] == 0) offset_borders(min, max, -(up_to-1));
        }
        cout << min <<"   "<< max << endl;
        ans += check_ans(min,max,searching_for);

    }
    print_ans(ans);
    // уровней = n
    //переходов на уровень выше всегда count steps на прошлом уровне / up_to
    return 0;
}
