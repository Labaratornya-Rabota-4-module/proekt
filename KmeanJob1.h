#pragma once
#include "Job.h"
#include "Kmean.h"
#include "VectorAndMatrix.h"
#include <string>
#include <vector>


// Функция которая считает Евклидово расстояние
inline double euclideanDistance(const Vector<double>& a, const Vector<double>& b)
{
	double res = 0;
	for (size_t i = 0; i < a.size(); ++i)
		res += (a[i] - b[i]) * (a[i] - b[i]);

	return std::sqrt(res);
}


// Работа которая выполняет алгоритм к-средних
template <typename T>
class KmeanJob : public Job
{
private:
	std::vector<T> data;
	size_t clusterNumber;

	bool isError;
public:
	KmeanJob(std::ifstream&& fileIn, std::ofstream&& fileOut) : Job(std::move(fileIn), std::move(fileOut)), isError(false)
	{ }

	virtual void makeJob()
	{
		readInitialSettings(); // Считываем данные
		if (!isError && clusterNumber < data.size() && clusterNumber != 0)
		{
			Kmean<T> algorithm(data, clusterNumber, euclideanDistance);
			algorithm.completeAlgorithm(); // Выполнение алгоритма
			algorithm.printIn(fileOut); // Выводим результат в файл
		}
		else
		{
			fileOut << "Bad data..." << std::endl;
		}
	}
private:
	void readInitialSettings()
	{
		bool isError;

		std::string line;
		size_t delimiter_pos;
		fileIn.seekg(0);
		std::getline(fileIn, line);
		std::getline(fileIn, line);

		if (!fileIn.eof() && (delimiter_pos = line.find('=')) != std::string::npos && delimiter_pos + 1 < line.length())
		{
			clusterNumber = std::atoi(line.substr(delimiter_pos + 1).c_str());
			readData();
		}
		else isError = true;
	}
private:
	void readData()
	{
		T a;
		while (!fileIn.eof())
		{
			fileIn >> a;
			if (fileIn.eof()) break;
			data.push_back(a);
		}
	}
};
