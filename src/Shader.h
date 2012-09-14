
#include "glew.h"
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sstream>
#include <string>



class Shader
{
public:

	typedef enum
	{
		VERTEX_SHADER = 0,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER,
		COMPUTE_SHADER,
		TESSELATION_SHADER
	} Type;

	void
	addShader(std::string shader, Type type)
	{
		_Shader s = {GLuint(), type};
		_shaders[shader] = s;
	}

	GLhandleARB 
	operator()()
	{
		return _program;
	}

	void
	use()
	{
		glUseProgram(_program);
	}

	bool
	compile()
	{
		_program = glCreateProgramObjectARB();
		std::map<std::string,_Shader>::iterator it;
		for(it = _shaders.begin(); it != _shaders.end(); ++it) {
			it->second._handle = _compileShader(it->first.c_str(),it->second._type);
			glAttachObjectARB(_program,it->second._handle);
		}
		_linkShaders(_program);

		//Get uniform locations
		std::map<std::string,GLuint>::iterator it2;
		for(it2 = _uniforms.begin(); it2 != _uniforms.end(); ++it2) {
			it2->second = glGetUniformLocation(_program,it2->first.c_str());
		}
		
		//Get attribLocations		
		for(it2 = _attribs.begin(); it2 != _attribs.end(); ++it2) {
			it2->second = glGetAttribLocation(_program,it2->first.c_str());
		}

		return true;
	}

	void
	addAttribute(std::string atr)
	{
		_attribs[atr] = GLuint();
	}

	GLuint
	getAttrib(std::string atr)
	{
		int count(_attribs.count(atr));
		if(count > 0)
			return _uniforms[atr];
		else
			return -1;
	}

	void
	addUniform(std::string uni)
	{
		_uniforms[uni] = GLuint();
	}

	GLuint
	getUniform(std::string uni)
	{
		int count(_uniforms.count(uni));
		if(count > 0)
			return _uniforms[uni];
		else
			return -1;
	}

private:

	struct _Shader
	{
		GLuint _handle;
		Type   _type;
	};

	GLhandleARB _program;
	std::map<std::string,GLuint> _attribs;
	std::map<std::string,GLuint> _uniforms;
	std::map<std::string,_Shader> _shaders;	

	

	void 
	_printError(const char *errtype, const char *errmsg) {
		fprintf(stderr, "%s: %s\n", errtype, errmsg);
	}	
	
	int 
	_filelength(const char *filename) {
		struct stat results;
		if (stat(filename, &results) == 0)
			return results.st_size;
		else
		{
			return NULL;
		}
	}

	char* 
	_readShaderFile(const char *filename) 
	{
		std::stringstream ss;
		ss << "../../src/Shaders/" << filename;
		std::string fullPath;
		ss >> fullPath;

		FILE *file = fopen( fullPath.c_str(), "r");
		if(file == NULL)
		{
			_printError("I/O error", "Cannot open shader file!");
			return 0;
		}
		int bytesinfile = _filelength(fullPath.c_str());
	
		if(bytesinfile == NULL)
		{
			_printError("I/O error", "Cannot get shader file size!");
			return 0;
		}

		unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
		int bytesread = fread( buffer, 1, bytesinfile, file);
		buffer[bytesread] = 0; // Terminate the string with 0
		fclose(file);

		return (char *)buffer;
	}

	void 
	_printShaderInfoLog(GLuint obj)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("printShaderInfoLog: %s\n",infoLog);
			free(infoLog);
		}else{
			printf("Shader Info Log: OK\n");
		}
	}

	void
	_printProgramInfoLog(GLuint obj)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("printProgramInfoLog: %s\n",infoLog);
			free(infoLog);
		}else{
			printf("Program Info Log: OK\n");
		}
	}

	GLhandleARB
	_createAndLinkShaders(int count, ...)
	{
		GLhandleARB programObj = glCreateProgramObjectARB();
		GLint shadersLinked;
		char str[4096]; // For error messages from the GLSL compiler and linker

		va_list ap;
		va_start(ap, count); //Requires the last fixed parameter (to get the address)
		for(int j = 0; j < count; j++)
			glAttachObjectARB(programObj,va_arg(ap, GLhandleARB));
		va_end(ap);

		// Link the program object and print out the info log
		glLinkProgramARB( programObj );
		glGetObjectParameterivARB( programObj, GL_OBJECT_LINK_STATUS_ARB, &shadersLinked );

		//Check if linkage was OK!
		if( shadersLinked == GL_FALSE )
		{
			glGetInfoLogARB( programObj, sizeof(str), NULL, str );
			_printError("Program object linking error", str);
		}

		//printProgramInfoLog(programObj);

		return programObj;
	}

	//----------------------------------------------------------------------------//
	// Links shaders, paramlist
	//----------------------------------------------------------------------------//
	GLhandleARB 
	_linkShaders(GLhandleARB & programObj)
	{
		GLint shadersLinked;
		char str[4096]; // For error messages from the GLSL compiler and linker

		// Link the program object and print out the info log
		glLinkProgramARB( programObj );
		glGetObjectParameterivARB( programObj, GL_OBJECT_LINK_STATUS_ARB, &shadersLinked );

		//Check if linkage was OK!
		if( shadersLinked == GL_FALSE )
		{
			glGetInfoLogARB( programObj, sizeof(str), NULL, str );
			_printError("Program object linking error", str);
		}

		return programObj;
	}

	//----------------------------------------------------------------------------//
	// Compiles a vertex shader and return the ID
	//----------------------------------------------------------------------------//
	GLhandleARB
	_compileShader(const char *shaderfilename, const Type type)
	{
	
		GLhandleARB shader;
		const char * shaderStrings[1];
		GLint compiled;
		char str[4096]; // For error messages from the GLSL compiler and linker
		char *shaderAssembly;

		//Create the shader object
		if(type ==  VERTEX_SHADER)
			shader = glCreateShaderObjectARB( GL_VERTEX_SHADER );
		else if(type == FRAGMENT_SHADER)
			shader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER );
		else if(type == GEOMETRY_SHADER)
			shader = glCreateShaderObjectARB( GL_GEOMETRY_SHADER );
		else if(type == COMPUTE_SHADER)
			shader = glCreateShaderObjectARB( GL_COMPUTE_SHADER );		

		//Build the shader
		shaderAssembly = _readShaderFile( shaderfilename );
		shaderStrings[0] = shaderAssembly;
		glShaderSource( shader, 1, shaderStrings, NULL );
		glCompileShader(shader);
		free((void *)shaderAssembly);

		//Check if compilation succeeded!
		glGetObjectParameterivARB( shader, GL_OBJECT_COMPILE_STATUS_ARB, 
			&compiled );
		if(compiled == GL_FALSE)
		{
			glGetInfoLogARB( shader, sizeof(str), NULL, str );
			if(type ==  VERTEX_SHADER)
				_printError("Vertex shader compile error", str);
			else if(type == FRAGMENT_SHADER)
				_printError("Fragment shader compile error", str);
			else if(type == GEOMETRY_SHADER)
				_printError("Geometry shader compile error", str);
			else if(type == GEOMETRY_SHADER)
				_printError("Geometry shader compile error", str);
			else if(type == GEOMETRY_SHADER)
				_printError("Geometry shader compile error", str);
		
		}

		//printShaderInfoLog(shader);

		return shader;
	
	}
};

