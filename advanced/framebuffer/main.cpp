#include <cmath>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ディレクトリの設定ファイル
#include "common.h"

static int WIN_WIDTH   = 500;                       // ウィンドウの幅
static int WIN_HEIGHT  = 500;                       // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

static const double PI = 4.0 * std::atan(1.0);

// シェーダファイル
static std::string VERT_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.vert";
static std::string FRAG_SHADER_FILE = std::string(SHADER_DIRECTORY) + "render.frag";

// テクスチャファイル
static std::string TEX_FILE = std::string(DATA_DIRECTORY) + "clock_board.png";
GLuint textureId;

// 頂点オブジェクト
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec2 &texcoord_)
        : position(position_)
        , texcoord(texcoord_) {
    }

    glm::vec3 position;
    glm::vec2 texcoord;
};

static const glm::vec3 positions[8] = {
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3( 1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3( 1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f,  1.0f,  1.0f)
};

static const glm::vec2 texcoords[2][3] = {
    { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) }
};

static const unsigned int faces[12][3] = {
    { 7, 4, 1 }, { 7, 1, 6 },
    { 2, 4, 7 }, { 2, 7, 5 },
    { 5, 7, 6 }, { 5, 6, 3 },
    { 4, 2, 0 }, { 4, 0, 1 },
    { 3, 6, 1 }, { 3, 1, 0 },
    { 2, 5, 3 }, { 2, 3, 0 }
};

// バッファを参照する番号
struct NeedleVao {
    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    size_t indexBufferSize;
} needleVao;

struct CubeVao {    
    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    size_t indexBufferSize;
} cubeVao;

struct PlaneVao {
    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    size_t indexBufferSize;
} planeVao;

// シェーダを参照する番号
GLuint programId;

// 立法体の回転角度
static float theta = 0.0f;
static float phi = 0.0f;

// VAOの初期化
void initVAO() {
    // 針のVAO
    {
        // Vertex配列の作成
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3( 0.0f,  0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3( 0.1f,  0.1f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3(-0.1f,  0.1f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( 0.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f))
        };

        std::vector<unsigned int> indices = { 0, 1, 3, 0, 3, 2 };

        // VAOの作成
        glGenVertexArrays(1, &needleVao.vaoId);
        glBindVertexArray(needleVao.vaoId);

        // 頂点バッファの作成
        glGenBuffers(1, &needleVao.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, needleVao.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // 頂点バッファの有効化
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

        // 頂点番号バッファの作成
        glGenBuffers(1, &needleVao.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, needleVao.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        // 頂点番号バッファのサイズ
        needleVao.indexBufferSize = indices.size();

        // VAOをOFFにしておく
        glBindVertexArray(0);        
    }

    // 平面のVAO
    {
        // Vertex配列の作成
        std::vector<Vertex> vertices = {
            Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
            Vertex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
            Vertex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
            Vertex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f))
        };

        std::vector<unsigned int> indices = { 0, 1, 3, 0, 3, 2 };

        // VAOの作成
        glGenVertexArrays(1, &planeVao.vaoId);
        glBindVertexArray(planeVao.vaoId);

        // 頂点バッファの作成
        glGenBuffers(1, &planeVao.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, planeVao.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // 頂点バッファの有効化
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

        // 頂点番号バッファの作成
        glGenBuffers(1, &planeVao.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeVao.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        // 頂点番号バッファのサイズ
        planeVao.indexBufferSize = indices.size();

        // VAOをOFFにしておく
        glBindVertexArray(0);
    }

    // 立方体のVAO
    {
        // Vertex配列の作成
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        int idx = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                Vertex v(positions[faces[i * 2 + 0][j]], texcoords[0][j]);
                vertices.push_back(v);
                indices.push_back(idx++);
            }

            for (int j = 0; j < 3; j++) {
                Vertex v(positions[faces[i * 2 + 1][j]], texcoords[1][j]);
                vertices.push_back(v);
                indices.push_back(idx++);
            }
        }

        // VAOの作成
        glGenVertexArrays(1, &cubeVao.vaoId);
        glBindVertexArray(cubeVao.vaoId);

        // 頂点バッファの作成
        glGenBuffers(1, &cubeVao.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVao.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // 頂点バッファの有効化
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

        // 頂点番号バッファの作成
        glGenBuffers(1, &cubeVao.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVao.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        // 頂点番号バッファのサイズ
        cubeVao.indexBufferSize = indices.size();

        // VAOをOFFにしておく
        glBindVertexArray(0);
    }
}

GLuint compileShader(const std::string &filename, GLuint type) {
    // シェーダの作成
    GLuint shaderId = glCreateShader(type);
    
    // ファイルの読み込み
    std::ifstream reader;
    size_t codeSize;
    std::string code;

    // ファイルを開く
    reader.open(filename.c_str(), std::ios::in);
    if (!reader.is_open()) {
        // ファイルを開けなかったらエラーを出して終了
        fprintf(stderr, "Failed to load a shader: %s\n", VERT_SHADER_FILE.c_str());
        exit(1);
    }

    // ファイルをすべて読んで変数に格納 (やや難)
    reader.seekg(0, std::ios::end);             // ファイル読み取り位置を終端に移動 
    codeSize = reader.tellg();                  // 現在の箇所(=終端)の位置がファイルサイズ
    code.resize(codeSize);                      // コードを格納する変数の大きさを設定
    reader.seekg(0);                            // ファイルの読み取り位置を先頭に移動
    reader.read(&code[0], codeSize);            // 先頭からファイルサイズ分を読んでコードの変数に格納

    // ファイルを閉じる
    reader.close();

    // コードのコンパイル
    const char *codeChars = code.c_str();
    glShaderSource(shaderId, 1, &codeChars, NULL);
    glCompileShader(shaderId);

    // コンパイルの成否を判定する
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        // コンパイルが失敗したらエラーメッセージとソースコードを表示して終了
        fprintf(stderr, "Failed to compile a shader!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetShaderInfoLog(shaderId, logLength, &length, &errMsg[0]);

            // エラーメッセージとソースコードの出力
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            fprintf(stderr, "%s\n", code.c_str());
        }
        exit(1);
    }

    return shaderId;
}

GLuint buildShaderProgram(const std::string &vShaderFile, const std::string &fShaderFile) {
    // シェーダの作成
    GLuint vertShaderId = compileShader(vShaderFile, GL_VERTEX_SHADER);
    GLuint fragShaderId = compileShader(fShaderFile, GL_FRAGMENT_SHADER);
    
    // シェーダプログラムのリンク
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    
    // リンクの成否を判定する
    GLint linkState;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkState);
    if (linkState == GL_FALSE) {
        // リンクに失敗したらエラーメッセージを表示して終了
        fprintf(stderr, "Failed to link shaders!\n");

        // エラーメッセージの長さを取得する
        GLint logLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            // エラーメッセージを取得する
            GLsizei length;
            std::string errMsg;
            errMsg.resize(logLength);
            glGetProgramInfoLog(programId, logLength, &length, &errMsg[0]);

            // エラーメッセージを出力する
            fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
        }
        exit(1);
    }
    
    // シェーダを無効化した後にIDを返す
    glUseProgram(0);
    return programId;
}

// シェーダの初期化
void initShaders() {
    programId = buildShaderProgram(VERT_SHADER_FILE, FRAG_SHADER_FILE);
}

// テクスチャの初期化
void initTexture() {
    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(TEX_FILE.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", TEX_FILE.c_str());
        exit(1);
    }

    // テクスチャの生成と有効化
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // 単純なテクスチャの転送
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    // テクスチャの画素値参照方法の設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャ境界の折り返し設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャの無効化
    glBindTexture(GL_TEXTURE_2D, 0);

    // ロードした画素情報の破棄
    stbi_image_free(bytes);
    
}

GLuint fboId;
GLuint colorTextureId;

void initFBO() {
    glGenTextures(1, &colorTextureId);
    glBindTexture(GL_TEXTURE_2D, colorTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureId, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// OpenGLの初期化関数
void initializeGL() {
    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // VAOの初期化
    initVAO();

    // シェーダの用意
    initShaders();

    // テクスチャの用意
    initTexture();

    // FBOの初期化
    initFBO();
}

// OpenGLの描画関数
void paintGL() {
    // 時間の計測
    time_t t = time(0);
    struct tm *now = localtime(&t);

    // 時計版の描画
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ビューポート変換の取得
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1024, 1024);

    glDisable(GL_DEPTH_TEST);
    {
        // シェーダの有効化
        glUseProgram(programId);
    
        // Uniform変数の転送
        glm::mat4 mvpMat(1.0f);

        GLuint uid;
        uid = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        uid = glGetUniformLocation(programId, "u_texture");
        glUniform1i(uid, 0);
        uid = glGetUniformLocation(programId, "u_useTexture");
        glUniform1i(uid, 1);

        // VAOの有効化
        glBindVertexArray(planeVao.vaoId);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, planeVao.indexBufferSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);    
    }

    // 短針の描画
    {
        // シェーダの有効化
        glUseProgram(programId);
    
        // Uniform変数の転送
        float rot = -now->tm_hour * (2.0 * PI) / 12.0f
                    -now->tm_min * (2.0 * PI) / (60.0f * 12.0f); 

        glm::mat4 mvpMat(1.0f);
        mvpMat = glm::rotate(mvpMat, rot, glm::vec3(0.0f, 0.0f, 1.0f));
        mvpMat = glm::scale(mvpMat, glm::vec3(0.5f, 0.6f, 1.0f));

        GLuint uid;
        uid = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
        uid = glGetUniformLocation(programId, "u_useTexture");
        glUniform1i(uid, 0);
        uid = glGetUniformLocation(programId, "u_needleColor");
        glUniform3f(uid, 0.0f, 0.5f, 1.0f);
    
        // VAOの有効化
        glBindVertexArray(needleVao.vaoId);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, needleVao.indexBufferSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);    
    }

    // 長針の描画
    {
        // シェーダの有効化
        glUseProgram(programId);
    
        // Uniform変数の転送
        float rot = -now->tm_min * (2.0 * PI) / 60.0f;
                    -now->tm_sec * (2.0 * PI) / (60.0f * 60.0f); 

        glm::mat4 mvpMat(1.0f);
        mvpMat =glm::rotate(mvpMat, rot, glm::vec3(0.0f, 0.0f, 1.0f));        
        mvpMat = glm::scale(mvpMat, glm::vec3(0.5f, 0.9f, 1.0f));

        GLuint uid;
        uid = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
        uid = glGetUniformLocation(programId, "u_useTexture");
        glUniform1i(uid, 0);
        uid = glGetUniformLocation(programId, "u_needleColor");
        glUniform3f(uid, 0.0f, 0.0f, 1.0f);
    
        // VAOの有効化
        glBindVertexArray(needleVao.vaoId);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, needleVao.indexBufferSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);    
    }

    // 秒針の描画
    {
        // シェーダの有効化
        glUseProgram(programId);
    
        // Uniform変数の転送
        float rot = -now->tm_sec * (2.0 * PI) / 60.0f;

        glm::mat4 mvpMat(1.0f);
        mvpMat = glm::rotate(mvpMat, rot, glm::vec3(0.0f, 0.0f, 1.0f));
        mvpMat = glm::scale(mvpMat, glm::vec3(0.2f, 0.9f, 1.0f));

        GLuint uid;
        uid = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
        uid = glGetUniformLocation(programId, "u_useTexture");
        glUniform1i(uid, 0);
        uid = glGetUniformLocation(programId, "u_needleColor");
        glUniform3f(uid, 0.0f, 0.8f, 1.0f);
    
        // VAOの有効化
        glBindVertexArray(needleVao.vaoId);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, needleVao.indexBufferSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);    
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    // 立方体の描画
    {
        // ビューポートを戻す
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // 背景色の描画
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // シェーダの有効化
        glUseProgram(programId);

        // 座標の変換
        glm::mat4 projMat = glm::perspective(45.0f,
            (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 viewMat = glm::lookAt(glm::vec3(3.0f, 4.0f, 5.0f),   // 視点の位置
                                        glm::vec3(0.0f, 0.0f, 0.0f),   // 見ている先
                                        glm::vec3(0.0f, 1.0f, 0.0f));  // 視界の上方向

        glm::mat4 modelMat(1.0f);
        modelMat = glm::rotate(modelMat, theta, glm::vec3(0.0f, 1.0f, 0.0f)); 
        modelMat = glm::rotate(modelMat, theta * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 mvpMat = projMat * viewMat * modelMat;

        GLuint uid;
        uid = glGetUniformLocation(programId, "u_mvpMat");
        glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTextureId);
        uid = glGetUniformLocation(programId, "u_texture");
        glUniform1i(uid, 0);
        uid = glGetUniformLocation(programId, "u_useTexture");
        glUniform1i(uid, 1);
    
        // VAOの有効化
        glBindVertexArray(cubeVao.vaoId);

        // 三角形の描画
        glDrawElements(GL_TRIANGLES, cubeVao.indexBufferSize, GL_UNSIGNED_INT, 0);

        // VAOの無効化
        glBindVertexArray(0);

        // シェーダの無効化
        glUseProgram(0);    
    }
}

void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    
    // GLFW管理のウィンドウサイズを変更
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);
    
    // 実際のウィンドウサイズ (ピクセル数) を取得
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);
    
    // ビューポート変換の更新
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

// アニメーションのためのアップデート
void animate() {
    theta += 2.0f * PI / 360.0f;  // 10分の1回転
    phi += 1.0f * PI / 360.0f;
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // OpenGLのバージョン設定 (Macの場合には必ず必要)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);

    // OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
        return 1;
    }

    // バージョンを出力する
    printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);

    // OpenGLを初期化
    initializeGL();

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();

        // アニメーション
        animate();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
