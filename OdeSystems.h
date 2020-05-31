#pragma once
#include "MatrixVector.h"
#include <map>
#include <cmath>

/* Формат задания параметров:
 m=1.4
 nullTime=1
 
 ИмяПараметра=ЗначениеПараметра
 
 Все вводится без пробелов
 Каждый параметр на новой строке
 Порядок задания параметров не важен
 Если какая либо величина не указана в файле, то будет использоваться значение по умолчанию
 */

class BaseOde
{
protected:
    size_t iterationNumber; // Количество итераций
    double timespan; // Размер шага по времени
    Vector<double> nullState; // Начальное состояние (его настраивает только конкретное ОДУ!)
    double nullTime; // Начальное время
public:
    BaseOde(const std::initializer_list<double>& null_state) : iterationNumber(1000), nullTime(0), timespan(0.05),
    nullState(null_state)
    { }
    // Возвращает производную по вектору состояния и времени
    Vector<double> operator() (double time, const Vector<double>& state) const;
    // Установка начального положения и параметров модели
    void setNullSettings(const std::map<std::string, double>& map);
    
    inline Vector<double> getNullState() const
    {
        return nullState;
    }
    inline double getNullTime() const
    {
        return nullTime;
    }
    inline double getTimespan() const
    {
        return timespan;
    }
    inline size_t getIterationNumber() const
    {
        return iterationNumber;
    }
};

/* Параметры общие для всех ОДУ
 timespan - размер шага интегрирования
 nullTime - начальное время
 iterationNumber - количество итераций
 */

void BaseOde::setNullSettings(const std::map<std::string, double>& map)
{
    std::map<std::string, double>::const_iterator elem;
    
    if ((elem = map.find("iterationNumber")) != map.end())
        iterationNumber = (size_t)elem->second;
    
    if ((elem = map.find("nullTime")) != map.end())
        nullTime = elem->second;
    
    if ((elem = map.find("timespan")) != map.end())
        timespan = elem->second;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//                        Первая  система ОДУ - Осциллятор Ван Дер Поля
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// Класс который является ОДУ для Ван Дер Поля
class VanDerPol : public BaseOde
{
private:
    double m; // Коэффициент в модели (ню)
public:
    VanDerPol() : m(1.2), BaseOde({0, 0})
    { }
    // Класс функцтор как в 3 лабе
    Vector<double> operator() (double time, const Vector<double>& state) const
    {
        Vector<double> res;
        res.resize(2);
        res[0] = m * (state[0] - (1 / 3) * std::pow(state[0], 3) - state[1]);
        res[1] = state[0] / m;
        
        return res;
    }
    void setNullSettings(const std::map<std::string, double>&); // Функция которая устанавливает параметры модели по ассоциативному массиву
};

/* Параметры, специфичные для Асцилятора Ван Дер Поля
 x0 - начальная позиция по х
 y0 - начальная позиция по у
 m - коэффициент ню
 */

void VanDerPol::setNullSettings(const std::map<std::string, double>& map)
{
    std::map<std::string, double>::const_iterator elem;
    // Применение общих для каждой из ОДУ настроек
    BaseOde::setNullSettings(map);
    // Настройка модели
    if ((elem = map.find("m")) != map.end())
        m = elem->second;
    // Настройка начального состояния
    if ((elem = map.find("x0")) != map.end())
        nullState[0] = elem->second;
    if ((elem = map.find("y0")) != map.end())
        nullState[1] = elem->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//                        Вторая система ОДУ - Аттрактор Лоренца
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// Такой же класс только для системы Аттрактор Лоренца
class LorenzAttractor : public BaseOde
{
private:
    double sigma;
    double b;
    double r;
public:
    LorenzAttractor() : r(28.0), b(8.0 / 3), sigma(10.0), BaseOde({0, 0, 0})
    { }
    
    Vector<double> operator() (double null_time, const Vector<double>& null_state) const
    {
        Vector<double> res;
        res.resize(3);
        
        res[0] = sigma * (null_state[1] - null_state[0]);
        res[1] = null_state[0] * (r - null_state[2]) - null_state[1];
        res[2] = null_state[0] * null_state[1] - b * null_state[2];
        
        return res;
    }
    
    void setNullSettings(const std::map<std::string, double>& map);
};

/* Параметры, специфичные для Аттрактора Лоренца
 x0 - начальная позиция по х
 y0 - начальная позиция по у
 z0 - начальная позиция по z
 r - коэффициент ню
 b - коэффициент b
 sigma - коеффициент sigma (рекомендовано использовать параметры по умолчанию)
 */

void LorenzAttractor::setNullSettings(const std::map<std::string, double>& map)
{
    std::map<std::string, double>::const_iterator elem;
    
    // Применение общих для каждой из ОДУ настроек
    BaseOde::setNullSettings(map);
    // Настройка модели
    if ((elem = map.find("sigma")) != map.end())
        sigma = elem->second;
    if ((elem = map.find("b")) != map.end())
        b = elem->second;
    if ((elem = map.find("r")) != map.end())
        r = elem->second;
    // Настройка начального состояния
    if ((elem = map.find("x0")) != map.end())
        nullState[0] = elem->second;
    
    if ((elem = map.find("y0")) != map.end())
        nullState[1] = elem->second;
    
    if ((elem = map.find("z0")) != map.end())
        nullState[2] = elem->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//                        Третья система ОДУ - Цепь Чуа
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class ChupChain : public BaseOde
{
private:
    double alpha;
    double m1;
    double m0;
    double betta;
public:
    ChupChain() : BaseOde({0, 0, 0}), alpha(1.2), m1(5.0/-7), m0(8.0/-7), betta(20.7)
    { }
    // state vector {x, y, z}
    Vector<double> operator() (double null_time, const Vector<double>& null_state) const
    {
        Vector<double> res;
        res.resize(3);
        
        double h = m1 * null_state[0] + (m0 - m1)*(std::abs(null_state[0] + 1) - std::abs(null_state[0] - 1)) / 2.0;
        
        res[0] = alpha * (null_state[1] - null_state[0] - h);
        res[1] = null_state[0] - null_state[1] + null_state[2];
        res[2] = -betta * null_state[1];
        
        return res;
    }
    
    void setNullSettings(const std::map<std::string, double>& map);
};

/* Параметры, специфичные для Цепи Чуа
 x0 - начальная позиция по х
 y0 - начальная позиция по у
 z0 - начальная позиция по z
 alpha - коэффициент альфа
 m0 - коэффициент m0
 m1 - коэффициент m1
 betta - коэффициент бетта
 */

void ChupChain::setNullSettings(const std::map<std::string, double>& map)
{
    std::map<std::string, double>::const_iterator elem;
    
    // Применение общих для каждой из ОДУ настроек
    BaseOde::setNullSettings(map);
    // Настройка модели
    if ((elem = map.find("alpha")) != map.end())
        alpha = elem->second;
    if ((elem = map.find("m0")) != map.end())
        m0 = elem->second;
    if ((elem = map.find("m1")) != map.end())
        m1 = elem->second;
    if ((elem = map.find("betta")) != map.end())
        betta = elem->second;
    // Настройка начального состояния
    if ((elem = map.find("x0")) != map.end())
        nullState[0] = elem->second;
    
    if ((elem = map.find("y0")) != map.end())
        nullState[1] = elem->second;
    
    if ((elem = map.find("z0")) != map.end())
        nullState[2] = elem->second;
}
