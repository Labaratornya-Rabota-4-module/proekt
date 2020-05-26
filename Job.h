#pragma once
#include <fstream>

// Абстрактная работа
class Job
{
	
protected:
	std::ifstream fileIn; // Файл, из которого считываем начальные данные
	std::ofstream fileOut; // Файл, в который записываем результат
public:
	Job(const Job&) = delete;
	// Конструктор
	Job(std::ifstream&& a, std::ofstream&& b) : fileIn(std::move(a)), fileOut(std::move(b)) { }
	// Функция, которая выполняет задание
	virtual void makeJob() = 0;
	virtual ~Job() { }
};
