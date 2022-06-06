#include <cassert>
#include <iostream>
#include <numeric>

#include "simple_vector.h"

using namespace std;

class X {
public:
    X()
        : X(5) {
    }
    X(size_t num)
        : x_(num) {
    }
    X(const X& other) = delete;
    X& operator=(const X& other) = delete;
    X(X&& other) {
        x_ = exchange(other.x_, 0);
    }
    X& operator=(X&& other) {
        x_ = exchange(other.x_, 0);
        return *this;
    }
    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    iota(v.begin(), v.end(), 1);
    return v;
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    cout << "Test with temporary object, copy elision" << endl;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    cout << "Test with temporary object, operator=" << endl;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    cout << "Test with named object, move constructor" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    cout << "Test with named object, operator=" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = move(vector_to_move);

    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    cout << "Test noncopiable object, move constructor" << endl;
    SimpleVector<X> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = move(vector_to_move);

    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    cout << "Test noncopiable push back" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    assert(v.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(v[i].GetX() == i);
    }
    cout << "Done!" << endl << endl;

    SimpleVector<X> x;
    for (size_t i = 0; i < size; ++i) {
        x.PushBack(X(i));
    }
    x.Resize(10);;
    assert(x.GetSize() == 10);
    assert(x.GetCapacity() == 10);
    x.Resize(5);
    assert(x.GetSize() == 5);
    assert(x.GetCapacity() == 10);
    x.Resize(0);
    assert(x.GetSize() == 0);
    assert(x.GetCapacity() == 10);
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    cout << "Test noncopiable insert" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    // в начало
    v.Insert(v.begin(), X(size + 1));
    assert(v.GetSize() == size + 1);
    assert(v.begin()->GetX() == size + 1);
    // в конец
    v.Insert(v.end(), X(size + 2));
    assert(v.GetSize() == size + 2);
    assert((v.end() - 1)->GetX() == size + 2);
    // в середину
    v.Insert(v.begin() + 3, X(size + 3));
    assert(v.GetSize() == size + 3);
    assert((v.begin() + 3)->GetX() == size + 3);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() {
    const size_t size = 3;
    cout << "Test noncopiable erase" << endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }
    // for (size_t i = 0; i < v.GetSize(); i++) {
    //     std::cout << "i do = " << v[i].GetX() << std::endl;
    // }

    auto it = v.Erase(v.begin());

    // for (size_t i = 0; i < v.GetSize(); i++) {
    //     std::cout << "i past = " << v[i].GetX() << std::endl;
    // }
    // if (it == nullptr) {
    //     std::cout << "nullptr" << std::endl;
    // } else {
    //     std::cout << "it->GetX() = " << (it - 0)->GetX() << std::endl;
    // }
    assert(it->GetX() == 1);
    cout << "Done!" << endl << endl;
}

void TestReserveConstructor() {
    using namespace std;
    cout << "TestReserveConstructor"s << endl;
    SimpleVector<int> v(Reserve(5));
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    using namespace std;
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> v;
    // зарезервируем 5 мест в векторе
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
    // попытаемся уменьшить capacity до 1
    v.Reserve(1);
    // capacity должно остаться прежним
    assert(v.GetCapacity() == 5);
    // поместим 10 элементов в вектор
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);

    }
    assert(v.GetSize() == 10);
    // увеличим capacity до 100
    v.Reserve(100);
    // проверим, что размер не поменялся
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    // проверим, что элементы на месте
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
    cout << "Done!"s << endl;
}

inline void Test1() {
    // Инициализация конструктором по умолчанию
    {
        SimpleVector<int> v;
        assert(v.GetSize() == 0u);
        assert(v.IsEmpty());
        assert(v.GetCapacity() == 0u);
    }

    // Инициализация вектора указанного размера
    {
        SimpleVector<int> v(5);
        assert(v.GetSize() == 5u);
        assert(v.GetCapacity() == 5u);
        assert(!v.IsEmpty());
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 0);
        }
    }

    // Инициализация вектора, заполненного заданным значением
    {
        SimpleVector<int> v(3, 42);
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 42);
        }
    }

    // Инициализация вектора при помощи initializer_list
    {
        SimpleVector<int> v{1, 2, 3};
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        assert(v[2] == 3);
    }

    // Доступ к элементам при помощи At
    {
        SimpleVector<int> v(3);
        assert(&v.At(2) == &v[2]);
        try {
            v.At(3);
            assert(false);  // Ожидается выбрасывание исключения
        } catch (const std::out_of_range&) {
        } catch (...) {
            assert(false);  // Не ожидается исключение, отличное от out_of_range
        }
    }

    // Очистка вектора
    {
        SimpleVector<int> v(10);
        const size_t old_capacity = v.GetCapacity();
        v.Clear();
        assert(v.GetSize() == 0);
        assert(v.GetCapacity() == old_capacity);
    }

    // Изменение размера
    {
        SimpleVector<int> v(3);
        v[2] = 17;
        v.Resize(7);
        // for (size_t i = 0; i < v.GetSize(); i++) {
        //     std::cout << "v[i] = " << v[i] << std::endl;
        // }
        assert(v.GetSize() == 7);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[2] == 17);
        assert(v[3] == 0);
    }
    {
        SimpleVector<int> x(5, 42);
        assert((x == SimpleVector<int>{42, 42, 42, 42, 42}));
        x.Resize(10);
        assert((x == SimpleVector<int>{42, 42, 42, 42, 42, 0, 0, 0, 0, 0}));
    }
    {
        SimpleVector<int> v(3);
        v[0] = 42;
        v[1] = 55;
        const size_t old_capacity = v.GetCapacity();
        v.Resize(2);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
        assert(v[0] == 42);
        assert(v[1] == 55);
    }
    {
        const size_t old_size = 3;
        SimpleVector<int> v(3);
        v.Resize(old_size + 5);
        v[3] = 42;
        v.Resize(old_size);
        v.Resize(old_size + 2);
        assert(v[3] == 0);
    }

    // Итерирование по SimpleVector
    {
        // Пустой вектор
        {
            SimpleVector<int> v;
            assert(v.begin() == nullptr);
            assert(v.end() == nullptr);
        }

        // Непустой вектор
        {
            SimpleVector<int> v(10, 42);
            assert(v.begin());
            assert(*v.begin() == 42);
            assert(v.end() == v.begin() + v.GetSize());
        }
    }
}

inline void Test2() {
    // PushBack
    {
        SimpleVector<int> v(1);
        v.PushBack(42);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[0] == 0);
        assert(v[1] == 42);
        v.PushBack(1);
        assert(v[2] == 1);
        v.PushBack(2);
        assert(v[3] == 2);
        v.PushBack(3);
        assert(v[4] == 3);
        v.PushBack(4);
        assert(v[5] == 4);
        v.PushBack(5);
        assert(v[6] == 5);
        v.PushBack(6);
        assert(v[7] == 6);
        v.PushBack(7);
        assert(v[8] == 7);
        v.PushBack(8);
        assert(v[9] == 8);
        v.PushBack(9);
        assert(v[10] == 9);

        SimpleVector<int> x;
        x.PushBack(42);
        assert(x[0] == 42);
    }

    // Если хватает места, PushBack не увеличивает Capacity
    {
        SimpleVector<int> v(2);
        v.Resize(1);
        const size_t old_capacity = v.GetCapacity();
        v.PushBack(123);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
    }

    // PopBack
    {
        SimpleVector<int> v{0, 1, 2, 3};
        const size_t old_capacity = v.GetCapacity();
        const auto old_begin = v.begin();
        v.PopBack();
        assert(v.GetCapacity() == old_capacity);
        assert(v.begin() == old_begin);
        assert((v == SimpleVector<int>{0, 1, 2}));
    }

    // Конструктор копирования
    {
        SimpleVector<int> numbers{1, 2};
        auto numbers_copy(numbers);
        assert(&numbers_copy[0] != &numbers[0]);
        assert(numbers_copy.GetSize() == numbers.GetSize());
        for (size_t i = 0; i < numbers.GetSize(); ++i) {
            assert(numbers_copy[i] == numbers[i]);
            assert(&numbers_copy[i] != &numbers[i]);
        }
    }

    // Сравнение
    {
        assert((SimpleVector<int>{1, 2, 3} == SimpleVector<int>{1, 2, 3}));
        assert((SimpleVector<int>{1, 2, 3} != SimpleVector<int>{1, 2, 2}));

        assert((SimpleVector<int>{1, 2, 3} < SimpleVector<int>{1, 2, 3, 1}));
        assert((SimpleVector<int>{1, 2, 3} > SimpleVector<int>{1, 2, 2, 1}));

        assert((SimpleVector<int>{1, 2, 3} >= SimpleVector<int>{1, 2, 3}));
        assert((SimpleVector<int>{1, 2, 4} >= SimpleVector<int>{1, 2, 3}));
        assert((SimpleVector<int>{1, 2, 3} <= SimpleVector<int>{1, 2, 3}));
        assert((SimpleVector<int>{1, 2, 3} <= SimpleVector<int>{1, 2, 4}));
    }

    // Обмен значений векторов
    {
        SimpleVector<int> v1{42, 666};
        SimpleVector<int> v2;
        v2.PushBack(0);
        v2.PushBack(1);
        v2.PushBack(2);
        const int* const begin1 = &v1[0];
        const int* const begin2 = &v2[0];

        const size_t capacity1 = v1.GetCapacity();
        const size_t capacity2 = v2.GetCapacity();

        const size_t size1 = v1.GetSize();
        const size_t size2 = v2.GetSize();

        static_assert(noexcept(v1.swap(v2)));
        v1.swap(v2);
        assert(&v2[0] == begin1);
        assert(&v1[0] == begin2);
        assert(v1.GetSize() == size2);
        assert(v2.GetSize() == size1);
        assert(v1.GetCapacity() == capacity2);
        assert(v2.GetCapacity() == capacity1);
    }

    // Присваивание
    {
        SimpleVector<int> src_vector{1, 2, 3, 4};
        SimpleVector<int> dst_vector{1, 2, 3, 4, 5, 6};
        dst_vector = src_vector;
        assert(dst_vector == src_vector);
    }

    // Вставка элементов
    {
        SimpleVector<int> v{1, 2, 3, 4};
        auto check = v.Insert(v.begin() + 2, 42);
        assert((v == SimpleVector<int>{1, 2, 42, 3, 4}));
        assert(v.GetSize() == 5);
        assert(v.GetCapacity() == 8);
        assert(*check == 42);
        assert(check == v.begin() + 2);
    }
    {
        SimpleVector<int> q{1, 2, 3, 4};
        auto check = q.Insert(q.begin(), 42);
        assert((q == SimpleVector<int>{42, 1, 2, 3, 4}));
        assert(q.GetSize() == 5);
        assert(q.GetCapacity() == 8);
        assert(*check == 42);
        assert(check == q.begin());
    }
    {
        SimpleVector<int> q;
        q.PushBack(1);
        q.PushBack(2);
        q.PushBack(3);
        q.PushBack(4);
        q.PushBack(5);
        q.PushBack(6);
        q.PushBack(7);
        q.PushBack(8);
        q.PopBack();
        q.PopBack();
        q.PopBack();
        // тест добавления в начало - достаточно емкости
        assert(q.GetSize() == 5);
        assert(q.GetCapacity() == 8);
        assert((q == SimpleVector<int>{1, 2, 3, 4, 5}));
        auto return_check_begin = q.Insert(q.begin(), 42);
        assert(*return_check_begin = 42);
        assert(return_check_begin = q.begin());
        assert(q.GetSize() == 6);
        assert(q.GetCapacity() == 8);
        assert((q == SimpleVector<int>{42, 1, 2, 3, 4, 5}));
        // тест добавления в конец - достаточно емкости
        auto return_check_end = q.Insert(q.end(), 21);
        assert(*return_check_end = 21);
        assert(return_check_end = q.end() - 1);
        assert(q.GetSize() == 7);
        assert(q.GetCapacity() == 8);
        assert((q == SimpleVector<int>{42, 1, 2, 3, 4, 5, 21}));
        // тест на добавление в середину - достаточно емкости
        auto return_check_ = q.Insert(q.begin() + 4, 13);
        assert(*return_check_ = 13);
        assert(return_check_ = q.begin() + 4);
        assert(q.GetSize() == 8);
        assert(q.GetCapacity() == 8);
        assert((q == SimpleVector<int>{42, 1, 2, 3, 13, 4, 5, 21}));
    }
    {   
        SimpleVector<int> w{1, 2, 3, 4};
        // тест добавления в начало - недостаточно емкости
        auto return_check_begin = w.Insert(w.begin(), 42);
        assert(*return_check_begin == 42);
        assert(return_check_begin == w.begin());
        assert(w.GetSize() == 5);
        assert(w.GetCapacity() == 8);
        assert((w == SimpleVector<int>{42, 1, 2, 3, 4}));
    }
    {
        SimpleVector<int> e{1, 2, 3, 4};
        // тест добавления в конец - недостаточно емкости
        auto return_check_end = e.Insert(e.end(), 21);
        assert(*return_check_end == 21);
        assert(return_check_end == e.end() - 1);
        assert(e.GetSize() == 5);
        assert(e.GetCapacity() == 8);
        assert((e == SimpleVector<int>{1, 2, 3, 4, 21}));
    }
    {
        SimpleVector<int> r{1, 2, 3, 4};
        // тест на добавление в середину - недостаточно емкости
        auto return_check_end = r.Insert(r.begin() + 2, 21);
        assert(*return_check_end == 21);
        assert(return_check_end == r.begin() + 2);
        assert(r.GetSize() == 5);
        assert(r.GetCapacity() == 8);
        assert((r == SimpleVector<int>{1, 2, 21, 3, 4}));
    }

    {
        SimpleVector<int> v{1, 2, 3, 4};
        v.Insert(v.begin() + 2, 42);
        assert((v == SimpleVector<int>{1, 2, 42, 3, 4}));
    }

//     // Удаление элементов
//     {
//         SimpleVector<int> v{1, 2, 3, 4};
//         auto check = v.Erase(v.cbegin() + 2);
//         assert(*check == 4);
//         assert(check == v.begin() + 2);
//         assert((v == SimpleVector<int>{1, 2, 4}));
//     }
//     {
//         SimpleVector<int> x{1, 2, 3, 4};
//         auto check = x.Erase(x.begin());
//         assert(*check == 2);
//         assert(check == x.begin());
//         assert((x == SimpleVector<int>{2, 3, 4}));
//     }
//     {
//         SimpleVector<int> c{1, 2, 3, 4};
//         auto check = c.Erase(c.begin() + 1);
//         assert(*check == 3);
//         assert(check == c.begin() + 1);
//         assert(c.GetSize() == 3);
//         assert(c.GetCapacity() == 4);
//         assert((c == SimpleVector<int>{1, 3, 4}));
//     }
//     {
//         SimpleVector<int> b{1, 2, 3, 4};
//         auto check = b.Erase(b.end() - 1);
//         assert(check == b.end());
//         assert((b == SimpleVector<int>{1, 2, 3}));
//     }
//     {
//         SimpleVector<int> n{1, 2, 3, 4};
//         n.Erase(n.end() - 2);
//         assert((n == SimpleVector<int>{1, 2, 4}));
//     }
    {
        SimpleVector<int> m{1, 2, 3, 4};
        m.Erase(nullptr);
        assert((m == SimpleVector<int>{1, 2, 3, 4}));
    }
    {
        SimpleVector<int> l;
        l.Erase(l.begin());
        assert((l == SimpleVector<int>{}));
    }
}

int main() {
    Test1();
    Test2();
    TestReserveConstructor();
    TestReserveMethod();
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    return 0;
}
