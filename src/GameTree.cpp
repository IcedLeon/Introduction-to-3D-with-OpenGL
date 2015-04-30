#include "GameTree.h"
#include <GLFW\glfw3.h>
#include "Vertex.h"
#include "Camera.h"
#include "AIBase.h"
#include "ConnectFour.h"
#include "Gizmos.h"

GameTree::GameTree()
{

}

GameTree::~GameTree()
{

}

void GameTree::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.7f, 0.7f, 0.7f, 0.7f);

	m_oGame->draw();

	//for (int i = 0; i <= 20; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
	//		i == 10 ? vec4(1, 0, 0, 1) : vec4(0, 0, 0, 1));
	//	Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
	//		i == 10 ? vec4(0, 1, 0, 1) : vec4(0, 0, 0, 1));
	//}
	mat4 _projView = m_oProjection * m_oView;
	Gizmos::draw(_projView);
}

void GameTree::Update(GLdouble a_fDeltaT)
{
	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));
	/* View */
	m_oView = m_oCamera.GetViewTransform();
	UpdateAiMoves();
	UpdateInput((float)a_fDeltaT);
}

void GameTree::CleanUpWin()
{
	delete m_oAI;
	delete m_oGame;
	Gizmos::destroy();
	Application::CleanUpWin();
}

void GameTree::UpdateAiMoves()
{
	static bool _madeMove = false;

	if (m_oGame->getCurrentGameState() == Game::UNKNOWN)
	{
		if (m_oGame->getCurrentPlayer() == Game::PLAYER_ONE)
		{
			if (m_bKeys[GLFW_MOUSE_BUTTON_RIGHT])
			{
				if (!_madeMove)
				{
					_madeMove = !_madeMove;

					double _x = 0, _y = 0;

					glfwGetCursorPos(m_oWin, &_x, &_y);

					m_vPickPos = m_oCamera.PickAgainstPlain((float)_x, (float)_y, m_vScreenSize.xy(), vec4(0, 1, 0, 0));

					int _col = (int)((m_vPickPos.z + ConnectFour::COLUMNS) / 2);

					if (m_oGame->isActionValid(_col))
						m_oGame->performAction(_col);
				}
			}
			else
			{
				_madeMove = !_madeMove;
			}
		}
		else
		{
			m_oGame->performAction(m_oAI->makeDecision(*m_oGame));
		}
	}
}

void GameTree::UpdateInput(float a_fDeltaTime)
{
	/* Camera controls */
	if (m_bKeys[GLFW_KEY_W])
		m_oCamera.KeyboardInput(FORWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_S])
		m_oCamera.KeyboardInput(BACKWARD, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_A])
		m_oCamera.KeyboardInput(LEFT, a_fDeltaTime);
	if (m_bKeys[GLFW_KEY_D])
		m_oCamera.KeyboardInput(RIGHT, a_fDeltaTime);

	if (m_bKeys[GLFW_KEY_TAB])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_bKeys[GLFW_KEY_LEFT_SHIFT])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GameTree::InitWindow(vec3 a_vCamPos, vec3 a_vScreenSize, const char* a_pccWinName, bool a_bFullScreen)
{
	Application::InitWindow(a_vCamPos, a_vScreenSize, a_pccWinName, a_bFullScreen);
	m_oProjection = m_oCamera.GetProjectionTransform(m_vScreenSize.xy());

	m_oGame = new ConnectFour();
	m_oAI = new MCTSAi(400);

	Gizmos::create();
}