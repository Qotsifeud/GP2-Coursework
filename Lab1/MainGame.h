#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Audio.h"

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void linkBumpMapping();
	void linkFogShader();
	void linkToon(Transform transform);
	void linkRimShader(Transform transform);
	void linkWhat(Transform transform);
	bool collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad, glm::vec3 m3Pos, float m3Rad);
	void playAudio(unsigned int Source, glm::vec3 pos);
	

	glm::vec3 forwardDirection;
	glm::vec3 backwardDirection;
	glm::vec3 currentPos;
	glm::vec3 currentRot;
	Display _gameDisplay;
	GameState _gameState;
	Mesh mesh1;
	Mesh mesh2;
	Mesh mesh3;
	Mesh mesh4;
	Camera myCamera;
	Shader fogShader;
	Shader toonShader;
	Shader rimShader;
	Shader bump;
	Shader noBump;
	Shader what;

	Texture texture1;
	Texture texture2;
	Texture texture3;
	Texture bumpMapping;
	
	Audio audioDevice;

	float counter;
	unsigned int bang;
	unsigned int backGroundMusic;
	bool missileActive;

	const Uint8* key;
	Uint32 currentFrame;
	float deltaTime;
	Uint32 lastFrame;
};

