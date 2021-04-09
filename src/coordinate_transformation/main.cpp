﻿#include <cstdio>
#include <cmath>
static const float Pi = std::atan(1.0) * 4.0;

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

static const int WIN_WIDTH   = 500;                 // ウィンドウの幅
static const int WIN_HEIGHT  = 500;                 // ウィンドウの高さ
static const char *WIN_TITLE = "OpenGL Course";     // ウィンドウのタイトル

// 立方体の頂点位置
static const float positions[8][3] = {
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f }
};

// 立方体の面の色
static const float colors[6][3] = {
    { 1.0f, 0.0f, 0.0f },  // 赤
    { 0.0f, 1.0f, 0.0f },  // 緑
    { 0.0f, 0.0f, 1.0f },  // 青
    { 1.0f, 1.0f, 0.0f },  // イエロー
    { 0.0f, 1.0f, 1.0f },  // シアン
    { 1.0f, 0.0f, 1.0f },  // マゼンタ
};

// 立方体の面となる三角形の定義
static const unsigned int indices[12][3] = {
    { 1, 6, 7 }, { 1, 7, 4 },
    { 2, 5, 7 }, { 2, 7, 4 },
    { 3, 5, 7 }, { 3, 7, 6 },
    { 0, 1, 4 }, { 0, 4, 2 },
    { 0, 1, 6 }, { 0, 6, 3 },
    { 0, 2, 5 }, { 0, 5, 3 }
};

// OpenGLの初期化関数
void initializeGL() {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// OpenGLの描画関数
void paintGL() {
    // 背景色の描画
    glClear(GL_COLOR_BUFFER_BIT);

    // ビューポート変換の指定
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

    // 射影変換行列
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 平行投影
    // glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
    // 透視投影
    // glFrustum(-2.0f, 2.0f, -2.0f, 2.0f, 5.0f, 10.0f);
    // 透視投影
    gluPerspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 1.0f, 10.0f);

    // モデルビュー変換行列
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3.0f, 4.0f, 5.0f,     // 視点の位置
              0.0f, 0.0f, 0.0f,     // 見ている先
              0.0f, 1.0f, 0.0f);    // 視界の上方向

    // 立方体の描画
    glBegin(GL_TRIANGLES);
    for (int face = 0; face < 6; face++) {
        // 面の色
        glColor3fv(colors[face]);

        // 1つの面(四角形)は2つの三角形から成る
        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 0][i]]);
        }

        for (int i = 0; i < 3; i++) {
            glVertex3fv(positions[indices[face * 2 + 1][i]]);
        }
    }
    glEnd();
}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

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
    
    // OpenGLを初期化
    initializeGL();

    // メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paintGL();

        // 描画用バッファの切り替え
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
