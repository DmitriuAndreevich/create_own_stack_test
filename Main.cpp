#include <iostream>
#include <cassert>//для assert
#include <sstream>//для ostringstream
#include <exception>
#include <string>

struct Stack {

private:
    int maxsize_;//Максимальный размер стека
    int* data_; //Указатель на эл-ты
    int size_; //Текущий размер стека
    
public:
    Stack(int size) { //Конструктор
        assert(size > 0);
        maxsize_ = size;
        data_ = new int[maxsize_];
        size_ = 0;
    }//т.е мы меняем объект, записывая дефолтные значения 

    void push(int x) {
        assert(size_ < maxsize_);
        data_[size_] = x;
        ++size_;
    }

    void pop() {
        --size_;
        assert(size_ >= 0);
    }

    int top() {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    bool empty() {
        return size_ == 0;
    }

    int size() {
        return size_;
    }

    void free() {
        delete[] data_;
        data_ = nullptr;
        maxsize_ = 0;
    }

    ~Stack() { //Деструктор (начинается с ~)
        free();
    }

};

template<class T, class U>
void AssertEqual(const T& t,const U& u,const std::string& hint) {//Почему тут классы?
    if (t != u) {
        std::ostringstream os; //поток для вывода - строка 
        os << "Assertion failed: " << t << "!=" << u<<"  Hint:"<<hint;//Запись производится в именов. обл. памяти
        throw std::runtime_error(os.str()); //Что такое (os.str()) 
    }
}



//Осуществим модульное тестирование
//Писать лучше разными модулями , чтобы отдельно проверять каждую функцию (в случае неполадки мы легко найдем где, 
// если же реализовывать в одном тесте , то в случае работы assert мы не сразу найдем в чем ошибка )
//Можно использовать assert, но тогда программа будет прекращать работу, при первом не сработавшем assert.
//Однако можно использовать AssertEqual, тогда программа закончит работу, даже при срабатывании AssertEqual

void testStackPush() {
    //Это отдельные тесты. 
    {
        Stack S(1);
        S.push(1);
        AssertEqual(S.top() , 1,"label 0");
    }
    //Это отдельные тесты. 
    {
        int n = 100;
        Stack S(n);
        for (int i = 1;i <= n;++i) {
            S.push(i);
            AssertEqual(S.top() , i, "label 1");
        }
    }
}

void testStackSize() {

    {
        Stack S(1);
        AssertEqual(S.size() , 0, "label 2"); //Как только стек создали размер должен быть 0
        S.push(1);
        AssertEqual(S.size() ,1, "label 3"); //Добавив 1 эл - т, размер стека должен быть 1
    }

    {
        int n = 100;
        Stack S(n);
        for (int i = 1;i <= n;++i) {
            S.push(i);
            AssertEqual(S.size() , i, "label 4");
        }
        AssertEqual(S.size() , n, "label 5");
    }
}

void testStackPop() {

    {
        Stack S(1);
        S.push(10);
        S.pop();
        AssertEqual(S.size() , 0, "label 6");
        AssertEqual(S.empty(), true, "label 7");
    }

    {
        int n = 100;
        Stack S(n);
        for (int i = 1;i <= n;++i) {
            S.push(i);
        }
        for (int i = 1;i <= n;++i) {
            S.pop();
            if (i != n) {
                AssertEqual(S.empty() , false , "label 8");
            }
        }
        AssertEqual(S.size() , 0, "label 9");
    }

    {
        int n = 100;
        Stack S(n);
        for (int i = 1;i <= n;++i) {
            S.push(i);
        }
        for (int i = n;i > 0;--i) {
            AssertEqual(S.top() , i, "label 10");
            S.pop();
        }
        AssertEqual(S.size() , 0,"label 11");
    }

}

void testStackEmpty() {
    Stack S(10);
    AssertEqual(S.empty(),true, "label 12");
}

void testStackPushPop() {
    {
        int n = 100;
        Stack S(n);
        for (int i = 1;i <= n;++i) {
            S.push(i);
            AssertEqual(S.top() , i, "label 13");
            S.pop();
            AssertEqual(S.empty(), true, "label 14");
        }
    }
}




//Создадим класс, в котором опишем шаблон функции, вызывающей тесты (дополнительно заключим вызов ф-ции в конструкцию
//try{}catch{} , чтобы программа не завершала работу, при неудачном тесте
class TestRunner {

private:
    //Создадим счетчик невыполн. тестов
    int fail_count = 0;

public:
template <class TestFunc>
void RunTest(TestFunc func, const std::string& test_name) {
    try {
        func();
        std::cerr << "Test " << test_name << " OK" << std::endl;
    }
    catch (std::runtime_error& e) {//std::runtime_error& , чтобы не копировать 
        std::cout <<"Test " << test_name << " fail. " << e.what() << std::endl;//e.what() - та строка, которая была передана exseption
        ++fail_count;
    }
    
}
//Опишем деструктор
~TestRunner() {
    if (fail_count > 0) {
        std::cerr << fail_count << " test failed. Terminate.";
        exit(1);//Выход из программы с кодом 1
    }
}

};



void testAll() {
    TestRunner tr;
    tr.RunTest(testStackPush, "StackPush");
    tr.RunTest(testStackEmpty, "StackEmpty");
    tr.RunTest(testStackSize, "StackSize");
    tr.RunTest(testStackPop, "StackPop");
    tr.RunTest(testStackPushPop, "StackPushPop");
     
}
//Здесь будет вызван деструктор от tr (после удаления объекта) 


int main()
{
#ifndef NDEBUG //Проверка находимся мы в realease или в debug
    testAll();
#endif
    std::cout << "size: ";
    int n;
    std::cin >> n;
    assert(n > 0);
    Stack S(n);

    //Добавление эл-тов в стек
    for (int i = 0; i < n;++i) {
        S.push(i + 1);
    }

    //Извлечение эл-тов из стека
    while (!S.empty()) {
        std::cout << S.top() << " ";
        S.pop();
    }

    return 0;
}

