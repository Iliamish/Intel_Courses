#include <iostream>

namespace my {

	template <typename T>
	class default_deleter {
	public:
		void operator()(T* ptr) const { delete ptr; }
	};

	template<typename T>
	class default_deleter<T[]> {
	public:
		void operator()(T* ptr) const { delete[] ptr; }
	};

	template <typename T, typename Deleter = default_deleter<T>>
	class unique_ptr {
	public:
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;

		unique_ptr(unique_ptr&& other) : my_data(other.my_data), my_deleter(std::move(other.my_deleter)) {
			other.my_data = nullptr;
		}

		unique_ptr& operator=(unique_ptr&& other) {
			if (&other != this) {
				if (my_data) my_deleter(my_data);
				my_data = other.my_data;
				my_deleter = std::move(other.my_deleter);
				other.my_data = nullptr;
			}
			return *this;
		}

		unique_ptr(T* data) : my_data(data) {}

		unique_ptr(T* data, Deleter deleter) : my_data(data), my_deleter(std::move(deleter)) {}

		~unique_ptr() {
			if (my_data) my_deleter(my_data);
		}

		T& operator*() const { return *my_data; }

		void reset(T* data) {
			if (my_data) my_deleter(my_data);
			my_data = data;
		}

	private:
		T * my_data;
		Deleter my_deleter;
	};


	struct def_deleter {
		virtual void destruct(void* ptr) {};

		virtual ~def_deleter() {
			std::cout << "Deleter deleted" << std::endl;
		};
	};

	template <typename Type, typename Deleter = default_deleter<Type>>
	struct shared_deleter : public def_deleter {
		Deleter deleter;

		shared_deleter() = default;

		shared_deleter(Deleter& del) : deleter(std::move(del)) {
		
		};
		
		void destruct(void* ptr) override {
			deleter(reinterpret_cast<Type*>(ptr));
		}
	};

	struct control_block {
		std::size_t ref_counter = 0;
		def_deleter* deleter;

		~control_block() {
			delete deleter;
		}
	};

	

	template <typename T>
	class shared_ptr {
	public:
		shared_ptr(const shared_ptr& other) : my_data(other.my_data), my_deleter(std::move(other.my_deleter)), my_control_block(other.my_control_block) {
			++other.my_control_block->ref_counter;
			my_control_block->deleter = new shared_deleter<default_deleter<T>, T>();
		};

		shared_ptr& operator=(const shared_ptr& other) {
			if (my_control_block->ref_counter == 1) {
				my_deleter(my_data);
				delete my_control_block;
			}
			else {
				--my_control_block->ref_counter;
			}
			my_data = other.my_data;
			my_deleter = other.my_deleter;
			my_control_block = other.my_control_block;
			++other.my_control_block->ref_counter;
		};

		shared_ptr(shared_ptr&& other) : my_data(other.my_data), my_deleter(std::move(other.my_deleter)), my_control_block(other.my_control_block) {
			other.my_data = nullptr;
			other.my_control_block = nullptr;
		}

		shared_ptr& operator=(shared_ptr&& other) {
			if (&other != this) {
				if (my_control_block->ref_counter == 1) {
					my_deleter(my_data);
					delete my_control_block;
				}
				else {
					--my_control_block->ref_counter;
				}
				my_data = other.my_data;
				my_deleter = std::move(other.my_deleter);
				my_control_block = other.my_control_block;
				other.my_data = nullptr;
				other.my_control_block = nullptr;
			}
			return *this;
		}


		shared_ptr(T* data) : my_data(data) {
			my_control_block = new control_block;
			++my_control_block->ref_counter;
			my_control_block->deleter = new shared_deleter< T >();
		}

		template <typename Deleter>
		shared_ptr(T* data, Deleter& del) : my_data(data) {
			my_control_block = new control_block;
			++my_control_block->ref_counter;
			my_control_block->deleter = new shared_deleter< T, Deleter >(del);
		}

		~shared_ptr() {
			if (my_control_block) {
				if (my_control_block->ref_counter == 1) {
					my_control_block->deleter->destruct(my_data);
					delete my_control_block;
				}
				else {
					--my_control_block->ref_counter;
				}
			}
		}

		T& operator*() const { return *my_data; }

		std::size_t use_count() {
			return my_control_block->ref_counter;
		}

		void reset(T* data) {
			if (my_control_block->ref_counter == 1) {
				my_deleter(my_data);
				delete my_control_block;
			}
			else {
				--my_control_block->ref_counter;
			}
			my_control_block = new control_block;
			++my_control_block->ref_counter;
			my_data = data;
		}

	private:
		T * my_data;
		control_block* my_control_block = nullptr;
		default_deleter<T> my_deleter;
	};

	struct BA{
	public:
		void operator()() {
			std::cout << "11111" << std::endl;
		}
	};

	template<typename... Args>
	void foo(Args... args) {
		auto list = { (std::forward<Args>(args)(), 0)... };
	}

	template<typename T>
	void foobar(T t) {
		t();
	};
}





struct A {
	A() { std::cout << "A()\n"; }
	~A() { std::cout << "~A()\n"; }
};

struct B {
	B() { std::cout << "B()\n"; }
	~B() { std::cout << "~B()\n"; }
};

int main() {
	auto deleter = [](A* a) { std::cout << "Deleter call\n"; delete a; };
	/*my::unique_ptr <B[], decltype(deleter) > uptr(new B[2], deleter);

	my::unique_ptr<B[]> uptr2(new B[2]);*/

	my::shared_ptr<A> sptr(new A, deleter);

	/*std::cout << sptr.use_count() << std::endl;

	my::shared_ptr<A> sptr2(sptr);

	std::cout << sptr.use_count() << std::endl;

	sptr.reset(new A);

	std::cout << sptr.use_count() << std::endl;

	std::cout << sptr2.use_count() << std::endl;

	sptr = std::move(sptr2);

	my::foo(my::BA(), my::BA(), my::BA());*/

	return 0;
}