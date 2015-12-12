int jedu(int a, int b);

int jedu(int a, int b ){
	int c;
	c = a + b;
	return c;
}

int factorial(int n) // Definice funkce pro vypocet hodnoty faktorialu
{
int temp_result;
auto decremented_n = n - 1;
if (n < 2) {
return 1;
} else {
temp_result = factorial(decremented_n);
return n * temp_result;
}
}


int main(){
	int d;
	int e;
	d = 5;
	e = 6;
	int f;
	f = jedu(e,d);
	//f = 11;
	for(int i=5; i < f; i=i+1){
		e = e + 2;
	}
	int z;
	z = factorial(4);
	cout << z << " " << e << " " << f; // vraci 24 18 11 
	return 0;
}