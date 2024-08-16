#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>


Transform transform1;
Transform transform2;
Transform transform3;

MainGame::MainGame()
{
	_gameState = GameState::PLAY;
	Display* _gameDisplay = new Display(); //new display
	Audio* audioDevice();
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay(); 
	bang = audioDevice.loadSound("..\\res\\bang.wav");
	backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	
	mesh1.loadModel("..\\res\\spaceship.obj");
	mesh2.loadModel("..\\res\\Ball.obj");
	mesh3.loadModel("..\\res\\rocket.obj");
	fogShader.init("..\\res\\fogShader.vert", "..\\res\\fogShader.frag"); //new shader
	toonShader.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag"); //new shader
	rimShader.init("..\\res\\Rim.vert", "..\\res\\Rim.frag");
	what.init("..\\res\\what.vert", "..\\res\\what.frag");
	bump.init("..\\res\\bump.vert", "..\\res\\bump.frag");
	noBump.init("..\\res\\noBump.vert", "..\\res\\noBump.frag");
	texture1.load ("..\\res\\Water.jpg"); //load texture
	texture2.load("..\\res\\rock.jpg"); //load texture
	texture3.load("..\\res\\white.jpg");
	bumpMapping.loadNormals("..\\res\\n.jpg");

	myCamera.initCamera(glm::vec3(2, 0, -4), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	transform2.SetPos(glm::vec3(5.0, 0, 0.0));
	transform2.SetRot(glm::vec3(0.0, 5.0, 0.0));
	transform2.SetScale(glm::vec3(0.03, 0.03, 0.03));

	transform3.SetPos(glm::vec3(1.0, 0.0, 0.0));
	transform3.SetRot(glm::vec3(1.0, 5.0, 7.0));
	transform3.SetScale(glm::vec3(0.0001f, 0.0001f, 0.0001f));

	counter = 1.0f;
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		drawGame();
		collision(mesh1.getSpherePos(), mesh1.getSphereRadius(), mesh2.getSpherePos(), mesh2.getSphereRadius(), mesh3.getSpherePos(), mesh3.getSphereRadius());
		playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while(SDL_PollEvent(&evnt)) //get and process events
	{
		SDL_PumpEvents();

		currentFrame = SDL_GetTicks();
		
		deltaTime = (currentFrame - lastFrame) / 1000.0f;
		
		lastFrame = currentFrame;

		key = SDL_GetKeyboardState(NULL);

		switch (evnt.type)
		{
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
			case SDL_KEYDOWN:

				forwardDirection = transform2.GetModel() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
				currentPos = *transform2.GetPos();
				currentRot = *transform2.GetRot();

				if (key[SDL_SCANCODE_UP])
				{
					transform2.SetPos(currentPos + forwardDirection * deltaTime * 8.0f);
				}
				else if (key[SDL_SCANCODE_DOWN])
				{
					transform2.SetPos(currentPos - forwardDirection * deltaTime * 5.0f);
				}
				else if (key[SDL_SCANCODE_RIGHT])
				{
					transform2.SetRot(currentRot - glm::vec3(0.0f, 1.0f * deltaTime, 0.0f));
				}
				else if (key[SDL_SCANCODE_LEFT])
				{
					transform2.SetRot(currentRot + glm::vec3(0.0f, 1.0f * deltaTime, 0.0f));
				}
				else if (key[SDL_SCANCODE_SPACE])
				{
					missileActive = true;
					forwardDirection = transform2.GetModel() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
					currentPos = *transform2.GetPos();
					currentPos.x = currentPos.x - 0.3;
					transform3.SetPos(currentPos);
				}
				
		}
	}
}


bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad, glm::vec3 m3Pos, float m3Rad)
{
	float shipPlanetDist = glm::sqrt((m2Pos.x - m1Pos.x)*(m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y)*(m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z)*(m2Pos.z - m1Pos.z));
	float missilePlanetDist = glm::sqrt((m2Pos.x - m3Pos.x) * (m2Pos.x - m3Pos.x) + (m2Pos.y - m3Pos.y) * (m2Pos.y - m3Pos.y) + (m2Pos.z - m3Pos.z) * (m2Pos.z - m3Pos.z));

	if (shipPlanetDist < (m1Rad + m2Rad))
	{
		audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		playAudio(bang, m1Pos);
		transform2.SetPos(currentPos - forwardDirection * deltaTime);
		return true;
	}
	else if (missilePlanetDist < (m2Rad + m3Rad) && missileActive)
	{
		audioDevice.setlistener(myCamera.getPos(), m1Pos);
		playAudio(bang, m1Pos);
		missileActive = false;
		transform3.SetPos(glm::vec3(1.0, 0, 0));
		return true;
	}
	else
	{
		return false;
	}
}

void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
{
	
	ALint state; 
	alGetSourcei(Source, AL_SOURCE_STATE, &state);
	/*
	Possible values of state
	AL_INITIAL
	AL_STOPPED
	AL_PLAYING
	AL_PAUSED
	*/
	if (AL_PLAYING != state)
	{
		audioDevice.playSound(Source, pos);
	}
}

void MainGame::linkRimShader(Transform transform)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	rimShader.setMat4("modelMatrix", transform.GetModel());
	//rimShader.setMat4("viewMatrix", myCamera.getView());
	rimShader.setFloat("rimPower", 3.0f);
	rimShader.setVec3("rimColor", glm::vec3(0.8f, 0.0f, 0.0f));
	rimShader.setVec3("camPos", myCamera.getPos());
}

void MainGame::linkFogShader()
{
	//fogShader.setMat4("u_pm", myCamera.getProjection());
	//fogShader.setMat4("u_vm", myCamera.getProjection());
	fogShader.setFloat("maxDist", 20.0f);
	fogShader.setFloat("minDist", 0.0f);
	fogShader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
}

void MainGame::linkToon(Transform transform)
{
	toonShader.setMat4("modelMatrix", transform.GetModel());
	toonShader.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
}

void MainGame::linkWhat(Transform transform)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	what.setVec3("cameraPosition", myCamera.getPos());
	what.setFloat("atmosphereRadius", 10);
	what.setFloat("planetRadius", 5.5);
	what.setVec3("planetCenter", *transform.GetPos());
	what.setVec3("atmosphereColor", glm::vec4(0.8,1.0,1.0,1.0));
}

void MainGame::linkBumpMapping()
{
	GLuint t1L = glGetUniformLocation(bump.getID(), "diffuse"); //texture 1 location
	GLuint t2L = glGetUniformLocation(bump.getID(), "normalT");

	//set textures
	glActiveTexture(GL_TEXTURE0); //set acitve texture unit
	glBindTexture(GL_TEXTURE_2D, texture2.getID());
	glUniform1i(t1L, 0);

	glActiveTexture(GL_TEXTURE1); //set acitve texture unit
	glBindTexture(GL_TEXTURE_2D, bumpMapping.getID());
	glUniform1i(t2L, 1);
}
void MainGame::drawGame()
{

	_gameDisplay.clearDisplay(0.0f, 0.0f, 0.1f, 1.0f); //sets our background colour

	transform1.SetPos(glm::vec3(1.0, 0.0, 0.0));
	transform1.SetRot(glm::vec3(0.0, 1 * counter, 0.0));
	transform1.SetScale(glm::vec3(1.5, 1.5, 1.5));

	if (missileActive)
	{
		currentPos = *transform3.GetPos();
		transform3.SetPos(currentPos + forwardDirection * deltaTime * 5.0f);
	}

	bump.Bind();
	linkBumpMapping();
	bump.Update(transform1, myCamera);
	mesh2.updateSphereData(*transform1.GetPos(), 1);
	mesh2.draw();

	toonShader.Bind();
	texture1.Bind(0);
	linkToon(transform2);
	toonShader.Update(transform2, myCamera);
	mesh1.updateSphereData(*transform2.GetPos(), 1);
	mesh1.draw();

	bump.Bind();
	linkBumpMapping();
	bump.Update(transform3, myCamera);
	mesh3.updateSphereData(*transform3.GetPos(), 1);
	mesh3.draw();

	currentPos = *transform2.GetPos();

	myCamera.setPos(currentPos);
	myCamera.setLook(currentPos);
	counter = counter + 0.0001f;

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	_gameDisplay.swapBuffer();
} 