int soucet(int b)
{
	int vysl;
	vysl = 1 + b;
	return vysl;
}

int rozdil(int b)
{
	int vysl;
	vysl = 2 - b;
	return vysl;
}

int nasobeni(int b)
{
	int vysl;
	vysl = 3 * b;
	return vysl;
}

int deleni(int b)
{
	int vysl;
	vysl = 4 / b;
	return vysl;
}

int main()
{
	int a;
	int vysledek;
	cout << "Zadej cislo\n";
	cin >> a;
	vysledek = soucet(a);
	cout << "Soucet:" << vysl;

	vysledek = rozdil(a);
	cout << "Rozdil:" << vysl;

	vysledek = nasobeni(a);
	cout << "A * A:" << vysl;

	vysledek = deleni(a);
	cout << "A : A:" << vysl;
}