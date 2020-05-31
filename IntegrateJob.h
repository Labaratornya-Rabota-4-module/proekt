#pragma once
#include "Job.h"
#include "MatrixVector.h"
#include <map>


// Класс который интегрирует переданную ему ОДУ и парсит файл с начальными данными объектом который передан вторым
template <typename Ode, typename OdeParser>
class IntegrateJob : public Job
{
private:
	// Для метода Рунге-Кутте
	Matrix<double> A;
	Vector<double> c;
	Vector<double> b_main;
	size_t stepsNumber;
	Ode ode;
	// Парсер файла
	OdeParser odeParser;
public:
	IntegrateJob(std::ifstream&& a, std::ofstream&& b);
	virtual ~IntegrateJob();
	virtual void makeJob();
private:
	void integrate_in_file(double null_time, double timespan, Vector<double> null_state, size_t iteration_number);
};


// Конструктор
template <typename Ode, typename OdeParser>
IntegrateJob<Ode, OdeParser>::IntegrateJob(std::ifstream&& a, std::ofstream&& b) : Job(std::move(a), std::move(b))
{
	// Начальное состояние ингтегратор возьмет из файла
	// RK5(4)7FS from Dorman and Prince
	A = { {0.0,			0.0,			0.0,			0.0,			0.0,		0.0,		0.0},
			{2.0 / 9,		0.0,			0.0,			0.0,			0.0,		0.0,		0.0},
			{1.0 / 12,		1.0 / 4,		0.0,			0.0,			0.0,		0.0,		0.0},
			{55.0 / 325,	-25.0 / 108,	50.0 / 81,		0.0,			0.0,		0.0,		0.0},
			{83.0 / 330,	-13.0 / 22,		61.0 / 66,		9.0 / 100,		0.0,		0.0,		0.0 },
			{-19.0 / 28,	9.0 / 4,		1.0 / 7,		-27.0 / 7,		22.0 / 7,	0.0,		0.0 },
			{19.0 / 200,	0.0,			3.0 / 5,		-243.0 / 400,	33.0 / 40,	7.0 / 80,	0.0} };
	c = { 0.0,			2.0 / 9,		1.0 / 3,		5.0 / 9,		2.0 / 3,	1.0,		1.0 };
	b_main = { 19.0 / 200,	0.0,			3.0 / 5,		-243.0 / 400,	33.0 / 40,	7.0 / 80,	0.0 };

	stepsNumber = c.size(); // Глубина метода
}

template <typename Ode, typename OdeParser>
IntegrateJob<Ode, OdeParser>::~IntegrateJob()
{ }

// Функция которая интегрирует ОДУ и записывает данные в файл (который ей передал JobKeeper)
template <typename Ode, typename OdeParser>
void IntegrateJob<Ode, OdeParser>::integrate_in_file(double null_time, double timespan, Vector<double> null_state, size_t iteration_number)
{
	const size_t dimension = null_state.size();
	Vector<double> a;
	Vector<Vector<double>> k;
	Vector<double> res;

	k.resize(stepsNumber);
	a.resize(dimension);
	res.resize(dimension);
	// Делаем некоторое количество шагов
	for (size_t n = 0; n < iteration_number; ++n)
	{
		// Обычный метод Рунге-Кутте
		res.apply([](double) -> double { return 0.0; });

		for (size_t i = 0; i < stepsNumber; ++i)
		{
			a.apply([](double a) -> double { return 0.0; });

			for (size_t j = 0; j < i; ++j)
				a += k[j]* A[i][j];
			(a *= timespan) += null_state;

			k[i] = ode(null_time + timespan * c[i], a);

			res += k[i] * b_main[i];
		}

		res *= timespan;
		res += null_state;

		fileOut << res << std::endl;
		// Переходим к следующему начальному состоянию
		null_time += timespan;
		null_state = res;
	}
}

template <typename Ode, typename OdeParser>
void IntegrateJob<Ode, OdeParser>::makeJob()
{
	std::map<std::string, double> map = odeParser(std::ref(fileIn)); // Парсим начальные данные
	ode.setNullSettings(map); // Устанавливаем настройки указанные в файле
	integrate_in_file(ode.getNullTime(), ode.getTimespan(), ode.getNullState(), ode.getIterationNumber()); // Интегрируем в файл
}
