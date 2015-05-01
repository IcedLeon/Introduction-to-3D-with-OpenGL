#ifndef _GAMETREE_H_
#define _GAMETREE_H_
//Lib
#include "Application.h"

class Game;
class AI;

class GameTree : public Application
{
private:
	Game* m_oGame;
	AI* m_oAI;
	vec3 m_vPickPos;

	struct
	{
		unsigned int m_uiIndexCount;
		unsigned int m_uiVBO;
		unsigned int m_uiIBO;
		unsigned int m_uiVAO;
	} aBird;

	void UpdateAiMoves();
	void UpdateInput(float a_fDeltaTime);

public:
	GameTree();
	~GameTree();

	/* Inherit member functions */
	virtual void InitWindow(vec3 a_vCamPos = vec3(0.0f, -10.0f, 14.0f), vec3 a_vScreenSize = vec3(1024.0f, 780.0f, 0.0f),
		const char* a_pccWinName = WIN_NAME_PREFIX, bool a_bFullScreen = false) override;
	virtual void Update(GLdouble a_fDeltaT) override;
	virtual void Draw() override;
	virtual void Use() {}
	virtual void CleanUpWin() override;
};
#endif //!_GAMETREE_H_