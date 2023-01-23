#ifndef MAINMENUVIEW_H_
#define MAINMENUVIEW_H_

#include "AbstractView.h"
#include "ScrollingButtonMenu.h"


bool pathset;
string path;

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  * This is the basic View for selecting a game to play.
  */
class MainMenuView : public AbstractView
{
	private:
		const ScrollingButtonMenu& menu;
		
	public:


		MainMenuView(const GLfloat& windowWidth, const GLfloat& windowHeight, const ScrollingButtonMenu& menu) : AbstractView(windowWidth, windowHeight), menu(menu) {}

		virtual void display() const
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);

			glMatrixMode (GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity ();
				
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

				glLoadIdentity();
				glOrtho(0, getWindowWidth(), 0, getWindowHeight(), -1, 1);
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
				glEnable(GL_BLEND);
				
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					
					//menu.draw();

					
					ImGui_ImplOpenGL3_NewFrame();
					ImGui_ImplGlfw_NewFrame();
					ImGui::NewFrame();


					static int counter = 0;

					ImGui::Begin("Levels");                          // Create a window called "Hello, world!" and append into it.


					WIN32_FIND_DATA ffd;
					HANDLE hFind = INVALID_HANDLE_VALUE;
					LARGE_INTEGER filesize;
					hFind = FindFirstFile(L"puzzles\\*", &ffd);


					do
					{
						if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
						}
						else {
							filesize.LowPart = ffd.nFileSizeLow;
							filesize.HighPart = ffd.nFileSizeHigh;

							wstring ws(ffd.cFileName);
							string str(ws.begin(), ws.end());
							size_t dot = str.find_last_of(".");
							string nameWithoutExt = str.substr(0, dot);

					

							if (ImGui::Button(nameWithoutExt.c_str())) {

					
								pathset = true;
								path = string("puzzles/") + str;
								//path = string("puzzles/") + str;
					
								//blockDriver->reset();
								//blockDriver->loadBlockStructure(new BlockStructure(string("puzzles/") + str, 30.0, Vector4(0.0, 50.0, 0.0, 1.0)));
							}

						}


					} while (FindNextFile(hFind, &ffd) != 0);

					
					ImGui::End();


					ImGui::Render();
					int display_w, display_h;
					//glfwGetFramebufferSize(window, &display_w, &display_h);
					glViewport(0, 0, 640, 480);
					ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
					glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
					glClear(GL_COLOR_BUFFER_BIT);
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					
			
				
				glDisable(GL_BLEND);
				glDepthFunc(GL_LEQUAL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
				
			glPopMatrix();
				
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
};

#endif /*MAINMENUVIEW_H_*/
