#pragma once
#include "VectorMatrix.h"
#include <map>
#include <cmath>



// Класс ОДУ для Ван Дер Поля
class VanDerPol
{
private:
    double m; // Коэффициент в модели (ню)
    
    size_t iterationNumber; // Количество итераций
    double timespan; // Размер шага по времени
    Vector<double> nullState; // Начальное состояние
    double nullTime; // Начальное время
public:
    VanDerPol() : m(1.2), nullState({0, 0}),
    iterationNumber(1000), nullTime(0), timespan(0.05)
    { }
    // Класс функцтор как в 3 лабе
    Vector<double> operator() (double time, const Vector<double>& state) const;
    void setNullSettings(std::map<std::string, double>&); // Функция которая устанавливает параметры модели по ассоциативному массиву
    
    Vector<double> getNullState() const
    { return nullState; }
    double getNullTime() const
    { return nullTime; }
    double getTimespan() const
    { return timespan; }
    size_t getIterationNumber() const
    { return iterationNumber; }
};



Vector<double> VanDerPol::operator() (double time, const Vector<double>& state) const
{
    Vector<double> res;
    res.resize(2);
    res[0] = m * (state[0] - (1 / 3) * std::pow(state[0], 3) - state[1]);
    res[1] = state[0] / m;
    
    return res;
}

void VanDerPol::setNullSettings(std::map<std::string, double>& map)
{
    // Настройка общих параметров
    if (map["timespan"]) timespan = map["timespan"];
    if (map["iterationNumber"]) iterationNumber = static_cast<size_t>(map["iterationNumber"]);
    // Настройка модели
    if (map["m"]) m = map["m"];
    // Настройка нулевого состояния
    if (map["nullTime"]) nullTime = map["nullTime"];
    if (map["x0"]) nullState[0] = map["x0"];
    if (map["y0"]) nullState[1] = map["y0"];
}

                                                            */

// Класс для системы Аттрактор Лоренца
class LorenzAttractor
{
private:
    double sigma;
    double b;
    double r;
    ///////////
    size_t iterationNumber;
    double timespan;
    Vector<double> nullState;
    double nullTime;
public:
    LorenzAttractor() : r(28.0), b(8.0 / 3), sigma(10.0), nullState({7.2, 10.0, 3.4}),
    iterationNumber(1000), nullTime(0), timespan(0.05)
    { }
    
    Vector<double> operator() (double null_time, const Vector<double>& null_state) const;
    void setNullSettings(std::map<std::string, double>&);
    
    Vector<double> getNullState() const
    { return nullState; }
    double getNullTime() const
    { return nullTime; }
    double getTimespan() const
    { return timespan; }
    size_t getIterationNumber() const
    { return iterationNumber; }
};



Vector<double> LorenzAttractor::operator() (double null_time, const Vector<double>& null_state) const
{
    Vector<double> res;
    res.resize(3);
    
    res[0] = sigma * (null_state[1] - null_state[0]);
    res[1] = null_state[0] * (r - null_state[2]) - null_state[1];
    res[2] = null_state[0] * null_state[1] - b * null_state[2];
    
    return res;
}


void LorenzAttractor::setNullSettings(std::map<std::string, double>& map)
{
    // Настройка общих параметров
    if (map["timespan"]) timespan = map["timespan"];
    if (map["iterationNumber"]) iterationNumber = static_cast<size_t>(map["iterationNumber"]);
    // Настройка модели
    if (map["sigma"]) sigma = map["sigma"];
    if (map["b"]) b = map["b"];
    if (map["r"]) r = map["r"];
    // Настройка нулевого состояния
    if (map["nullTime"]) nullTime = map["nullTime"];
    if (map["x0"]) nullState[0] = map["x0"];
    if (map["y0"]) nullState[1] = map["y0"];
    if (map["z0"]) nullState[1] = map["z0"];
}
