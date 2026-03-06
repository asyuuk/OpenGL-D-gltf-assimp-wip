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
#include"FBX.h"

class Object;

class FBX;

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
		
		bool Draw3D(GLuint VAO, GLsizei indicesSize, GLuint texture, float x, float y, float z,float size,float radian,glm::vec3 center,float cameraposx,float cameraposy,float cameraposz); // 3Dモデルの描画
		
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
	private:

		GLuint shaderProgramTextures;//切り分けないシェーダー
		GLuint shaderProgramDivSizeTextures;//切り分けるシェーダー
		GLuint shader3D;
		GLuint shader;//画像そのまま

		GLuint Titleshader;


		GLuint textureID; // テクスチャID

		GLuint textID;

		GLuint VAO, VBO;
		GLuint textVAO, textVBO;

		GLuint fbo;

		GLuint colorTex;

		int width, height, channels;

		float offsetX = 0.5, offsetY = 0; // テクスチャのオフセット

		GLint offsetLoc;
		GLint uSpriteIndex;
		GLint uSheetSize;
		GLint uTextureSize;
		GLint uSpriteSize;
		GLint uFrameX;
		GLint uFrameY;
		GLint OffsetsizeX;
		GLint OffsetsizeY;
		GLint Matrix4;
		GLuint ebo;

		GLint _time;
		GLint _waveamp;
		GLint _wavefreq;
		GLint _wavespeed;


		GLFWwindow* window; // GLFWウィンドウ
		float vertices[16];
		Object* Owner = new Object;
		Time* timer = new Time;
		Component* component = new Component;
		bool AddComponentsFlag = false;
		FBX* fbx = nullptr;
	};
}
