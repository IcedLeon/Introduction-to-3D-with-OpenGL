#include "AIBase.h"

int MCTSAi::makeDecision(const Game& a_oGame)
{
	//Game::State player1;
	//Game::State player2;
	//Create the list of action,
	std::vector<int> _action;
	a_oGame.getValidActions(_action);
	std::vector<int>::iterator itr = _action.begin();

	Game* _newGame;

	for (itr; itr != _action.end(); ++itr)
	{
		//set the end value to 0.
		int _result = 0;
		//For all the max playthrough
		for (int playIdx = 0; playIdx < m_iPlayouts; ++playIdx)
		{
			//clone the current game
			_newGame = a_oGame.clone();
			//perform the action
			_newGame->performAction(*itr);

			//now need to simulate a game.
			while (_newGame->getCurrentGameState() == Game::UNKNOWN)
			{
				std::vector<int> _copyAction = _action;

				int _randAct = rand() % _copyAction.size();
				_newGame->performAction(_randAct);
				
				if (_newGame->getCurrentGameState() == Game::PLAYER_ONE)
					_result += 1;
				else if (_newGame->getCurrentGameState() == Game::PLAYER_TWO)
					_result -= 1;
			}
			delete _newGame;
		}
		return _action[_result / m_iPlayouts];
	}
}