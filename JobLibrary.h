#pragma once
#include "Job.h"
#include "IntegrateJob.h"
#include "OdeParser.h"
#include "OdeSystems.h"
#include "KmeanJob.h"


// абстрактный обработчик

class Handler {}

// конкретные обработчики
class IntegrateVanDerPolHandler : public Handler{}

class IntegrateLorenzAttractorHandler : public Handler{}

class IntegrateChuaChainHandler : public Handler{}

class KmeanHandler : public Handler{}
