//#ifndef _TUNNELEFFECT_H_
//#define _TUNNELEFFECT_H_
////Lib
//#include "ParticleEffect.h"
//#include <memory>
//
//class ParticleSystem;
//class ParticlesGenerators;
//class ParticlesRenderer;
//class ParticlesUpdaters;
//
//class TunnelEffect : public IEffect
//{
//private:
//	std::shared_ptr<ParticleSystem>		m_oSystem;
//	std::shared_ptr<IParticlesRenderer> m_oRenderer;
//	std::shared_ptr<RoundPosGen>		m_oPosGenerator;
//	std::shared_ptr<BasicColorGen>		m_oColGenerator;
//
//public:
//	TunnelEffect() { }
//	~TunnelEffect() { }
//
//	bool Initialize(size_t a_NumParticles) override;
//	bool InitializeRenderer() override;
//	void Reset() override;
//	void Clean() override;
//	void AddUI() override;
//	void RemoveUI() override;
//
//	void Update(double a_dDeltaTime) override;
//	void CpuUpdate(double a_dDeltaTime) override;
//	void GpuUpdate(double a_dDeltaTime) override;
//	void Render() override;
//
//	int AllParticles() override;
//	int AllAliveParticles() override;
//};
//#endif //!_TUNNELEFFECT_H_