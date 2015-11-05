int soucet(b)
{
	int vysl;
	vysl = b + b;
	return vysl;
}

int rozdil(b)
{
	int vysl;
	vysl = b - b;
	return vysl;
}

int nasobeni(b)
{
	int vysl;
	vysl = b * b;
	return vysl;
}

int deleni(b)
{
	int vysl;
	vysl = b / b;
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