#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>	//string and rfind
#include <iostream>	//cout
#include <memory>	//auto_ptr
#include <cassert>	//assert

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include "ScrollingButtonMenu.h"
#include "NullCommand.h"
#include "LoadBlockStructureCommand.h"
#include "MainMenuView.h"
#include "BlocksView.h"
#include "DebugBlocksView.h"
#include "Vector4.h"

namespace fs = boost::filesystem;

using namespace std;

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  * This is the controller which drives the main operation of the program (i.e. switching views when necessary, taking in mouse and keyboard input, etc.).
  */
class Controller
{
	private:
		static const GLfloat paddingRatio;
		static const GLfloat blockSize;
		static const Vector4 base;
		static const ScrollingButtonMenu::size_type visibleButtons = 5;
		static const fs::path blockStructurePath;
		static const string extension;

		bool mainMenuEnabled, debugViewEnabled;
		GLfloat originalWindowWidth, originalWindowHeight, currentWindowWidth, currentWindowHeight;
		BlockDriver blockDriver;
		auto_ptr<BlockDriver::Laser> laser;
		ScrollingButtonMenu menu;
		AbstractView* view;

	public:
		Controller(	const GLfloat& windowWidth,
					const GLfloat& windowHeight) :
		
					mainMenuEnabled(true),
					debugViewEnabled(false),

					originalWindowWidth(windowWidth), originalWindowHeight(windowHeight),
					currentWindowWidth(windowWidth), currentWindowHeight(windowHeight),


					//x, y
					menu(0, windowHeight,
					//buttonWidth
					windowWidth - (2.0 * windowHeight * paddingRatio),
					//buttonHeight (We need to account for the scroll buttons when computing the button height.)
					(windowHeight - ((visibleButtons + 3) * windowHeight * paddingRatio)) / (GLfloat)(visibleButtons + 2),
					//visibleButtons
					visibleButtons,
					//padding
					windowHeight * paddingRatio),

					view(new MainMenuView(windowWidth, windowHeight, menu))
		{
				assert(fs::exists(blockStructurePath) && fs::is_directory(blockStructurePath));

				//Adopted from Boost Filesystem tutorial
				fs::directory_iterator end_iter;

				for (fs::directory_iterator dir_itr(blockStructurePath); dir_itr != end_iter; ++dir_itr)
				{
					try
					{
						if (fs::is_regular_file(dir_itr -> status()))
						{
							string file = (dir_itr -> path().filename()).string();

							if(file.rfind(extension, file.size()) != string::npos)
								menu.add(Button(new LoadBlockStructureCommand(blockDriver, dir_itr -> path().string(), blockSize, base), file.substr(0, file.size() - extension.size())));
						}
					}
					catch ( const std::exception & ex ) { std::cout << dir_itr -> path().filename() << " " << ex.what() << std::endl; }
				}
		}

		void sendKeyPress(unsigned char key)
		{
			switch(key)
			{
				case 'q':				if(blockDriver.isLoaded())
										{
											delete view;
											view = new MainMenuView(originalWindowWidth, originalWindowHeight, menu);

											mainMenuEnabled = true;
											laser.reset();
											blockDriver.unload();
										}
										break;

				case GLUT_KEY_F2:		debugViewEnabled = !debugViewEnabled;
					
										if(blockDriver.isLoaded())
										{
											delete view;

											if(debugViewEnabled)
												view = new DebugBlocksView(originalWindowWidth, originalWindowHeight, blockDriver, *laser.get());
											else
												view = new BlocksView(originalWindowWidth, originalWindowHeight, blockDriver, *laser.get());
										}
										break;

				case 'p':				if(laser.get() != 0 && blockDriver.isLoaded()) laser -> setMobile(!laser -> isMobile());
										break;

				case GLUT_KEY_UP:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::UP);
										break;

				case GLUT_KEY_DOWN:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::DOWN);
										break;

				case GLUT_KEY_LEFT:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::LEFT);
										break;

				case GLUT_KEY_RIGHT:	if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::RIGHT);
										break;
			}
		}

		void update()
		{
			if(blockDriver.isLoaded())
			{
				//We already have a game in progress.
				if(laser.get() != 0)
				{
					if(laser -> isMobile())
					{
						laser -> move();
					}
					//Return to the main menu if we are not in debug mode
					else if(!debugViewEnabled && laser -> isIdle())
					{
						delete view;
						view = new MainMenuView(originalWindowWidth, originalWindowHeight, menu);

						mainMenuEnabled = true;
						laser.reset();
						blockDriver.unload();
					}
				}
				//We must have just loaded a block structure, so let's get a new laser.
				else
				{
					laser.reset();
					laser = blockDriver.getLaser();
					mainMenuEnabled = false;

					delete view;

					if(debugViewEnabled)
						view = new DebugBlocksView(originalWindowWidth, originalWindowHeight, blockDriver, *laser.get());
					else
						view = new BlocksView(originalWindowWidth, originalWindowHeight, blockDriver, *laser.get());
				}
			}
		}

		void display()
		{
			view -> display();
		}

		void sendMouseUp(GLfloat mouseX, GLfloat mouseY)
		{
			if(!blockDriver.isLoaded())
				menu.sendMouseUp(mouseX * (originalWindowWidth / currentWindowWidth), mouseY * (originalWindowHeight / currentWindowHeight));
		}
		
		void sendMouseDown(GLfloat mouseX, GLfloat mouseY)
		{
			if(!blockDriver.isLoaded())
				menu.sendMouseDown(mouseX * (originalWindowWidth / currentWindowWidth), mouseY * (originalWindowHeight / currentWindowHeight));
		}

		void setWindowWidth(const GLfloat& windowWidth) { currentWindowWidth = windowWidth; }

		void setWindowHeight(const GLfloat& windowHeight) { currentWindowHeight = windowHeight; }

		const GLfloat& getWindowWidth() const { return currentWindowWidth; }

		const GLfloat& getWindowHeight() const { return currentWindowHeight; }

		bool isMainMenuEnabled() const { return mainMenuEnabled; }
};

const GLfloat Controller::paddingRatio = .02;
const GLfloat Controller::blockSize = 30.0;
const Vector4 Controller::base = Vector4(0.0, 50.0, 0.0, 1.0);
const fs::path Controller::blockStructurePath = fs::system_complete("puzzles");
const string Controller::extension = ".block";

#endif /*CONTROLLER_H_*/
