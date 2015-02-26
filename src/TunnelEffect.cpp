#include "TunnelEffect.h"
#include "ParticleSystem.h"
#include "ParticlesGenerators.h"
#include "ParticlesRenderer.h"
#include "ParticlesUpdaters.h"
#include "ParticleEmitter.h"

bool TunnelEffect::Initialize(size_t a_NumParticles)
{
	const size_t NUM_PARTICLES = a_NumParticles == 0 ? 1000 : a_NumParticles;
	m_oSystem = std::make_shared<ParticleSystem>(NUM_PARTICLES);

	auto _particleEmitter = std::make_shared<ParticleEmitter>();
	{
		_particleEmitter->m_fEmitterRate = (float)NUM_PARTICLES * 0.45f;

		//Pos
		m_oPosGenerator = std::make_shared<RoundPosGen>();
		m_oPosGenerator->m_vCenter = vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
		m_oPosGenerator->m_fRadX = 0.15f;
		m_oPosGenerator->m_fRadY = 0.15f;
		_particleEmitter->AddGenerator(m_oPosGenerator);

		//Col
		m_oColGenerator = std::make_shared<BasicColorGen>();
		m_oColGenerator->m_vMinStartCol = vec4{ 0.7, 0.0, 0.7, 1.0 };
		m_oColGenerator->m_vMaxStartCol = vec4{ 1.0, 1.0, 1.0, 1.0 };
		m_oColGenerator->m_vMinEndCol = vec4{ 0.5, 0.0, 0.6, 0.0 };
		m_oColGenerator->m_vMaxEndCol = vec4{ 0.7, 0.5, 1.0, 0.0 };
		_particleEmitter->AddGenerator(m_oColGenerator);
		
		//Vel
		auto _velGenerator = std::make_shared<BasicVelGen>();
		_velGenerator->m_vMinStartVel = vec4{ 0.0f, 0.0f, 0.15f, 0.0f };
		_velGenerator->m_vMaxStartVel = vec4{ 0.0f, 0.0f, 0.45f, 0.0f };
		_particleEmitter->AddGenerator(_velGenerator);

		//Time
		auto _timeGenerator = std::make_shared<BasicTimeGen>();
		_timeGenerator->m_fMinTime = 1.0f;
		_timeGenerator->m_fMaxTime = 3.5f;
		_particleEmitter->AddGenerator(_timeGenerator);
	}
	m_oSystem->AddEmitter(_particleEmitter);

	auto _timeUpdater = std::make_shared<BasicTimeUpdater>();
	m_oSystem->AddUpdater(_timeUpdater);

	auto _colorUpdater = std::make_shared<BasicColourtUpdater>();
	//colorUpdater->m_minPos = glm::vec4{ -1.0f };
	//colorUpdater->m_maxPos = glm::vec4{ 1.0f };
	m_oSystem->AddUpdater(_colorUpdater);

	auto _eulerUpdater = std::make_shared<EulerUpdater>();
	_eulerUpdater->m_vGlobalAcceleration= vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_oSystem->AddUpdater(_eulerUpdater);

	return true;
}

bool TunnelEffect::InitializeRenderer()
{
	m_oRenderer = RendererFactory::Create("gl");
	m_oRenderer->Generate(m_oSystem.get(), false);

	return true;
}

void TunnelEffect::Reset()
{
	m_oSystem->Reset();
}

void TunnelEffect::Clean()
{
	if (m_oRenderer)
	{
		m_oRenderer->Destroy();
	}
}

void TunnelEffect::AddUI()
{
	
}

void TunnelEffect::RemoveUI()
{

}

void TunnelEffect::Update(double a_dDeltaTime)
{
	static double _timer = 0.0;
	_timer += a_dDeltaTime;

	m_oPosGenerator->m_vCenter.x = 0.1f * sinf((float)_timer * 2.5f);
	m_oPosGenerator->m_vCenter.y = 0.1f * cosf((float)_timer * 2.5f);
	m_oPosGenerator->m_fRadX = 0.15f + 0.05f * sin((float)_timer);
	m_oPosGenerator->m_fRadY = 0.15f + 0.05f * sin((float)_timer);
}

void TunnelEffect::CpuUpdate(double a_dDeltaTime)
{
	m_oSystem->Update(a_dDeltaTime);
}

void TunnelEffect::GpuUpdate(double a_dDeltaTime)
{
	m_oSystem->Update(a_dDeltaTime);
}

int TunnelEffect::AllParticles()
{ 
	return m_oSystem->AllTheParticles(); 
}

int TunnelEffect::AllAliveParticles() 
{
	return m_oSystem->AllTheParticlesAlive(); 
}

void TunnelEffect::Render()
{
	m_oRenderer->Render();
}