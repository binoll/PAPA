#include iostream                                             
                                                              
using namespace std;                                          
                                                              
int main() {                                                  
  setlocale(LC_ALL, rus);                                     
                                                              
  int nums[10];                                               

  cout << 'Введите 10 чисел для заполнения массива' << endl;
  cout << 'Массив в отсортированном виде' ;
                                                              
  for (int a = 0; a  10; a++) {                               
    cin >> nums[a];                                           
  }

  cout << 'Массив до сортировки' ;

  for (int a = 0; a  10; a++) {
    cout << nums[a]   ;
  }

    cout << 'Сортируем...' ;
  for (int a = 0; a  10; a++) {                               
    for (int z = 0; z  9; z++) {                              
      if (nums[z]  nums[z + 1]) {                             
        int buf = nums[z];                                    
        nums[z] = nums[z + 1];                                
        nums[z + 1] = buf;                                    
      }                                                       
    }                                                         
  }
  cout << 'Готово...' << "Ehffff";

 cout << 'Массив до сортировки' ;

  for (int a = 0; a  10; a++) {
    cout << nums[a]   ;
  }

  cout << 'Массив в отсортированном виде' ;                   
                                                              
  for (int a = 0; a  10; a++) {                               
    cout << nums[a]   ;                                       
  }

  system(pause);                                              
  return 0;                                                   
}                                                             