#include "Texture.h"
#include <string>

Texture::Texture(const char* path, GLenum textureIndex, GLenum targetTexType, const bool isCubemap)
	:targetTexType(targetTexType), textureIndex(textureIndex)
{
	// load and create a texture 
	glGenTextures(1, &texId);
	glActiveTexture(textureIndex);
	glBindTexture(targetTexType, texId);
	// set the texture wrapping parameters
	glTexParameteri(targetTexType, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(targetTexType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(targetTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(targetTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	if (isCubemap)
	{
		for (int i = 0; i < 6; i++)
		{
			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
			std::string img_path = path + std::to_string(i);
			img_path += ".png";
			unsigned char* data = stbi_load(img_path.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
		}
	}
	else
	{
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(targetTexType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(targetTexType);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
}

void Texture::Bind() const
{
	glActiveTexture(textureIndex);
	glBindTexture(targetTexType, texId);
}