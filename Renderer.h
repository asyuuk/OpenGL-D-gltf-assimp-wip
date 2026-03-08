#pragma once
#include<GL/glew.h>	
#include<GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<map>
#include<unordered_map>
#include<list>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Object.h"
#include"Time.h"
#include"Gltf3D.h"

class Gltf3D;

class Object;

class Time;


// GLMの名前空間を使用GLM初期化ウィンドウの初期化、テクスチャの読み込み、描画処理などを行うRendererクラスを定義します。
namespace Drawing
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void Run();
		void GL(int width, int height);//OpenGLの初期化をする。
		void InitGL();// OpenGLの初期化
		void InitWindow(int Width, int Height); //ウィンドウの初期化
		void GlewInit(); // GLEWの初期化
		void MainLoop(); // メインループ


		bool LoadTexture(const std::string& filePath, const std::string& filename); // テクスチャの読み込み

		bool DrawInit(); // テクスチャの描画初期化
		
		bool Draw3D(Gltf3D* gltf, float x, float y, float z,float size,float radian,glm::vec3 center,float cameraposx,float cameraposy,float cameraposz); // 3Dモデルの描画
		
		bool DrawMMD(GLuint vao, GLsizei indexCount, GLenum indexType, GLuint texture, glm::mat4& model, float size, float x, float y, float z, float radian, glm::vec3 center, float cameraX, float cameraY, float cameraZ,size_t offset);

		bool LoadShaderPrograms();

		void SetAspect(int width, int height); // ビューポートの設定
		std::string LoadShaderSource(const std::string& path); // シェーダーの初期化
		GLuint CompileShader(GLenum type, const std::string& source); // シェーダーのコンパイル

		bool CheckHitKey(int key); // キー入力のチェック

		bool CheckReleaseKey(int key);

	 
		GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource); // シェーダープログラムの作成

		//Object List

		std::list<Component*> objectsList; // List of game objects]
		std::vector<GLuint> textureIDs; // 読み込んだテクスチャIDのリスト
		GLuint shaderProgram; // シェーダープログラムID
		int Width = 1030; // ウィンドウの幅
		int Height = 800; // ウィンドウの高
		bool entrytrigger = false;
		bool releasetrigger = false;
		GLuint shader3D;
	private:

		GLuint shaderProgramTextures=0;//切り分けないシェーダー
		GLuint shaderProgramDivSizeTextures=0;//切り分けるシェーダー
		
		GLuint shader=0;//画像そのまま

		GLuint Titleshader=0;

		
		GLuint textureID=0; // テクスチャID

		GLuint textID=0;

		GLuint VAO=0, VBO=0;
		GLuint textVAO=0, textVBO=0;

		GLuint fbo=0;

		GLuint colorTex=0;

		int width=0, height=0, channels=0;

		float offsetX = 0.5, offsetY = 0; // テクスチャのオフセット

		GLint offsetLoc=0;
		GLint uSpriteIndex=0;
		GLint uSheetSize=0;
		GLint uTextureSize=0;
		GLint uSpriteSize=0;
		GLint uFrameX=0;
		GLint uFrameY=0;
		GLint OffsetsizeX=0;
		GLint OffsetsizeY=0;
		GLint Matrix4=0;
		GLuint ebo=0;

		GLint _time=0;
		GLint _waveamp=0;
		GLint _wavefreq=0;
		GLint _wavespeed=0;

		
		GLFWwindow* window; // GLFWウィンドウ
		float vertices[16];
		Object* Owner = new Object;
		Time* timer = new Time;
		Component* component = new Component;

		bool AddComponentsFlag = false;

		glm::vec3 center
			= {
				0,0,1
		};
	};
}
