#include <iostream>

/*template<typename T, typename U>
struct is_same {
	static bool same;

};

template<typename T, typename U>
bool is_same<T, U>::same = false;

template<typename T>
struct is_same<T,T>{
	static bool same;
};

template<typename T>
bool is_same<T, T>::same = true;

*/

template <typename T>
class Comparator {
public:
	Comparator() {};
	Comparator(T) {};
	template <typename P, typename U>
	bool operator()(T first,T second) {
		if (std::is_same_v<P, U>) {
			try {
				return *(P*)first < *(P*)second;
			}
			catch (...)
			{
				throw "Error";
			}
		}
		else {
			try
			{
				return (*(P*)first < *(P*)second);
			}
			catch (...)
			{

			}

			try
			{
				return (*(U*)first < *(U*)second);
			}
			catch (...)
			{

			}

			throw "Error";
		}
	}
};

template <typename T, typename U, typename Comp>
bool compare(T first, U second, Comp comp) {
	return(comp.operator()<T,U>(&first, &second));
}

struct A {

};

struct B {

};

/*int main() {
	
	std::cout << compare(1, 2, Comparator<void*>()) << std::endl;
	//system("pause");
}*/

