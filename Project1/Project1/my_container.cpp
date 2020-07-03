#include <iostream>
#include <utility>
#include <algorithm>

namespace my {
	struct XYZ {
		XYZ() = delete;
		XYZ(int a) {};
		~XYZ() {
			std::cout << "Destruct" << std::endl;
		}
	};

	template<typename T>
	class vector;

	template<typename T>
	class vector_iterator {
		/*template <typename U>
		friend class vector;*/

		friend class vector<T>;

		T* my_item;

		vector_iterator(T* item) : my_item(item) {};
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using reference = T& ;
		using pointer = T*;
		using difference_type = int;

		vector_iterator() : my_item(nullptr) {};

		vector_iterator(const vector_iterator& other) : my_item(other.my_item) {};

		vector_iterator& operator=(const vector_iterator& other) {
			my_item = other.my_item;
			return *this;
		};

		~vector_iterator() {};

		T& operator*() const {
			return *my_item;
		};

		T* operator->() const {
			return my_item;
		}

		vector_iterator operator++(int) {
			vector_iterator tmp = *this;
			++*this;
			return tmp;
		};

		vector_iterator& operator++() {
			++my_item;
			return *this;
		};

		vector_iterator operator--(int) {
			vector_iterator tmp = *this;
			--*this;
			return tmp;
		};

		vector_iterator& operator--() {
			--my_item;
			return *this;
		};

		vector_iterator& operator+=(int n) {
			my_item += n;
			return *this;
		};

		vector_iterator& operator-=(int n) {
			my_item -= n;
			return *this;
		};

		vector_iterator operator-(int n) {
			return vector_iterator(my_item - n);
		};

		vector_iterator operator+(int n) {
			return vector_iterator(my_item + n);
		};


		bool operator<(const vector_iterator& it) const {
			return my_item < it.my_item;
		}

		bool operator<=(const vector_iterator& it) const {
			return my_item <= it.my_item;
		}

		bool operator>(const vector_iterator& it) const {
			return my_item > it.my_item;
		}

		bool operator>=(const vector_iterator& it) const {
			return my_item >= it.my_item;
		}

		int operator-(const vector_iterator& it) const  {
			return my_item - it.my_item;
		};

		friend vector_iterator& operator+(int n, const vector_iterator& rhs) {
			return rhs.my_item + n;
		}

		friend vector_iterator& operator-(int n, const vector_iterator& rhs) {
			return rhs.my_item + n;
		}

		friend bool operator==(const vector_iterator& lhs, const vector_iterator& rhs) {
			return lhs.my_item == rhs.my_item;
		}

		friend bool operator!=(const vector_iterator& lhs, const vector_iterator& rhs) {
			return lhs.my_item != rhs.my_item;
		}
	};

	

	template<typename T>
	class vector
	{
	private:
		T * my_data;
		std::size_t my_size;
		std::size_t my_cap;

		template <typename U>
		void push_back_impl(U&& value) {
			if (my_size == my_cap) {
				int cap = my_cap == 0 ? 1 : my_cap * 2;

				T* tmp = static_cast<T*>(operator new[](sizeof(T) * cap));

				for (std::size_t i = 0; i < my_size; ++i) {
					*(tmp + i) = std::move(my_data[i]);
				}

				for (size_t i = 0; i < my_size; i++) {
					(my_data + i)->~T();
				}
				operator delete(my_data);
				my_data = tmp;
				my_cap = cap;
			}
			new(my_data + my_size) T(std::forward<U>(value));
			my_size++;
		}
	public:
		using iterator = vector_iterator<T>;

		vector() : my_data(nullptr), my_size(0), my_cap(0) {};

		vector(std::size_t sz, const T& value): my_data(static_cast<T*>(operator new[](sizeof(T) * sz))), my_size(sz), my_cap(sz){
			for (std::size_t i = 0; i < sz; i++)
			{
				new(my_data + i)T(value);
			}
		}

		vector(std::initializer_list<T> init) : my_data(static_cast<T*>(operator new[](sizeof(T) * init.size()))), my_size(init.size()), my_cap(init.size()) {
			size_t i = 0;
			for (auto& item : init) {
				new(my_data + i)T(item);
				++i;
			}
		}

		vector(vector<T>& vec) : my_data(static_cast<T*>(operator new[](sizeof(T) * vec.size()))), my_size(vec.size()), my_cap(vec.size()) {
			for (std::size_t i = 0; i < vec.size(); i++)
			{
				new(my_data + i)T(vec[i]);
			}
		}

		vector(vector<T>&& vec) 
			: my_size(vec.size()), my_cap(vec.size()) {
			my_data = vec.my_data;
			vec.my_data = nullptr;
			vec.size = 0;
			vec.capacity = 0;
		}

		vector& operator=(vector<T>& vec){
			if (my_data != vec.my_data) {
				for (size_t i = 0; i < my_size; i++) {
					(my_data + i)->~T();
				}
				operator delete(my_data);
				///TODO: size
				my_size = vec.size();
				my_cap = vec.size();
				my_data = static_cast<T*>(operator new[](sizeof(T) * vec.size()));

				for (std::size_t i = 0; i < vec.size(); i++)
				{
					new(my_data + i)T(vec[i]);
				}
			}
			return *this;
		}

		vector& operator=(vector<T>&& vec) {
			if (my_data != vec.my_data) {
				for (size_t i = 0; i < my_size; i++) {
					(my_data + i).~T();
				}
				operator delete(my_data);

				my_size = vec.size();
				my_cap = vec.size();
				my_data = vec.my_data;
				vec.my_data = nullptr;
				vec.size = 0;
				vec.capacity = 0;
			}
			return *this;
		}

		vector& operator=(std::initializer_list<T> init) {
			for (size_t i = 0; i < my_size; i++) {
				(my_data + i).~T();
			}
			operator delete(my_data);

			my_size = init.size();
			my_cap = init.size();
			my_data = static_cast<T*>(operator new[](sizeof(T) * init.size()));
			size_t i = 0;
			for (auto& item : init) {
				new(my_data + i)T(item);
				++i;
			}
			return *this;
		}

		~vector() {
			for (size_t i = 0; i < my_size; i++) {
				(my_data + i)->~T();
			}
			operator delete(my_data);
		}

		std::size_t size() {
			return my_size;
		}

		std::size_t capacity() {
			return my_cap;
		}

		bool empty() { 
			return my_size == 0;
		}

		void clear() {
			for(size_t i = 0; i < my_size; i++) {
				(my_data + i)->~T();
			}
			my_size = 0;
			my_cap = 0;
		}

		T& operator[](std::size_t index) {
			return my_data[index];
		}

		T& at(std::size_t index) {
			if (index >= my_size) {
				throw std::out_of_range("Index out of range");
			}
			return my_data[index];
		}

		void resize(int newSize) {
			while (newSize + 1 > my_cap) {
				///TODO: capacity and size 
				my_cap = my_cap != 0 ? my_cap * 2 : 1;
			}
			T* newData = new T[my_cap];
			if (my_size < newSize) {
				for (size_t i = 0; i < my_size; i++)
				{
					newData[i] = std::move(my_data[i]);
				}
			}		

			for (size_t i = 0; i < my_size; i++) {
				(my_data + i)->~T();
			}
			operator delete(my_data);
			my_size = newSize;
			my_data = newData;
		}

		void reserve(int newCap) {
			if (newCap > my_cap) {
				T* newData = static_cast<T*>(operator new[](sizeof(T) * newCap));
				for (size_t i = 0; i < my_size; i++)
				{
					*(newData+i) = std::move(my_data[i]);					
				}

				for (size_t i = 0; i < my_size; i++) {
					(my_data + i)->~T();
				}
				operator delete(my_data);
				my_cap = newCap;
				my_data = newData;
			}
		}

		void push_back(const T& value) {
			push_back_impl(value);
		}

		void push_back(T&& value) {
			push_back_impl(std::move(value));
		}

		iterator begin() {
			return iterator(my_data);
		}

		iterator end() {
			return iterator(my_data + my_size);
		}
	};
}

/*int main() {
	int * a = nullptr;
	delete a;
	my::vector<int> v0{ 3,4,1,2,6,5 };

	std::sort(v0.begin(), v0.end());

	for (auto a : v0) {
		std::cout << a;
	}

	my::vector<int> v(10, 2);

	my::XYZ x(1);

	my::vector<my::XYZ> v1(2, x);

	my::vector<my::XYZ> v2(v1);

	v1 = v2;

	v1.push_back(x);

	int k = 10;

	v1.push_back(my::XYZ(4));

	v.resize(10);

	v.reserve(20);

	system("pause");
}*/