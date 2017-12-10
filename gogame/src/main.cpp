
#include <iostream>
#include <cmath>
#include <stdexcept>

#include <GL/glew.h>

//#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

#include "render/GoModel.h"
#include "logic\GameState.h"

struct GameEvents
{
	bool addStone = false;
	bool pass = false;
	bool newGame = false;
	std::pair<int, int> pick = { -1, -1 };

	void reset() { newGame = false; addStone = false; pass = false; pick = { -1, -1 }; }
	bool fired() const { return (newGame || addStone || pass); }
	bool pickChanged(std::pair<int, int> newPick) const { return newPick != pick; }
};

static GameEvents events;
static render::Player player = render::Player::Black;

void mouse_button_callback(GLFWwindow*, int button, int action, int /*mods*/)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		events.addStone = true;
}

void key_callback(GLFWwindow*, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		events.pass = true;
	else 	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		events.newGame = true;
}

GLFWwindow* initGlfw()
{
	/* Initialize the library */
	if (!glfwInit())
		throw std::runtime_error("failed to init glfw");

	/* Create a windowed mode window and its OpenGL context */
	auto window = glfwCreateWindow(1000, 600, "Nanovg", NULL, NULL);
	if (nullptr == window)
		throw std::runtime_error("failed to create glfw window");

	glfwMakeContextCurrent(window);
	std::cout << glGetString(GL_VERSION) << std::endl;

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
		throw std::runtime_error("Could not init glew");

	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	return window;
}

NVGcontext* initNanoVg()
{
	auto vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (nullptr == vg)
		throw std::runtime_error("Could not init nanovg.\n");

	auto fontNormal = nvgCreateFont(vg, "sans", "font/Roboto-Regular.ttf");
	if (-1 == fontNormal)
		throw std::runtime_error("Could not init nanovg font.\n");

	auto fontBold = nvgCreateFont(vg, "bold", "font/Roboto-Bold.ttf");
	if (-1 == fontBold)
		throw std::runtime_error("Could not init nanovg font.\n");

	auto fontEmoji = nvgCreateFont(vg, "emoji", "font/NotoEmoji-Regular.ttf");
	if (-1 == fontEmoji)
		throw std::runtime_error("Could not init nanovg font.\n");

	nvgAddFallbackFontId(vg, fontNormal, fontEmoji);
	nvgAddFallbackFontId(vg, fontBold, fontEmoji);

	return vg;
}

void processPickEvent(std::pair<int, int> pick, render::GoModel& renderModel, logic::GameState& gameState)
{
	events.pick = pick;
	renderModel.board.setPickedPosition(pick.first, pick.second);

	if (pick.first < 0 || pick.second < 0)
		return;

	if (gameState.canPutStoneAtPosition(pick.first, pick.second))
		renderModel.board.setPickColor(player == render::Player::White ? render::PickColor::White : render::PickColor::Black);
	else 	
		renderModel.board.setPickColor(render::PickColor::Red);
}

void changePlayer(render::GoModel& renderModel, logic::GameState& gameState)
{
	using render::Player;
	player = (gameState.getCurrentPlayer() == logic::Player::BLACK) ? render::Player::Black : render::Player::White;
	renderModel.infos.setPlayer(player);
}

void retrieveStones(render::GoModel& renderModel, logic::GameState& gameState)
{
	renderModel.stones.clear();

	for (int x = 0; x < gameState._board._sizeX; ++x)
	{
		for (int y = 0; y < gameState._board._sizeY; ++y)
		{
			auto& stoneColor = gameState._board.stoneAt(x, y);
			if (stoneColor != logic::Stone::NONE)
			{
				renderModel.stones.emplace_back(x, y, (stoneColor == logic::Stone::BLACK) ? render::Player::Black : render::Player::White);
			}
		}
	}
}

void processEvents(std::pair<int, int> pick, render::GoModel& renderModel, logic::GameState& gameState)
{
	if (events.pickChanged(pick))
		processPickEvent(pick, renderModel, gameState);

	if (!events.fired())
		return;

	auto firedEvent = events;
	events.reset();

	if (firedEvent.addStone && pick.first >= 0 && pick.second >= 0)
	{
		if (gameState.canPutStoneAtPosition(pick.first, pick.second))
		{
			gameState.putStoneAtPosition(pick.first, pick.second);
			renderModel.stones.emplace_back(pick.first, pick.second, player);
			//renderModel.infos.setScore(12, 19);
			changePlayer(renderModel, gameState);
		}
	}
	else if (firedEvent.pass)
	{
		gameState.pass();
		changePlayer(renderModel, gameState);
		//renderModel.infos.setScore(42, 0);
	}
	else if (firedEvent.newGame)
	{
		gameState.reset();
		changePlayer(renderModel, gameState);
		retrieveStones(renderModel, gameState);
	}

	renderModel.infos.setMessage("I don't have any idea of a cool message yet :(, but I'm sure you'll have some :))");
}

int main(void)
{
	auto window = initGlfw();
	auto vg = initNanoVg();

	constexpr int boardWidth = 9;
	constexpr int boardHeight = 9;

	render::DrawContext context(*vg, boardWidth, boardHeight);
	render::GoModel renderModel(context);
	logic::GameState gameState{ boardWidth , boardHeight };

	// Loop until the user closes the window
	int winWidth, winHeight;
	double mx, my;
	int fbWidth, fbHeight;

	while (!glfwWindowShouldClose(window))
	{
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetCursorPos(window, &mx, &my);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		context.setSquareSize(render::squareSizeFromCanvasSize(winWidth, winHeight, context));
		auto pick = render::squarePosFromScreenPos(int(std::floor(mx)), int(std::floor(my)), context);

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Calculate pixel ration for hi-dpi devices.
		auto pxRatio = float(fbWidth) / float(winWidth);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
		render::draw(renderModel);
		nvgEndFrame(vg);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processEvents(pick, renderModel, gameState);
	}

	glfwTerminate();
	return 0;

}