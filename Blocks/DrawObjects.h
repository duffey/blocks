#ifndef DRAWOBJECTS_H_
#define DRAWOBJECTS_H_

/**
  * @brief This class is a toolbag of miscellaneous graphical functions.
  */
class DrawObjects
{
	public:
		//Credit: http://www.nullterminator.net/gltexture.html
		static GLuint loadRawTexture(const char * filename, int wrap)
		{
			GLuint texture;
			int width, height;
			void * data;
			FILE * file;
			
			// open texture data
			file = fopen(filename, "rb");
			if (file == NULL) return 0;
			
			// allocate buffer
			width = 512;
			height = 512;
			data = malloc(width * height * 3);
			
			// read texture data
			fread(data, width * height * 3, 1, file);
			fclose(file);
			
			// allocate a texture name
			glGenTextures(1, &texture);
			
			// select our current texture
			glBindTexture(GL_TEXTURE_2D, texture);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			
			// free buffer
			free(data);
			
			return texture;
		}
		
		static void freeTexture(GLuint texture)
		{
			glDeleteTextures(1, &texture);
		}
};

#endif /*DRAWOBJECTS_H_*/
