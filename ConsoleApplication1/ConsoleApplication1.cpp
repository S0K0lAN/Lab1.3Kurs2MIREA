#include <iostream>
#include <fstream>
using namespace std;

template <class T>
class Element
{//элемент связного списка
private:
    //указатель на предыдущий и следующий элемент
    Element* next;
    Element* prev;
    //информация, хранимая в поле
    T field;
public:
    Element(T value = 0, Element<T>* next_ptr = NULL, Element<T>* prev_ptr = NULL)
    {
        field = value;
        next = next_ptr;
        prev - prev_ptr;
    }

    //доступ к полю *next
    virtual Element* getNext() { return next; }
    virtual void setNext(Element* value) { next = value; }

    //доступ к полю *prev
    virtual Element* getPrevious() { return prev; }
    virtual void setPrevious(Element* value) { prev = value; }

    //доступ к полю с хранимой информацией field
    virtual T getValue() { return field; }
    virtual void setValue(T value) { field = value; }
    template<class T> friend ostream& operator<< (ostream& ustream, Element<T>& obj);
};

template<class T>
ostream& operator << (ostream& ustream, Element<T>& obj)
{
    ustream << obj.field;
    return ustream;
}

template <class T>
class LinkedListParent
{
protected:
    //достаточно хранить начало и конец
    Element<T>* head;
    Element<T>* tail;
    //для удобства храним количество элементов
    int num;
public:
    virtual int Number() { return num; }
    virtual Element<T>* getBegin() { return head; }
    virtual Element<T>* getEnd() { return tail; }

    LinkedListParent()
    {
        //конструктор без параметров
        cout << "\nParent constructor";
        head = NULL;
        num = 0;
    }

    //чисто виртуальная функция: пока не определимся с типом списка, не сможем реализовать добавление
    virtual Element<T>* push(T value) = 0;

    //чисто виртуальная функция: пока не определимся с типом списка, не сможем реализовать удаление
    virtual Element<T>* pop() = 0;

    virtual ~LinkedListParent()
    {
        //деструктор - освобождение памяти
        cout << "\nParent destructor";
    }

    //получение элемента по индексу - какова асимптотическая оценка этого действия ?
    virtual Element<T>* operator[](int i)
    {
        //индексация
        if (i<0 || i>num) return NULL;
        int k = 0;
        //ищем i-й элемент - вставем в начало и отсчитываем i шагов вперед
        Element<T>* cur = head;
        for (k = 0; k < i; k++)
        {
            cur = cur->getNext();
        }
        return cur;
    }

    template<class T> friend ostream& operator<<(ostream& ustream, LinkedListParent<T>& obj);
    template<class T> friend istream& operator>>(istream& ustream, LinkedListParent<T>& obj);
};

template<class T>
ostream& operator << (ostream& ustream, LinkedListParent<T>& obj)
{
    if (typeid(ustream).name() == typeid(ofstream).name())
    {
        ustream << obj.num << "\n";
        for (Element<T>* current = obj.getBegin(); current != NULL; current = current->getNext())
            ustream << current->getValue() << " ";
        return ustream;
    }
    ustream << "\nLength: " << obj.num << "\n";
    int i = 0;
    for (Element<T>* current = obj.getBegin(); current != NULL; current = current->getNext(), i++)
        ustream << "arr[" << i << "] = " << current->getValue() << "\n";
    return ustream;
}

template<class T>
istream& operator >> (istream& ustream, LinkedListParent<T>& obj)
{
    //чтение из файла и консоли совпадают
    int len;
    ustream >> len;
    //здесь надо очистить память под obj, установить
    obj.num = 0;
    double v = 0;
    for (int i = 0; i < len; i++)
    {
        ustream >> v;
        obj.push(v);
    }
    return ustream;
}

template<typename ValueType>
class ListIterator : public iterator<input_iterator_tag, ValueType>
{
public:
    ListIterator() { ptr = NULL; }
    //ListIterator(ValueType* p) { ptr = p; }
    ListIterator(Element<ValueType>* p) { ptr = p; }
    ListIterator(const ListIterator& it) { ptr = it.ptr; }

    bool operator!=(ListIterator const& other) const { return ptr != other.ptr; }
    bool operator==(ListIterator const& other) const { return ptr == other.ptr; }//need for BOOST_FOREACH

    Element<ValueType>& operator*() { return *ptr; }
    ListIterator& operator++() { ptr = ptr->getNext(); return *this; }
    ListIterator& operator++(int v) { ptr = ptr->getNext(); return *this; }
    ListIterator& operator=(const ListIterator& it) { ptr = it.ptr; return *this; }
    ListIterator& operator=(Element<ValueType>* p) { ptr = p; return *this; }
private:
    Element<ValueType>* ptr;
};

template <class T = int>
class IteratedLinkedList : public LinkedListParent<T>
{
public:
    IteratedLinkedList() : LinkedListParent<T>() { cout << "\nIteratedLinkedList constructor"; }
    virtual ~IteratedLinkedList() { cout << "\nIteratedLinkedList destructor"; }

    ListIterator<T> iterator;
    ListIterator<T> begin() { ListIterator<T> it = LinkedListParent<T>::head; return it; }
    ListIterator<T> end() { ListIterator<T> it = LinkedListParent<T>::tail; return it; }
};


template<class T = int>
class Queue : public IteratedLinkedList<T> {
protected:

public:
    explicit Queue(size_t n) : IteratedLinkedList<T>() {
        if (n == 0) {
            // Если размер 0, то head и tail инициализируем как nullptr
            this->head = this->tail = nullptr;
        }
        else {
            // Создаем первый элемент
            this->head = new Element<T>(0);  // Первый элемент списка с полем 0
            Element<T>* current = this->head;

            // Заполняем остальные элементы
            for (size_t i = 1; i < n; ++i) {
                Element<T>* newElement = new Element<T>(i);  // Новый элемент с полем i
                current->setNext(newElement);  // Связываем текущий элемент с новым
                newElement->setPrevious(current);  // Связываем новый элемент с предыдущим
                current = newElement;  // Переходим к новому элементу
            }

            // Последний элемент списка — это tail
            this->tail = current;
        }
        this->num = n;  // Количество элементов в очереди
    }

    Element<T>* push(T value) override {
        auto newElement = new Element<T>(value);
        newElement->setPrevious(this->tail);
        this->tail->setNext(newElement);
        this->tail = newElement;
        this->num++;
        return newElement;
    }

    Element<T>* pop() override {
        auto popedElement = this->tail;
        this->tail->previous->next = nullptr;
        this->tail->previous = nullptr;
        this->tail = this->tail->previous;
        this->num--;
        return popedElement;
    }

    virtual ~Queue() {
        auto cur = this->head;
        while (cur != this->tail) {
            auto cur1 = cur;
            cur = cur->getNext();
            delete cur1;
        }
        delete this->tail;
    }

};




int main() {
    Queue<int> d(3);
    d.push(5);
    d.push(3);
    d.push(4);
    return 0;
}