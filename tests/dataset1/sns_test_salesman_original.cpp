#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
//#include <algorithm>
//#include <numeric>
#include <fstream>
#include <sstream>
#include <cstring>

#include "mpi.h"

#define SUCCESS 0
#define ERROR 1
#define N_BORDER 11

using namespace std;

// Функция чтения матрицы из файла filename в строку data
int ReadAll(const char* filename, string& data, MPI_Comm comm)
{
    int errValue;
    char* buf;
    MPI_Offset offset;
    MPI_Status status;
    MPI_File fd;

    errValue = MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fd);

    // Проверка выполнения чтения файла
    if (errValue != MPI_SUCCESS)
    {
        fprintf(stderr, "File error: %s\n", strerror(errno));
        return ERROR;
    }

    MPI_File_get_size(fd, &offset);

    // Проверка файла на пустоту
    if (offset == 0)
    {
        fprintf(stderr, "Format error: empty file \n");
        return ERROR;
    }

    // Чтение в массив char
    buf = new char[offset + 1];

    MPI_File_read_all(fd, buf, offset, MPI_CHAR, &status);

    // Окончание массива
    buf[offset] = '\0';

    data = buf;

    delete[] buf;
    MPI_File_close(&fd);
    return SUCCESS;
}

 // Получение размера матрицы, которая считана из файла в строку
 // data - строка, которая содержит матрицу, считанную из файла
 // size - размер матрицы
int GetSizeMatrix(const string& data, int* size)
{
    string line;
    stringstream stream(data);

    // Чтение первой строки
    stream >> *size;
    
    // Проверка формата
    if (stream.fail() || (*size > N_BORDER) || (*size < 0))
    {
        fprintf(stderr, "Format error: String 1 symbol 1\n");
        return ERROR;
    }

    // Проверка на нулевое значение
    if (*size == 0)
    {
        fprintf(stderr, "Format error: empty N\n");
        return ERROR;
    }

    // Проверка наличия в строке матрицы А
    getline(stream, line);

    if(stream.eof())
    {
        fprintf(stderr, "Format error: empty A\n");
        return ERROR;
    }

    return SUCCESS;
}

// Преобразование строки text в матрицу A размера nxn
int ParseMatrix(const string& text, float* A, int n)
{
    string line;
    float buf;

    stringstream stream(text);

    //  Пропуск 1-й строки - в ней хранится размер
    getline(stream, line);

    // Чтение последующих строк
    for (int i = 0; i < n; ++i) 
    {
        getline(stream, line);

        // Проверка соответствия количества строк размеру
        if (i != (n - 1) && stream.eof()) 
        {
            fprintf(stderr, "Format error: Inconsistency\n");
            return ERROR;
        }

        stringstream sStream(line);

        for (int j = 0; j < n; ++j) {
            string number;

            // Считывание одного числа в строке
            getline(sStream, number, ' ');

            stringstream stream3(number);

            // Проверка формата считываемого значения
            if ((stream3 >> buf).fail() || (buf < 0)) 
            {
                fprintf(stderr, "Format error: String %d, symbol %d\n", i + 2, j + 1);
                return ERROR;
            }

            A[i * n + j] = buf;
        }

        // Проверка соответствия длины строки размеру
        if (!sStream.eof()) 
        {
            fprintf(stderr, "Format error: Inconsistency\n");
            return ERROR;
        }
    }

    // Проверка соответствия количества строк размеру
    if (!stream.eof()) 
        {
            fprintf(stderr, "Format error: Inconsistency\n");
            return ERROR;
        }

    return SUCCESS;
}

// Компаратор для qsort (int, т.к. сортируется перестановка)
int compare(const void* a, const void* b)
{
    const int* x = (int*)a;
    const int* y = (int*)b;

    if (*x > *y)
        return 1;
    else if (*x < *y)
        return -1;

    return 0;
}

// Вычисление факториала от целого числа n
int factorial(int n)
{
    int f = 1;
    for (int i = 1; i <= n; ++i) f *= i;
    return f;
}

// Функция, позволяющая поменять местами элементы с номерами i и j
// в перестановке permutation
void swapEl(int* permutation, int i, int j)
{
    int buf = permutation[i];
    permutation[i] = permutation[j];
    permutation[j] = buf;
}

// Выделение части перестановки permutation размером size,
// начиная с i-го элемента до конца перестановки
int* createRight(int* permutation, int i, int size)
{
    int* right = new int[size - i];
    int iter = 0;
    for (int j = i; j < size; ++j)
    {
        right[iter] = permutation[j];
        ++iter;
    }
    return right;
}

// Поиск индекса минимального элемента перестановки right размером size,
// превосходящего num
int findMin(int* right, int num, int size)
{
    int min = 0;
    for (int i = 1; i < size; ++i)
    {
        if ( (right[i] > num) && (right[i] < right[min]) ) min = i;
    }
    return min;
}

// Генерация перестановки размером n по уникальному номеру k:
// permutation – искомая перестановка с номером k;
// was[j] – встречается ли элемент с номером j в генерируемой перестановке;
// alreadyWas – номер элемента переставляемого множества, который
// встречался на i-й позиции перестановок с номером, меньшим k, n раз;
// curFree – номер элемента переставляемого множества, который встречался на
// i-й позиции перестановок с номером, меньшим k, менее n раз.
int* num2permutation(int k, int n)
{
    n -=1; // т.к. при генерации перестановки нет 0

    int* permutation = new int[n] {0};

    bool* was = new bool[n] {false};
    
    for (int i = 1; i < n + 1; ++i)
    {
        int alreadyWas = floor(k / factorial(n - i));
        k %= factorial(n - i);
        int curFree = 0;

        for (int j = 1; j < n + 1; ++j)
        { 
            if (!was[j - 1])
            {
                ++curFree;
                if (curFree == alreadyWas + 1)
                {
                    permutation[i - 1] = j;
                    was[j - 1] = true;
                }
            }
        }
    }

    // Первый элемент перестановки всегда 0
    int* res = new int[n+1];
    res[0] = 0;
    for(int i = 1; i < n+1; ++i)
    {
        res[i] = permutation[i-1];
    }

    delete[] permutation;
    return res;
}

// Генерация следующей за permutation перестановки
// размером n в лексикографическом порядке:
int* nextLexisPermutation(int* permutation, int n)
{
    int* nextPermutation = new int[n];

    for(int i = 0; i < n; ++i) nextPermutation[i] = permutation[i];

    for (int i = 2; i < n + 1; ++i) // поиск справа налево
    {
        // Поиск пары элементов такой, что значение правого
        // элемента пары больше значения левого
        if (nextPermutation[n - i] > nextPermutation[n - i + 1]) continue;

        // Выделение возрастающего суффикса
        int* right = createRight(nextPermutation, n - i + 1, n);

        // Поиск в перестановке минимального элемента, превосходящего левый
        // элемент пары, начиная с правого элемента пары до конца массива
        int swapPoint = findMin(right, nextPermutation[n - i], i) + n - i + 1;

        // Перестановка найденного в прошлом шаге элемента
        // с левым элементом пары
        swapEl(nextPermutation, swapPoint, n - i);

        // Указатель на часть перестановки, начинающуюся со следующего
        // за левым переставленным в прошлом шаге элементом
        int* toSort = nextPermutation + n - i + 1;

        // Быстрая сортировка элементов перестановки, расположенных
        // с полученного в прошлом шаге указателя до конца перестановки 
        qsort(toSort, i - 1, sizeof(int), compare);

        return nextPermutation;
    }

    return nextPermutation;
}

// Расчёт стоимости маршрута permutation размера n
// на основе матрицы А размера nxn, элемент A[i][j] которой 
// является стоимостью проезда из города i в город j
float permutationToCost(int* permutation, int n, float *A)
{
    float cost = 0;

    for(int i = 0; i < n - 1; ++i)
    {
        cost += A[permutation[i]*n + permutation[i+1]];
    }
    // Из последнего города маршрута коммифояжер
    // возвращается в первый город
    cost += A[permutation[n - 1]*n]; 

    return cost;
}

// Структура маршрут-стоимость
struct routeCost
{
    int* route;
    float cost;
};

// Структура стоимость-ранг
struct forReduceMin
{
    float cost;
    int rank;
};

// Работа в тестовом режиме
// np - количество процессов
// num - количество городов
// filename - имя входного файла
// rank - ранг вычисляющего процесса
// COMM - коммуникатор
int Test(int np, int num, const char* filename, int rank, MPI_Comm COMM)
{
    int errValue;
    string text;

    // Чтение файла в строку text
    if (rank == 0) errValue = ReadAll(filename, text, MPI_COMM_SELF);

    // Распространение ошибки
    MPI_Bcast(&errValue, 1, MPI_INT, 0, COMM);
    if (errValue != SUCCESS) return ERROR;

    // Инициализация матрицы А
    float *A = new float[num*num];
    if ((rank == 0) && (errValue == SUCCESS)) errValue = ParseMatrix(text, A, num);
    
    MPI_Bcast(&errValue, 1, MPI_INT, 0, COMM);
    if (errValue != SUCCESS) return ERROR;

    // Создание пользовательского типа данных,
    // содержащего матрицу А для широковещательной передачи 
    long int size_of_float;
	MPI_Datatype column, xpose;

    MPI_Type_extent(MPI_FLOAT, &size_of_float);
	MPI_Type_vector(num, 1, num, MPI_FLOAT, &column);
	MPI_Type_hvector(num, 1, size_of_float, column, &xpose);
	MPI_Type_commit(&xpose);

    // Передача матрицы А  всем процессам
    errValue = MPI_Bcast(A, 1, xpose, 0, COMM);
    if (errValue != SUCCESS) return ERROR;

    // Всего на одном процессоре (num-1)!/np перестановок
    // т.к. первым элементом всегда явл. 1-й город
    routeCost* routes = new routeCost[factorial(num-1)/np];
    
    // k - номер первой перестановки диапазона каждого из процессов
    int k = rank*factorial(num-1)/np;

    // Генерация первой перестановки по номеру k и нахождение стоимости
    // соответствующего маршрута
    int min = 0;
    routes[0].route = num2permutation(k, num);
    routes[0].cost = permutationToCost(routes[0].route, num, A);
    
    // Генерация последующих перестановок диапазона 
    // в лексикографическом порядке и рассчёт стоимостей
    // соответствующих им маршрутов 
    for (int j = 1; j < factorial(num - 1) / np; ++j)
    {
        routes[j].route = nextLexisPermutation(routes[j-1].route, num);
        routes[j].cost = permutationToCost(routes[j].route, num, A);
        
        // Поиск минимального по стоимости маршрута
        if (routes[j].cost < routes[min].cost) min = j;
    }

    // Редукция с глобальной операцией "минимум с локализацией" и поиск
    // процесса, содержащего минимальный по стоимости маршрут
    forReduceMin value, result;
    value.cost = routes[min].cost;
    value.rank = rank;

    errValue = MPI_Allreduce(&value, &result, 1, MPI_FLOAT_INT, MPI_MINLOC, COMM);
    if (errValue != SUCCESS) return ERROR;

    // Вывод результата из процесса, содержащего минимальный
    // по стоимости маршрут
    if (rank == result.rank)
    {
        printf("Desired route: ");
        for (int i = 0; i < num; ++i) printf("%d ", routes[min].route[i] + 1);
        printf("\nCost: %.2f\n", routes[min].cost);
    }

    delete[] routes;
    delete[] A;
    MPI_Type_free(&xpose);
    MPI_Type_free(&column);

    return SUCCESS;
}

// Работа в режиме вычислительного эксперимента
// np - количество процессов
// num - количество городов
// rank - ранг вычисляющего процесса
// COMM - коммуникатор
int Experiment(int np, int num, int rank, MPI_Comm COMM)
{
    int errValue = SUCCESS;
    double starttime, endtime;
    starttime = MPI_Wtime();

    // Матрица А, содержащая стоимости проезда
    float* A = new float[num*num];

    // Всего на одном процессоре (num-1)!/np перестановок
    // т.к. первым элементом всегда явл. 1-й город
    routeCost* routes = new routeCost[factorial(num-1)/np];

    // Создание пользовательского типа данных,
    // содержащего матрицу А для широковещательной передачи 
    long int size_of_float;
	MPI_Datatype column, xpose;

    MPI_Type_extent(MPI_FLOAT, &size_of_float);
	MPI_Type_vector(num, 1, num, MPI_FLOAT, &column);
	MPI_Type_hvector(num, 1, size_of_float, column, &xpose);
	MPI_Type_commit(&xpose);

    // Инициализация матрицы А на главном процессе и передача всем другим
    if (rank == 0)
    {
        for (int i = 0; i < num*num; ++i) A[i] = (float)rand();

        errValue = MPI_Bcast(A, 1, xpose, 0, COMM);
        if (errValue != SUCCESS) return ERROR;
    }
    else 
    {
        errValue = MPI_Bcast(A, 1, xpose, 0, COMM);
        if (errValue != SUCCESS) return ERROR;
    }

    // k - номер первой перестановки диапазона каждого из процессов
    int k = rank*factorial(num-1)/np;

    // Генерация первой перестановки по номеру k и нахождение стоимости
    // соответствующего маршрута
    int min = 0;
    routes[0].route = num2permutation(k, num);
    routes[0].cost = permutationToCost(routes[0].route, num, A);
    
    // Генерация последующих перестановок диапазона 
    // в лексикографическом порядке и рассчёт стоимостей
    // соответствующих им маршрутов 
    for (int j = 1; j < factorial(num - 1) / np; ++j)
    {
        routes[j].route = nextLexisPermutation(routes[j-1].route, num);
        routes[j].cost = permutationToCost(routes[j].route, num, A);
        if (routes[j].cost < routes[min].cost) min = j;
    }

    // Редукция с глобальной операцией "минимум с локализацией" и поиск
    // процесса, содержащего минимальный по стоимости маршрут
    forReduceMin value, result;
    value.cost = routes[min].cost;
    value.rank = rank;

    errValue = MPI_Allreduce(&value, &result, 1, MPI_FLOAT_INT, MPI_MINLOC, COMM);
    if (errValue != SUCCESS) return errValue;

    // Вывод результата из процесса, содержащего минимальный
    // по стоимости маршрут
    if (rank == result.rank)
    {
        endtime = MPI_Wtime();
        printf("Result time: %.2f \n", endtime-starttime);
    }

    delete[] routes;
    delete[] A;
    MPI_Type_free(&xpose);
    MPI_Type_free(&column);
    return SUCCESS;
}

int main(int argc, char* argv[])
{
    int rank, allnum, errValue, num, key;
    int color = 0;
    char strError[MPI_MAX_ERROR_STRING];
    bool is_test = false;
    string mode, flag, filename;

    MPI_Init(&argc, &argv);

    errValue = MPI_Comm_size(MPI_COMM_WORLD, &allnum);
    if (errValue != SUCCESS)
    {
        MPI_Finalize();
        return ERROR;
    }

    errValue = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (errValue != SUCCESS) 
    {
        MPI_Finalize();
        return ERROR;
    }

    // Парсинг командной строки

    // Проверка наличия входных параметров
    if (argc != 4 ) 
    {
        if (rank == 0) fprintf(stderr, "Argument error\n");
        MPI_Finalize();
        return ERROR;
    }

    mode = argv[1];

    // Проверка входных параметров для тестового режима
    if ( mode == "--test" )
    {
        flag = argv[2];
        if (flag != "-i")
        {
            if (rank == 0) fprintf(stderr, "Argument error\n");
            MPI_Finalize();
            return ERROR;
        }
        filename = argv[3];
    }
    // Проверка входных параметров для режима эксперимента
    else if ( mode == "--experiment")
    {
        flag = argv[2];
        if (flag != "-N")
        {
            if (rank == 0) fprintf(stderr, "Argument error\n");
            MPI_Finalize();
            return ERROR;
        }

        stringstream convert{ argv[3] };

        // Перевод char* в int
        if (!(convert >> num))
        {
            if (rank == 0) fprintf(stderr, "Argument error\n");
            MPI_Finalize();
            return ERROR;
        }

        if ((num <= 0) || (num > N_BORDER))
        {
            if (rank == 0) fprintf(stderr, "Argument error\n");
            MPI_Finalize();
            return ERROR;
        }
    }
    // Проверка правильности задания флага режима запуска
    else
    {
        if (rank == 0) fprintf(stderr, "Mode error\n");
        MPI_Finalize();
        return ERROR;
    }

    if (mode == "--test")
    {
        string text;

        // Нахождение размера входной матрицы
        if (rank == 0)
        {
            errValue = ReadAll(filename.c_str(), text, MPI_COMM_SELF);
            if (errValue == SUCCESS)
            {
                errValue = GetSizeMatrix(text, &num);
                if (errValue != SUCCESS) errValue = ERROR;
            }
        }

        MPI_Bcast(&errValue, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (errValue != SUCCESS) 
        {
            MPI_Finalize();
            return ERROR;
        }

        errValue = MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (errValue != SUCCESS) 
        {
            MPI_Finalize();
            return ERROR;
        }

        // Количество запущенных процессов не может превышать количество перестановок
        if (allnum > factorial(num - 1)) 
        {
            allnum = factorial(num - 1);

            if (rank < allnum) color = 1;
        }

        // Инициализация нового коммуникатора
        MPI_Comm newcomm;
        MPI_Comm_split(MPI_COMM_WORLD, color, rank, &newcomm);
        
        if (rank < allnum) Test(allnum, num, filename.c_str(), rank, newcomm);
    }

    if (mode == "--experiment")
    {
        // Количество запущенных процессов не может превышать количество перестановок
        if (allnum > factorial(num - 1)) 
        {
            allnum = factorial(num - 1);

            if (rank < allnum) color = 1;
        }

        // Инициализация нового коммуникатора
        MPI_Comm newcomm;
        MPI_Comm_split(MPI_COMM_WORLD, color, rank, &newcomm);

        if (rank < allnum) Experiment(allnum, num, rank, newcomm);
    }

    
    MPI_Finalize();
    return SUCCESS;
}